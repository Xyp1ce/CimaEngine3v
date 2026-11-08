#pragma once
#include <Juego/objetos/Entidad.hpp>
#include <Juego/objetos/TileMap.hpp>
#include <Motor/Primitivos/Escena.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <optional>
#include <vector>

namespace IVJ {

struct ProyectilBala {
  std::shared_ptr<Entidad> entidad;
  CE::Vector2D direccion;
  float velocidad{850.f};
  float tiempo_vida{2.5f};
  float tiempo_transcurrido{0.f};
};

struct EnemigoCuadro {
  std::shared_ptr<Entidad> entidad;
  sf::RectangleShape cuadro{{56.f, 56.f}};
  int vida_max{200};
  int vida{200};
  bool vivo{true};
};

enum class TipoOrbe {
  PERMANENTE_VERDE, // 1/9 (~11.1%) - orbe_verde_K
  TEMPORAL_MORADO   // 1/4 (25.0%)  - orbe_morado_K
};

struct OrbeMejora {
  std::shared_ptr<Entidad> entidad;
  TipoOrbe tipo{TipoOrbe::TEMPORAL_MORADO};
  int disparos_restantes{4};      // Resistencia fija de 4 disparos del jugador
  float tiempo_vida_max{15.0f};   // 15 segundos antes de expirar
  float tiempo_transcurrido{0.f}; // Tiempo activo en la arena
};

class Escena_Propia : public CE::Escena {
public:
  explicit Escena_Propia(std::shared_ptr<Entidad> &pref);
  virtual ~Escena_Propia() {};
  void onInit() override;
  void onFinal() override;
  void onUpdate(float dt) override;
  void onInputs(const CE::Botones &accion) override;
  void onRender() override;
  std::shared_ptr<Entidad> getJugador() override { return jugador_soldier; }

private:
  int inicializar{1};
  std::shared_ptr<Entidad> &jugador_soldier;
  std::vector<TileMap> tiles_industrial;
  std::vector<ProyectilBala> proyectiles;
  float tiempo_ultimo_disparo{1.0f / 3.0f};
  const float cadencia_disparo{1.0f / 3.0f}; // 3 balas por segundo

  // Sistema de Munición y Arma HUD
  const int balas_cargador_max{24};
  const int balas_reserva_max{360};
  int balas_cargador{24};
  int balas_reserva{360};
  std::optional<sf::Sprite> sprite_arma_hud;
  void recargarArma();
  bool iniciarRecarga();

  // Sistema de Recarga
  bool recargando{false};
  float tiempo_recarga_actual{0.0f};
  const float tiempo_recarga_total{2.0f};

  // Enemigo de prueba (cuadro rojo sin IA aún)
  EnemigoCuadro enemigo_rojo;
  const int danio_bala{5};

  // Sistema de Orbes de Mejora (GDD)
  std::vector<OrbeMejora> orbes;
  int danio_continuo_jefe{0};
  const int umbral_danio_drops{15}; // Daño continuo requerido (x) antes de habilitar drops
  int impactos_consecutivos_jefe{0};
  const int impactos_requeridos_drop{3}; // Ráfaga de 3 impactos seguidos tras alcanzar el umbral
  int fallos_rng_orbe{0};
  const int max_fallos_pity{3}; // Pity timer garantizado a los 3 chequeos fallidos (50/50)
  void evaluarDropOrbe();
  void generarOrbe(TipoOrbe tipo);
  void reclamarOrbe(const OrbeMejora &orbe);

  // Sistema de Pausa
  bool en_pausa{false};
  int opcion_pausa{0}; // 0: Reanudar, 1: Regresar al menu principal, 2: Regresar al escritorio
  void limpiarControles();
  void resincronizarControles();
  void dispararBala();
  bool estaRodando() const;
};
} // namespace IVJ
