#include "IdleBoss.hpp"
#include "AttackBoss.hpp"
#include "Motor/Componentes/IComponentes.hpp"

namespace IVJ {

IdleBoss::IdleBoss(int max_frames, float frame_rate)
    : FSMBoss{}, max_frames{max_frames}, id_frame{0},
      max_frame_time{frame_rate}, cur_frame_time{frame_rate} {
  // muy importante el nombre es el que vamos utilizar para nuestro facade
  nombre = "IdleBoss";
}
FSM *IdleBoss::onInputs(Entidad &parent, CE::Vector2D &target) {
  auto pos = parent.getTransformada()->posicion;
  float distancia = pos.distancia(target);
  if (distancia < 200.f)
    return new AttackBoss(6, 0.25f);
  return nullptr;
}
void IdleBoss::onEntrar(const Entidad &obj) {
  auto sprite = obj.getComponente<CE::ISprite>();
  t_w = sprite->width;
  t_h = sprite->height;
}
void IdleBoss::onSalir(const Entidad &obj) { (void)obj; }
void IdleBoss::onUpdate(const Entidad &obj, float dt) {
  cur_frame_time -= dt;

  if (cur_frame_time <= 0) {
    obj.getComponente<CE::ISprite>()->m_sprite.setTextureRect(sf::IntRect(
        {
            // pos
            t_w * (id_frame % max_frames),
            0 * t_h // primer renglon
        },
        {// dimensinones
         t_w, t_h}));
    cur_frame_time = max_frame_time;
    id_frame++;
  }
}
} // namespace IVJ
