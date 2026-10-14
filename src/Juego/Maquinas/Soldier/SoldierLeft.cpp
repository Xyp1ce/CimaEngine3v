#include "SoldierLeft.hpp"
#include "IdleSoldier.hpp"
#include "SoldierDown.hpp"
#include "SoldierDownSpin.hpp"
#include "SoldierLeftSpin.hpp"
#include "SoldierRight.hpp"
#include "SoldierRightSpin.hpp"
#include "SoldierUp.hpp"
#include "SoldierUpSpin.hpp"
#include "Motor/Primitivos/GestorAssets.hpp"

namespace IVJ {
SoldierLeft::SoldierLeft(int max_frames, float frame_rate)
    : FSM{}, sprite{nullptr}, s_w{0}, s_h{0}, max_tiempo{frame_rate},
      act_tiempo{frame_rate}, max_frames{max_frames} {
  nombre = "SoldierLeft";
}
FSM *SoldierLeft::onInputs(const CE::IControl &control) {
  if (control.sacc) {
    if (control.abj)
      return new SoldierDownSpin(6, 0.055f);
    if (control.arr)
      return new SoldierUpSpin(6, 0.055f);
    if (control.der)
      return new SoldierRightSpin(6, 0.055f);
    return new SoldierLeftSpin(6, 0.055f);
  }
  if (!control.izq) {
    if (control.abj)
      return new SoldierDown(4, 0.1f);
    if (control.arr)
      return new SoldierUp(4, 0.1f);
    if (control.der)
      return new SoldierRight(4, 0.1f);
    return new IdleSoldier();
  }

  return nullptr;
}
void SoldierLeft::onEntrar(const Entidad &obj) {
  auto comp = obj.getComponente<CE::ISprite>();
  comp->m_sprite.setTexture(
      CE::GestorAssets::Get().getTextura("soldier_left_sheet_K"));
  sprite = &comp->m_sprite;
  s_w = comp->width;
  s_h = comp->height;
  id_frame = 0;
}
void SoldierLeft::onSalir(const Entidad &obj) { (void)obj; }
void SoldierLeft::onUpdate(const Entidad &obj, float dt) {
  (void)obj; // quitar el warning de no usar obj
  act_tiempo = act_tiempo - dt;
  // frame rate
  if (act_tiempo <= 0) {
    // frame mostrando
    sprite->setTextureRect(sf::IntRect{{// posicion id*width = frame actual
                                        s_w * (id_frame % max_frames), 0},
                                       {// tamaño
                                        s_w, s_h}});
    id_frame++;
    act_tiempo = max_tiempo;
  }
}
} // namespace IVJ
