#pragma once
#include "../FSM.hpp"
namespace IVJ {
class SoldierRight : public FSM {
public:
  SoldierRight(int max_frames, float frame_rate);
  ~SoldierRight() override {};
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
};
} // namespace IVJ
