#include "IdleSoldier.hpp"
#include "SoldierDown.hpp"
#include "SoldierDownLeftSpin.hpp"
#include "SoldierDownRightSpin.hpp"
#include "SoldierDownSpin.hpp"
#include "SoldierLeft.hpp"
#include "SoldierLeftSpin.hpp"
#include "SoldierRight.hpp"
#include "SoldierRightSpin.hpp"
#include "SoldierUp.hpp"
#include "SoldierUpLeftSpin.hpp"
#include "SoldierUpRightSpin.hpp"
#include "SoldierUpSpin.hpp"
#include "Motor/Primitivos/GestorAssets.hpp"

namespace IVJ {
IdleSoldier::IdleSoldier() : FSM{}, sprite{nullptr}, s_w{0}, s_h{0} {
  nombre = "IdleSoldier";
}
FSM *IdleSoldier::onInputs(const CE::IControl &control) {
  if (control.sacc) {
    if (control.arr && control.der)
      return new SoldierUpRightSpin(6, 0.055f);
    if (control.arr && control.izq)
      return new SoldierUpLeftSpin(6, 0.055f);
    if (control.abj && control.der)
      return new SoldierDownRightSpin(6, 0.055f);
    if (control.abj && control.izq)
      return new SoldierDownLeftSpin(6, 0.055f);

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
      auto &assets = CE::GestorAssets::Get();
      if (&tex == &assets.getTextura("soldier_up_sheet_K") ||
          &tex == &assets.getTextura("soldier_idle_up_sheet_K"))
        return new SoldierUpSpin(6, 0.055f);
      if (&tex == &assets.getTextura("soldier_left_sheet_K") ||
          &tex == &assets.getTextura("soldier_idle_left_sheet_K"))
        return new SoldierLeftSpin(6, 0.055f);
      if (&tex == &assets.getTextura("soldier_right_sheet_K") ||
          &tex == &assets.getTextura("soldier_idle_right_sheet_K"))
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
  sprite = &comp->m_sprite;
  s_w = comp->width;
  s_h = comp->height;
  id_frame = 0;
  act_tiempo = max_tiempo;

  // Mapear la dirección actual a su hoja de animación Idle respirando (4 frames)
  auto &assets = CE::GestorAssets::Get();
  const auto &tex = sprite->getTexture();
  if (&tex == &assets.getTextura("soldier_up_sheet_K") ||
      &tex == &assets.getTextura("soldier_idle_up_sheet_K")) {
    sprite->setTexture(assets.getTextura("soldier_idle_up_sheet_K"));
  } else if (&tex == &assets.getTextura("soldier_left_sheet_K") ||
             &tex == &assets.getTextura("soldier_idle_left_sheet_K")) {
    sprite->setTexture(assets.getTextura("soldier_idle_left_sheet_K"));
  } else if (&tex == &assets.getTextura("soldier_right_sheet_K") ||
             &tex == &assets.getTextura("soldier_idle_right_sheet_K")) {
    sprite->setTexture(assets.getTextura("soldier_idle_right_sheet_K"));
  } else {
    sprite->setTexture(assets.getTextura("soldier_idle_down_sheet_K"));
  }
  sprite->setTextureRect(sf::IntRect{{0, 0}, {s_w, s_h}});
}
void IdleSoldier::onSalir(const Entidad &obj) { (void)obj; }
void IdleSoldier::onUpdate(const Entidad &obj, float dt) {
  (void)obj;
  if (!sprite)
    return;
  act_tiempo -= dt;
  if (act_tiempo <= 0.f) {
    id_frame = (id_frame + 1) % max_frames;
    sprite->setTextureRect(sf::IntRect{{s_w * id_frame, 0}, {s_w, s_h}});
    act_tiempo = max_tiempo;
  }
}
} // namespace IVJ
