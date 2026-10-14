#include "SoldierRightSpin.hpp"
#include "IdleSoldier.hpp"
#include "SoldierDown.hpp"
#include "SoldierLeft.hpp"
#include "SoldierRight.hpp"
#include "SoldierUp.hpp"
#include "Motor/Primitivos/GestorAssets.hpp"
#include <SFML/Window/Keyboard.hpp>

namespace IVJ {
// Curva de velocidad por fotograma: despegue explosivo, velocidad máxima en el aire y desaceleración al aterrizar
static constexpr float CURVA_ROLL_RIGHT[6] = {2.2f, 2.8f, 3.0f, 2.8f, 1.8f, 1.2f};

SoldierRightSpin::SoldierRightSpin(int max_frames, float frame_rate, float mult_velocidad)
    : FSM{}, sprite{nullptr}, s_w{0}, s_h{0}, max_tiempo{frame_rate},
      act_tiempo{frame_rate}, max_frames{max_frames}, mult_velocidad{mult_velocidad} {
  nombre = "SoldierRightSpin";
}

FSM *SoldierRightSpin::onInputs(const CE::IControl &control) {
  if (animacion_terminada) {
    if (control.der)
      return new SoldierRight(4, 0.1f);
    if (control.izq)
      return new SoldierLeft(4, 0.1f);
    if (control.arr)
      return new SoldierUp(4, 0.1f);
    if (control.abj)
      return new SoldierDown(4, 0.1f);
    return new IdleSoldier();
  }

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

  auto trans = obj.getComponente<CE::ITransform>();
  if (trans) {
    vel_original = trans->velocidad;
    float factor = CURVA_ROLL_RIGHT[0] * (mult_velocidad / 2.3f);
    trans->velocidad.escala(factor);
  }
}

void SoldierRightSpin::onSalir(const Entidad &obj) {
  auto comp = obj.getComponente<CE::ISprite>();
  comp->m_sprite.setTexture(
      CE::GestorAssets::Get().getTextura("soldier_right_sheet_K"));

  auto trans = obj.getComponente<CE::ITransform>();
  if (trans) {
    trans->velocidad = vel_original;
  }

  auto control = obj.getComponente<CE::IControl>();
  if (control) {
    control->abj = sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::S) ||
                   sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Down);
    control->arr = sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::W) ||
                   sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Up);
    control->izq = sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A) ||
                   sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left);
    control->der = sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D) ||
                   sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right);
    control->sacc = sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::E);
  }
}

void SoldierRightSpin::onUpdate(const Entidad &obj, float dt) {
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

  // Modulación dinámica de velocidad por fotograma durante el salto/voltereta
  auto trans = obj.getComponente<CE::ITransform>();
  if (trans) {
    int f = (id_frame < max_frames) ? id_frame : (max_frames - 1);
    float factor = CURVA_ROLL_RIGHT[f] * (mult_velocidad / 2.3f);
    trans->velocidad = vel_original;
    trans->velocidad.escala(factor);
  }

  // Mantener dirección hacia la derecha en controles para SistemaMover
  auto control = obj.getComponente<CE::IControl>();
  if (control) {
    control->setActivo(true);
    control->der = true;
    control->izq = false;
    control->arr = false;
    control->abj = false;
  }
}
} // namespace IVJ
