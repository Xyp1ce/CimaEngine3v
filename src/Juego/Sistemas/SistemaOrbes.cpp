#include "SistemaOrbes.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace IVJ {

void BuildJugador::reset() {
  permanentes.fill(0);
  temporales_activos.clear();
  ultimo_verde = -1;
  ultimo_morado = -1;
  total_verdes = 0;
  total_morados = 0;
}

InfoMejora BuildJugador::getInfoPermanente(MejoraPermanenteId id) {
  switch (id) {
  case MejoraPermanenteId::CALIBRE_PLASMA:
    return {"CALIBRE PLASMA", "DMG+", "+28% Daño por bala",
            sf::Color(70, 255, 130), 0.f};
  case MejoraPermanenteId::CADENCIA_TURBO:
    return {"CADENCIA TURBO", "ROF+", "+24% Cadencia y vel. de disparo",
            sf::Color(60, 245, 175), 0.f};
  case MejoraPermanenteId::SERVOMOTORES:
    return {"SERVOMOTORES", "SPD+", "+14% Velocidad y -15% CD Dash",
            sf::Color(90, 255, 110), 0.f};
  case MejoraPermanenteId::CARGADOR_EXPANDIDO:
    return {"CARGADOR EXPANDIDO", "MAG+", "+6 Cargador, +60 Reserva, recarga rapida",
            sf::Color(120, 240, 95), 0.f};
  case MejoraPermanenteId::BALAS_CRITICAS:
    return {"NUCLEO CRITICO", "CRIT+", "+22% Prob. Critico (x1.8 daño)",
            sf::Color(150, 255, 90), 0.f};
  case MejoraPermanenteId::BLINDAJE_REACTIVO:
    return {"BLINDAJE REACTIVO", "HP+", "+1 Integridad de Armadura",
            sf::Color(50, 235, 120), 0.f};
  default:
    return {"MEJORA", "UPG", "Mejora permanente", sf::Color(60, 235, 100), 0.f};
  }
}

InfoMejora BuildJugador::getInfoTemporal(MejoraTemporalId id) {
  switch (id) {
  case MejoraTemporalId::SOBRECARGA_RAFAGA:
    return {"SOBRECARGA DE RAFAGA", "OVERDRIVE", "Cadencia x2.2 + Municion Infinita",
            sf::Color(215, 95, 255), 9.0f};
  case MejoraTemporalId::DISPARO_TRIPLE_HELIX:
    return {"TRIPLE HELIX", "3X SHOT", "+2 Proyectiles en abanico de plasma",
            sf::Color(195, 75, 255), 10.0f};
  case MejoraTemporalId::BALAS_TELEDIGIRIDAS:
    return {"MUNICION HOMING", "HOMING", "Proyectiles teledirigidos al jefe",
            sf::Color(175, 110, 255), 10.0f};
  case MejoraTemporalId::ESCUDO_DE_FASE:
    return {"ESCUDO DE FASE", "SHIELD", "2 Cargas de escudo + Onda limpia-balas",
            sf::Color(235, 115, 255), 12.0f};
  case MejoraTemporalId::CAMPO_CRONO:
    return {"CAMPO CRONO", "CHRONO", "Ralentiza balas enemigas 48% + Velocidad",
            sf::Color(165, 90, 255), 8.5f};
  case MejoraTemporalId::MUNICION_RAILGUN:
    return {"CARGA RAILGUN", "RAILGUN", "Daño x2.0 + Vaporiza balas naranjas",
            sf::Color(225, 70, 255), 9.0f};
  default:
    return {"SOBRECARGA", "TEMP", "Mejora temporal", sf::Color(195, 65, 255), 9.0f};
  }
}

