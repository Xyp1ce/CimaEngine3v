#include "SoldierRight.hpp"
#include "IdleSoldier.hpp"
#include "SoldierDown.hpp"
#include "SoldierDownSpin.hpp"
#include "SoldierLeft.hpp"
#include "SoldierLeftSpin.hpp"
#include "SoldierRightSpin.hpp"
#include "SoldierUp.hpp"
#include "SoldierUpSpin.hpp"
#include "Motor/Primitivos/GestorAssets.hpp"

namespace IVJ {
SoldierRight::SoldierRight(int max_frames, float frame_rate)
    : FSM{}, sprite{nullptr}, s_w{0}, s_h{0}, max_tiempo{frame_rate},
      act_tiempo{frame_rate}, max_frames{max_frames} {
  nombre = "SoldierRight";
}
FSM *SoldierRight::onInputs(const CE::IControl &control) {
  if (control.sacc) {
    if (control.abj)
      return new SoldierDownSpin(6, 0.055f);
    if (control.arr)
      return new SoldierUpSpin(6, 0.055f);
    if (control.izq)
      return new SoldierLeftSpin(6, 0.055f);
    return new SoldierRightSpin(6, 0.055f);
  }
  if (!control.der) {
    if (control.abj)
      return new SoldierDown(4, 0.1f);
    if (control.arr)
      return new SoldierUp(4, 0.1f);
    if (control.izq)
      return new SoldierLeft(4, 0.1f);
    return new IdleSoldier();
  }

  return nullptr;
}
void SoldierRight::onEntrar(const Entidad &obj) {
  auto comp = obj.getComponente<CE::ISprite>();
  comp->m_sprite.setTexture(
      CE::GestorAssets::Get().getTextura("soldier_right_sheet_K"));
  sprite = &comp->m_sprite;
  s_w = comp->width;
  s_h = comp->height;
  id_frame = 0;
}
void SoldierRight::onSalir(const Entidad &obj) { (void)obj; }
void SoldierRight::onUpdate(const Entidad &obj, float dt) {
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
