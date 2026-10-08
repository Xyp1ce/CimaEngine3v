#include "AttackBoss.hpp"
#include "IdleBoss.hpp"
#include "Motor/Componentes/IComponentes.hpp"
#include "Motor/Utils/Vector2D.hpp"

namespace IVJ {

AttackBoss::AttackBoss(int max_frames, float frame_rate)
    : FSMBoss{}, max_frames{max_frames}, id_frame{0},
      max_frame_time{frame_rate}, cur_frame_time{frame_rate} {
  // muy importante el nombre es el que vamos utilizar para nuestro facade
  nombre = "AttackBoss";
}
FSM *AttackBoss::onInputs(Entidad &parent, CE::Vector2D &target) {
  auto pos = parent.getTransformada()->posicion;
  float distancia = pos.distancia(target);
  // muy lejos para atacar
  if (distancia > 200.f)
    return new IdleBoss(6, 0.25f);
  return nullptr;
}
void AttackBoss::onEntrar(const Entidad &obj) {
  auto sprite = obj.getComponente<CE::ISprite>();
  t_w = sprite->width;
  t_h = sprite->height;
}
void AttackBoss::onSalir(const Entidad &obj) {
  (void)obj;
  // se usará después con sonidos
}
void AttackBoss::onUpdate(const Entidad &obj, float dt) {
  cur_frame_time -= dt;

  if (cur_frame_time <= 0) {
    obj.getComponente<CE::ISprite>()->m_sprite.setTextureRect(sf::IntRect(
        {
            // pos
            t_w * (id_frame % max_frames),
            1 * t_h // segundo renglón
        },
        {// dimensinones
         t_w, t_h}));
    cur_frame_time = max_frame_time;
    id_frame++;
  }
}
} // namespace IVJ