ResultadoReclamoOrbe BuildJugador::aplicarOrbeAleatorio(TipoOrbe tipo,
                                                        bool permite_curacion,
                                                        int &hp_jugador,
                                                        int &hp_max_jugador) {
  ResultadoReclamoOrbe res;
  res.tipo = tipo;

  if (tipo == TipoOrbe::PERMANENTE_VERDE) {
    total_verdes++;
    std::vector<MejoraPermanenteId> candidatos;
    for (int i = 0; i < static_cast<int>(MejoraPermanenteId::COUNT); ++i) {
      auto id = static_cast<MejoraPermanenteId>(i);
      // En Difícil 2 el GDD prohíbe curación
      if (id == MejoraPermanenteId::BLINDAJE_REACTIVO && !permite_curacion)
        continue;
      // Limitar Crítico a máx 4 niveles
      if (id == MejoraPermanenteId::BALAS_CRITICAS &&
          permanentes[static_cast<size_t>(id)] >= 4)
        continue;
      // Limitar Blindaje si ya tiene 10 HP máximo
      if (id == MejoraPermanenteId::BLINDAJE_REACTIVO &&
          hp_jugador >= hp_max_jugador && hp_max_jugador >= 10)
        continue;
      candidatos.push_back(id);
    }

    if (candidatos.empty()) {
      candidatos.push_back(MejoraPermanenteId::CALIBRE_PLASMA);
    }

    // Evitar repetir exactamente el mismo orbe verde consecutivo si hay varias opciones
    MejoraPermanenteId elegido = candidatos[std::rand() % candidatos.size()];
    if (candidatos.size() > 1 && static_cast<int>(elegido) == ultimo_verde) {
      elegido = candidatos[(std::rand() % candidatos.size())];
      if (static_cast<int>(elegido) == ultimo_verde) {
        for (auto c : candidatos) {
          if (static_cast<int>(c) != ultimo_verde) {
            elegido = c;
            break;
          }
        }
      }
    }

    ultimo_verde = static_cast<int>(elegido);
    permanentes[static_cast<size_t>(elegido)]++;
    res.info = getInfoPermanente(elegido);

    if (elegido == MejoraPermanenteId::BLINDAJE_REACTIVO) {
      if (hp_jugador < hp_max_jugador) {
        hp_jugador++;
        res.curo_vida = true;
      } else {
        hp_max_jugador = std::min(10, hp_max_jugador + 1);
        hp_jugador = hp_max_jugador;
        res.aumento_vida_max = true;
      }
    } else if (elegido == MejoraPermanenteId::CARGADOR_EXPANDIDO) {
      res.recargo_reserva = 60;
    }
    return res;
  }

  // Orbe Morado (Mejora Temporal de alto impacto)
  total_morados++;
  std::vector<MejoraTemporalId> candidatos;
  for (int i = 0; i < static_cast<int>(MejoraTemporalId::COUNT); ++i) {
    candidatos.push_back(static_cast<MejoraTemporalId>(i));
  }

  MejoraTemporalId elegido = candidatos[std::rand() % candidatos.size()];
  if (candidatos.size() > 1 && static_cast<int>(elegido) == ultimo_morado) {
    for (int intento = 0; intento < 6; ++intento) {
      auto c = candidatos[std::rand() % candidatos.size()];
      if (static_cast<int>(c) != ultimo_morado) {
        elegido = c;
        break;
      }
    }
  }
  ultimo_morado = static_cast<int>(elegido);
  res.info = getInfoTemporal(elegido);

  // Si ya está activo, refrescar duración (y cargas de escudo)
  bool encontrado = false;
  for (auto &b : temporales_activos) {
    if (b.id == elegido) {
      b.tiempo_restante = res.info.duracion;
      b.duracion_total = res.info.duracion;
      if (elegido == MejoraTemporalId::ESCUDO_DE_FASE) {
        b.cargas_extra = 2;
      }
      encontrado = true;
      break;
    }
  }
  if (!encontrado) {
    BuffTemporalActivo nuevo;
    nuevo.id = elegido;
    nuevo.tiempo_restante = res.info.duracion;
    nuevo.duracion_total = res.info.duracion;
    nuevo.cargas_extra = (elegido == MejoraTemporalId::ESCUDO_DE_FASE) ? 2 : 0;
    temporales_activos.push_back(nuevo);
  }
  return res;
}

void BuildJugador::actualizarTemporales(float dt) {
  for (auto it = temporales_activos.begin(); it != temporales_activos.end();) {
    it->tiempo_restante -= dt;
    if (it->tiempo_restante <= 0.f ||
        (it->id == MejoraTemporalId::ESCUDO_DE_FASE && it->cargas_extra <= 0)) {
      it = temporales_activos.erase(it);
    } else {
      ++it;
    }
  }
}

int BuildJugador::getNivelPermanente(MejoraPermanenteId id) const {
  return permanentes[static_cast<size_t>(id)];
}

int BuildJugador::danioPorBala() const {
  float base = 10.0f;
  int lvl_dmg = getNivelPermanente(MejoraPermanenteId::CALIBRE_PLASMA);
  float mult = 1.0f + 0.28f * static_cast<float>(lvl_dmg);
  if (tieneRailgun()) {
    mult *= 2.0f;
  }
  return std::max(1, static_cast<int>(std::round(base * mult)));
}

