#include "IdleSoldier.hpp"
#include "SoldierDown.hpp"
#include "SoldierDownSpin.hpp"
#include "SoldierLeft.hpp"
#include "SoldierLeftSpin.hpp"
#include "SoldierRight.hpp"
#include "SoldierRightSpin.hpp"
#include "SoldierUp.hpp"
#include "SoldierUpSpin.hpp"
#include "Motor/Primitivos/GestorAssets.hpp"

namespace IVJ {
IdleSoldier::IdleSoldier() : FSM{}, sprite{nullptr}, s_w{0}, s_h{0} {
  nombre = "IdleSoldier";
}
FSM *IdleSoldier::onInputs(const CE::IControl &control) {
  if (control.sacc) {
    if (control.abj)
      return new SoldierDownSpin(6, 0.055f);
    if (control.arr)
      return new SoldierUpSpin(6, 0.055f);
    if (control.izq)
      return new SoldierLeftSpin(6, 0.055f);
    if (control.der)
      return new SoldierRightSpin(6, 0.055f);

    if (sprite) {
      const auto &tex = sprite->getTexture();
      if (&tex == &CE::GestorAssets::Get().getTextura("soldier_up_sheet_K"))
        return new SoldierUpSpin(6, 0.055f);
      if (&tex == &CE::GestorAssets::Get().getTextura("soldier_left_sheet_K"))
        return new SoldierLeftSpin(6, 0.055f);
      if (&tex == &CE::GestorAssets::Get().getTextura("soldier_right_sheet_K"))
        return new SoldierRightSpin(6, 0.055f);
    }
    return new SoldierDownSpin(6, 0.055f);
  }
  if (control.abj)
    return new SoldierDown(4, 0.1f);
  if (control.arr)
    return new SoldierUp(4, 0.1f);
  if (control.izq)
    return new SoldierLeft(4, 0.1f);
  if (control.der)
    return new SoldierRight(4, 0.1f);
  return nullptr;
}
void IdleSoldier::onEntrar(const Entidad &obj) {
  auto comp = obj.getComponente<CE::ISprite>();
  // Mantiene la textura que ya tenía o la resetea a la textura frontal
  sprite = &comp->m_sprite;
  s_w = comp->width;
  s_h = comp->height;
}
void IdleSoldier::onSalir(const Entidad &obj) { (void)obj; }
void IdleSoldier::onUpdate(const Entidad &obj, float dt) {
  (void)obj;
  (void)dt;
  // Mostrar el primer frame (0) en reposo
  sprite->setTextureRect(sf::IntRect{{0, 0}, {s_w, s_h}});
}
} // namespace IVJ
