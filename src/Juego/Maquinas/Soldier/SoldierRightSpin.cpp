#include "SoldierRightSpin.hpp"
#include "IdleSoldier.hpp"
#include "Motor/Primitivos/GestorAssets.hpp"

namespace IVJ {
SoldierRightSpin::SoldierRightSpin(int max_frames, float frame_rate)
    : FSM{}, sprite{nullptr}, s_w{0}, s_h{0}, max_tiempo{frame_rate},
      act_tiempo{frame_rate}, max_frames{max_frames} {
  nombre = "SoldierRightSpin";
}
FSM *SoldierRightSpin::onInputs(const CE::IControl &control) {
  (void)control;
  // si nada esta presionado regresar a idle
  if (animacion_terminada)
    return new IdleSoldier();

  return nullptr;
}
void SoldierRightSpin::onEntrar(const Entidad &obj) {
  auto comp = obj.getComponente<CE::ISprite>();
  comp->m_sprite.setTexture(
      CE::GestorAssets::Get().getTextura("soldier_right_spin_sheet_K"));
  sprite = &comp->m_sprite;
  s_w = comp->width;
  s_h = comp->height;
  id_frame = 0;
  animacion_terminada = false;
}
void SoldierRightSpin::onSalir(const Entidad &obj) { (void)obj; }
void SoldierRightSpin::onUpdate(const Entidad &obj, float dt) {
  (void)obj;
  act_tiempo -= dt;
  if (act_tiempo <= 0.f) {
    if (id_frame >= max_frames) {
      animacion_terminada = true;
      return;
    }
    sprite->setTextureRect(sf::IntRect{{s_w * id_frame, 0}, {s_w, s_h}});
    id_frame++;
    act_tiempo = max_tiempo;
  }
}
} // namespace IVJ
