#pragma once
#include "../FSM.hpp"
namespace IVJ {
class IdleSoldier : public FSM {
public:
  // como solo sera 1 frame, no es necesario
  // llevar control de máximos frames y framerate
  IdleSoldier();
  ~IdleSoldier() override {};
  FSM *onInputs(const CE::IControl &control) override;
  void onEntrar(const Entidad &obj) override;
  void onSalir(const Entidad &obj) override;
  void onUpdate(const Entidad &obj, float dt) override;

private:
  sf::Sprite *sprite;
  int s_w;
  int s_h;
};
} // namespace IVJ
