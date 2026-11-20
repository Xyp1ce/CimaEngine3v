#pragma once
#include <SFML/Graphics/Color.hpp>
#include <array>
#include <string>
#include <vector>

namespace IVJ {

enum class TipoOrbe {
  PERMANENTE_VERDE, // Mejora permanente (GDD: verde)
  TEMPORAL_MORADO   // Mejora temporal de alto impacto (GDD: morado)
};

enum class MejoraPermanenteId {
  CALIBRE_PLASMA = 0,     // +28% Daño por disparo
  CADENCIA_TURBO = 1,     // +24% Cadencia +12% vel. bala
  SERVOMOTORES = 2,       // +14% Vel. movimiento -15% CD dash
  CARGADOR_EXPANDIDO = 3, // +6 cargador, +60 reserva, -18% recarga
  BALAS_CRITICAS = 4,     // +22% prob. crítico (x1.8 daño)
  BLINDAJE_REACTIVO = 5,  // +1 HP (omitido en Difícil 2)
  COUNT = 6
};

enum class MejoraTemporalId {
  SOBRECARGA_RAFAGA = 0,    // 9.0s: Cadencia x2.2 + munición infinita
  DISPARO_TRIPLE_HELIX = 1, // 10.0s: +2 proyectiles en abanico de plasma
  BALAS_TELEDIGIRIDAS = 2,  // 10.0s: Proyectiles homing hacia el jefe
  ESCUDO_DE_FASE = 3,       // 12.0s: 2 cargas de escudo + onda limpiadora al romperse
  CAMPO_CRONO = 4,          // 8.5s: Balas enemigas al 52% vel. y jugador +18% vel.
  MUNICION_RAILGUN = 5,     // 9.0s: Daño x2.0 + balas pesadas destruyen balas enemigas
  COUNT = 6
};

struct InfoMejora {
  std::string nombre;
  std::string codigo_corto;
  std::string descripcion;
  sf::Color color_acento;
  float duracion{0.f}; // >0 solo para temporales
};

struct BuffTemporalActivo {
  MejoraTemporalId id{MejoraTemporalId::SOBRECARGA_RAFAGA};
  float tiempo_restante{0.f};
  float duracion_total{9.f};
  int cargas_extra{0}; // Usado para Escudo de Fase (2 cargas)
};

struct ResultadoReclamoOrbe {
  TipoOrbe tipo{TipoOrbe::TEMPORAL_MORADO};
  InfoMejora info;
  bool curo_vida{false};
  bool aumento_vida_max{false};
  int recargo_reserva{0};
};

class BuildJugador {
public:
  void reset();

  ResultadoReclamoOrbe aplicarOrbeAleatorio(TipoOrbe tipo, bool permite_curacion,
                                            int &hp_jugador, int &hp_max_jugador);

  void actualizarTemporales(float dt);

  // Consultas de estadísticas efectivas para el combate
  int danioPorBala() const;
  float cadenciaDisparo() const;       // segundos entre disparos
  float velocidadProyectil() const;
  float velocidadMovimiento() const;
  float cooldownDash() const;
  int tamCargadorMax() const;
  float tiempoRecarga() const;
  int cantidadProyectiles() const;     // 1 base, 3 con Triple Helix temporal
  float probCritico() const;           // 0.0 - 0.85
  bool tieneHoming() const;
  bool tieneMunicionInfinita() const;
  bool tieneRailgun() const;
  float escalaVelocidadEnemiga() const; // 1.0 normal, 0.52 con Campo Crono
  bool tieneEscudoFase() const;
  int cargasEscudoFase() const;
  bool consumirCargaEscudo();          // true si absorbió golpe

  // Acceso para HUD y estadísticas de partida
  int getNivelPermanente(MejoraPermanenteId id) const;
  const std::vector<BuffTemporalActivo> &getBuffsTemporales() const {
    return temporales_activos;
  }
  int getTotalVerdes() const { return total_verdes; }
  int getTotalMorados() const { return total_morados; }

  static InfoMejora getInfoPermanente(MejoraPermanenteId id);
  static InfoMejora getInfoTemporal(MejoraTemporalId id);

private:
  std::array<int, static_cast<size_t>(MejoraPermanenteId::COUNT)> permanentes{};
  std::vector<BuffTemporalActivo> temporales_activos;
  int ultimo_verde{-1};
  int ultimo_morado{-1};
  int total_verdes{0};
  int total_morados{0};
};

} // namespace IVJ
