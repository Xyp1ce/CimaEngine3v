#include "SoldierLeft.hpp"
#include "IdleSoldier.hpp"
#include "Motor/Primitivos/GestorAssets.hpp"
#include "SoldierRightSpin.hpp"

namespace IVJ {
SoldierLeft::SoldierLeft(int max_frames, float frame_rate)
    : FSM{}, sprite{nullptr}, s_w{0}, s_h{0}, max_tiempo{frame_rate},
      act_tiempo{frame_rate}, max_frames{max_frames} {
  nombre = "SoldierLeft";
}
FSM *SoldierLeft::onInputs(const CE::IControl &control) {
  // si nada esta presionado regresar a idle
  if (control.sacc)
    return new SoldierRightSpin(6, 0.08f);
  if (!control.izq)
    return new IdleSoldier();

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