float BuildJugador::cadenciaDisparo() const {
  // Cadencia base reducida en un 30% respecto a 4.8 rps (4.8 * 0.70 = 3.36 rps)
  float base_rps = 3.36f;
  int lvl_rof = getNivelPermanente(MejoraPermanenteId::CADENCIA_TURBO);
  float rps = base_rps * (1.0f + 0.24f * static_cast<float>(lvl_rof));
  if (tieneMunicionInfinita()) {
    rps *= 2.15f;
  }
  rps = std::min(rps, 18.0f);
  return 1.0f / rps;
}

float BuildJugador::velocidadProyectil() const {
  int lvl_rof = getNivelPermanente(MejoraPermanenteId::CADENCIA_TURBO);
  int lvl_crit = getNivelPermanente(MejoraPermanenteId::BALAS_CRITICAS);
  float vel = 900.f * (1.0f + 0.10f * lvl_rof + 0.08f * lvl_crit);
  if (tieneRailgun())
    vel *= 1.25f;
  return std::min(vel, 1500.f);
}

float BuildJugador::velocidadMovimiento() const {
  int lvl_spd = getNivelPermanente(MejoraPermanenteId::SERVOMOTORES);
  float vel = 340.f * (1.0f + 0.14f * static_cast<float>(lvl_spd));
  if (escalaVelocidadEnemiga() < 0.99f) {
    vel *= 1.18f;
  }
  return std::min(vel, 540.f);
}

float BuildJugador::cooldownDash() const {
  int lvl_spd = getNivelPermanente(MejoraPermanenteId::SERVOMOTORES);
  float cd = 0.42f * std::pow(0.85f, static_cast<float>(lvl_spd));
  return std::max(0.16f, cd);
}

int BuildJugador::tamCargadorMax() const {
  int lvl_mag = getNivelPermanente(MejoraPermanenteId::CARGADOR_EXPANDIDO);
  return 24 + lvl_mag * 6;
}

float BuildJugador::tiempoRecarga() const {
  int lvl_mag = getNivelPermanente(MejoraPermanenteId::CARGADOR_EXPANDIDO);
  float t = 1.45f * std::pow(0.82f, static_cast<float>(lvl_mag));
  return std::max(0.55f, t);
}

int BuildJugador::cantidadProyectiles() const {
  for (const auto &b : temporales_activos) {
    if (b.id == MejoraTemporalId::DISPARO_TRIPLE_HELIX) {
      return 3;
    }
  }
  return 1;
}

float BuildJugador::probCritico() const {
  int lvl_crit = getNivelPermanente(MejoraPermanenteId::BALAS_CRITICAS);
  return std::min(0.85f, 0.05f + 0.22f * static_cast<float>(lvl_crit));
}

bool BuildJugador::tieneHoming() const {
  for (const auto &b : temporales_activos) {
    if (b.id == MejoraTemporalId::BALAS_TELEDIGIRIDAS)
      return true;
  }
  return false;
}

bool BuildJugador::tieneMunicionInfinita() const {
  for (const auto &b : temporales_activos) {
    if (b.id == MejoraTemporalId::SOBRECARGA_RAFAGA)
      return true;
  }
  return false;
}

bool BuildJugador::tieneRailgun() const {
  for (const auto &b : temporales_activos) {
    if (b.id == MejoraTemporalId::MUNICION_RAILGUN)
      return true;
  }
  return false;
}

float BuildJugador::escalaVelocidadEnemiga() const {
  for (const auto &b : temporales_activos) {
    if (b.id == MejoraTemporalId::CAMPO_CRONO)
      return 0.52f;
  }
  return 1.0f;
}

bool BuildJugador::tieneEscudoFase() const {
  return cargasEscudoFase() > 0;
}

int BuildJugador::cargasEscudoFase() const {
  for (const auto &b : temporales_activos) {
    if (b.id == MejoraTemporalId::ESCUDO_DE_FASE)
      return b.cargas_extra;
  }
  return 0;
}

bool BuildJugador::consumirCargaEscudo() {
  for (auto it = temporales_activos.begin(); it != temporales_activos.end(); ++it) {
    if (it->id == MejoraTemporalId::ESCUDO_DE_FASE && it->cargas_extra > 0) {
      it->cargas_extra--;
      if (it->cargas_extra <= 0) {
        temporales_activos.erase(it);
      }
      return true;
    }
  }
  return false;
}

} // namespace IVJ
