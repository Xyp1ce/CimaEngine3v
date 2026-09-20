#include "GHistograma.hpp"
#include <Juego/Componentes/IJComponentes.hpp>
#include <Juego/Escenas/Escena_Sim.hpp>
#include <Motor/Primitivos/GestorEscenas.hpp>
#include <algorithm>
#include <cmath>
#include <imgui.h>
#include <memory>
#include <vector>

namespace CE {
void GHistograma::OnInit(const MotorConfig &des) { (void)des; }
void GHistograma::OnUpdate(float dt) {
  (void)dt; // Agrega esta línea para evitar el error de variable no utilizada

  if (total ==
      getEntesSim(
          CE::GestorEscenas::Get().getEscenaActual().getPool().getPool()))
    return;
  total = this->getEntesSim(
      CE::GestorEscenas::Get().getEscenaActual().getPool().getPool());
  histograma = crearHistograma(
      CE::GestorEscenas::Get().getEscenaActual().getPool().getPool(), 10);
}
void GHistograma::OnRender(void) {
  if (ImGui::Begin("Estadísticas")) {

    char overlay[64];
    snprintf(overlay, sizeof(overlay), "n=%zu  [%.2f, %.2f]", total,
             histograma.minVal, histograma.maxVal);

    ImGui::PlotHistogram("##hist", histograma.bins.data(),
                         (int)histograma.bins.size(),
                         0,                                     // offset
                         overlay, 0.0f, histograma.peak * 1.1f, // escala Y
                         ImVec2(-1, 150)); // -1 = ancho disponible
  }
  ImGui::End();
}
size_t GHistograma::getEntesSim(
    const std::vector<std::shared_ptr<CE::Objeto>> &objetos) {
  size_t cuenta = 0;
  for (auto &ente : objetos) {
    if (!ente->tieneComponente<IVJ::IScore>())
      continue;

    cuenta++;
  }
  return cuenta;
}
GHistograma::Histograma GHistograma::crearHistograma(
    const std::vector<std::shared_ptr<CE::Objeto>> &objetos, int bins) {
  GHistograma::Histograma hist;

  if (objetos.size() <= 0 || bins <= 0)
    return hist;
  std::vector<int> data;
  for (auto &ente : objetos) {
    if (!ente->tieneComponente<IVJ::IScore>())
      continue;
    data.push_back(ente->getComponente<IVJ::IScore>()->score);
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
    int idx = data[i];
    // en teoria se debe de interpolar el valor y ver en que lote cae
    // pero en nuestro caso con 10 lotes es suficiente
    //(int)(data[i] - hist.minVal) / hist.binWidth;
    // idx = std::clamp(idx, 0, bins - 1);
    hist.bins[idx]++;
  }

  hist.peak = *std::max_element(hist.bins.begin(), hist.bins.end());
  return hist;
}

} // namespace CE
