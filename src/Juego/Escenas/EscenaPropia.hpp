#pragma once
#include <Juego/objetos/Entidad.hpp>
#include <Juego/objetos/TileMap.hpp>
#include <Motor/Primitivos/Escena.hpp>
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

  // Sistema de Pausa
  bool en_pausa{false};
  int opcion_pausa{0}; // 0: Reanudar, 1: Regresar al menu principal, 2: Regresar al escritorio
  void limpiarControles();
  void resincronizarControles();
  void dispararBala();
  bool estaRodando() const;
};
} // namespace IVJ
