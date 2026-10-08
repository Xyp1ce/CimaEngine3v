#include "IdleJugadores.hpp"
#include "MoverJugadores.hpp"

namespace IVJ {
IdleJugadores::IdleJugadores() : FSM{}, sprite{nullptr}, s_w{0}, s_h{0} {
  nombre = "IdleJugador";
}
FSM *IdleJugadores::onInputs(const CE::IControl &control) {
  if (control.arr || control.abj || control.der || control.izq)
    return new MoverJugadores(4, 0.1f);
  return nullptr;
}
void IdleJugadores::onEntrar(const Entidad &obj) {
  sprite = &obj.getComponente<CE::ISprite>()->m_sprite;
  s_w = obj.getComponente<CE::ISprite>()->width;
  s_h = obj.getComponente<CE::ISprite>()->height;
}
void IdleJugadores::onSalir(const Entidad &obj) { (void)obj; }
void IdleJugadores::onUpdate(const Entidad &obj, float dt) {
  (void)obj;
  (void)dt;
  sprite->setTextureRect(sf::IntRect{{// posicion frame 4 al de idle
                                      s_w * 4, 0},
                                     {// tamaño
                                      s_w, s_h}});
}
} // namespace IVJ
