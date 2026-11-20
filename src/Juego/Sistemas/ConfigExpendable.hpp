#pragma once
#include <string>

namespace IVJ {

// Configuración única basada en el modo "Difícil 2" del GDD con balanceo de orbes
struct ReglasDificultad {
  std::string nombre{"DIFICIL 2"};
  int hp_jugador{5};               // 5 golpes permitidos (GDD Difícil 2)
  int boss_hp{1800};               // Vida del jefe AEGIS-01
  int prob_permanente_pct{30};     // 30% probabilidad Orbe Verde (permanente)
  int prob_temporal_pct{10};       // 10% probabilidad Orbe Morado (temporal)
  int impactos_para_orbe{5};       // 5 disparos consecutivos contra Aegis
  bool pity_activo{false};         // Sin pity timer (probabilidad pura)
  bool permite_curacion{false};    // Sin curación (GDD Difícil 2)
  float mult_vel_balas_boss{1.15f};
};

class ConfigExpendable {
public:
  static ConfigExpendable &Get() {
    static ConfigExpendable inst;
    return inst;
  }

  ReglasDificultad getReglasActuales() const { return reglas_unicas; }

private:
  ConfigExpendable() = default;
  ReglasDificultad reglas_unicas{};
};

} // namespace IVJ
