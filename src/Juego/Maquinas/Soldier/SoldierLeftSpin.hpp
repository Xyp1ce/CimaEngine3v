#pragma once
#include "../FSM.hpp"
#include "Motor/Utils/Vector2D.hpp"

namespace IVJ {
class SoldierLeftSpin : public FSM {
public:
  SoldierLeftSpin(int max_frames = 6, float frame_rate = 0.055f, float mult_velocidad = 2.3f);
  ~SoldierLeftSpin() override {};
  FSM *onInputs(const CE::IControl &control) override;
  void onEntrar(const Entidad &obj) override;
  void onSalir(const Entidad &obj) override;
  void onUpdate(const Entidad &obj, float dt) override;

private:
  sf::Sprite *sprite; // referencia al sprite
  int s_w;            // sprite width
  int s_h;            // sprite height
  float max_tiempo;   // frame rate
  float act_tiempo;   // frame rate actual
  int max_frames;     // frames de animación
  int id_frame;       // frame actual de animación
  bool animacion_terminada{false};
  float mult_velocidad{2.3f};
  CE::Vector2D vel_original{0.f, 0.f};
};
} // namespace IVJ
