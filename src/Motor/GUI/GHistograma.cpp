#include "GHistograma.hpp"
#include <Juego/Componentes/IJComponentes.hpp>
#include <Juego/Escenas/Escena_Sim.hpp>
#include <Motor/Primitivos/GestorEscenas.hpp>
#include <algorithm>
#include <cmath>
#include <imgui.h>

namespace CE {
void GHistograma::OnInit(const MotorConfig &des) { (void)des; }

void GHistograma::OnUpdate(float dt) {
  (void)dt;
  auto pool_actual =
      CE::GestorEscenas::Get().getEscenaActual().getPool().getPool();
  if (total == getEntesSim(pool_actual))
    return;

  total = this->getEntesSim(pool_actual);

  // Generar ambos histogramas
  hist_agi = crearHistogramaStats(pool_actual, 10, true);
  hist_hp = crearHistogramaStats(pool_actual, 10, false);
}

void GHistograma::OnRender(void) {
  if (ImGui::Begin("Estadísticas de Evolución")) {

    // Panel AGI
    ImGui::Text("Rango de Visión (AGI)");
    char overlay_agi[64];
    snprintf(overlay_agi, sizeof(overlay_agi), "n=%zu  [%.2f, %.2f]", total,
             hist_agi.minVal, hist_agi.maxVal);
    ImGui::PlotHistogram("##histAgi", hist_agi.bins.data(),
                         (int)hist_agi.bins.size(), 0, overlay_agi, 0.0f,
                         hist_agi.peak * 1.1f, ImVec2(-1, 100));

    ImGui::Separator();

    // Panel HP
    ImGui::Text("Energía Máxima (HP_MAX)");
    char overlay_hp[64];
    snprintf(overlay_hp, sizeof(overlay_hp), "n=%zu  [%.2f, %.2f]", total,
             hist_hp.minVal, hist_hp.maxVal);
    ImGui::PlotHistogram("##histHp", hist_hp.bins.data(),
                         (int)hist_hp.bins.size(), 0, overlay_hp, 0.0f,
                         hist_hp.peak * 1.1f, ImVec2(-1, 100));
  }
  ImGui::End();
}

size_t GHistograma::getEntesSim(
    const std::vector<std::shared_ptr<CE::Objeto>> &objetos) {
  size_t cuenta = 0;
  for (auto &ente : objetos) {
    if (ente->tieneComponente<IVJ::IScore>())
      cuenta++;
  }
  return cuenta;
}

GHistograma::Histograma GHistograma::crearHistogramaStats(
    const std::vector<std::shared_ptr<CE::Objeto>> &objetos, int bins,
    bool isAgi) {

  GHistograma::Histograma hist;
  if (objetos.size() <= 0 || bins <= 0)
    return hist;

  std::vector<int> data;
  for (auto &ente : objetos) {
    if (!ente->tieneComponente<IVJ::IScore>())
      continue;

    // Distinguir qué estadística extraer
    if (isAgi) {
      data.push_back(ente->getStats()->agi);
    } else {
      data.push_back(ente->getStats()->hp_max);
    }
  }

  if (data.size() <= 0)
    return hist;

  auto mm = std::minmax_element(data.begin(), data.end());
  hist.minVal = *mm.first;
  hist.maxVal = *mm.second;
  if (hist.maxVal <= hist.minVal)
    hist.maxVal = hist.minVal + 1.f;

  hist.binWidth = (hist.maxVal - hist.minVal) / bins;
  hist.bins.assign(bins, 0.0f);

  for (size_t i = 0; i < total; i++) {
    int idx =
        std::clamp((int)((data[i] - hist.minVal) / hist.binWidth), 0, bins - 1);
    hist.bins[idx]++;
  }

  hist.peak = *std::max_element(hist.bins.begin(), hist.bins.end());
  return hist;
}
} // namespace CE
