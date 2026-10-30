#include "SoldierUpLeftSpin.hpp"
#include "IdleSoldier.hpp"
#include "SoldierDown.hpp"
#include "SoldierLeft.hpp"
#include "SoldierRight.hpp"
#include "SoldierUp.hpp"
#include "Motor/Primitivos/GestorAssets.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

namespace IVJ {
// Curva de velocidad por fotograma modulada para movimiento diagonal normalizado (1 / sqrt(2) ≈ 0.7071)
static constexpr float CURVA_ROLL_UP_LEFT[6] = {2.2f, 2.8f, 3.0f, 2.8f, 1.8f, 1.2f};
static constexpr float INV_SQRT2 = 0.70710678f;

SoldierUpLeftSpin::SoldierUpLeftSpin(int max_frames, float frame_rate, float mult_velocidad)
    : FSM{}, sprite{nullptr}, s_w{0}, s_h{0}, max_tiempo{frame_rate},
      act_tiempo{frame_rate}, max_frames{max_frames}, mult_velocidad{mult_velocidad} {
  nombre = "SoldierUpLeftSpin";
}

FSM *SoldierUpLeftSpin::onInputs(const CE::IControl &control) {
  if (animacion_terminada) {
    if (control.arr && control.izq)
      return new SoldierLeft(4, 0.1f);
    if (control.arr && control.der)
      return new SoldierRight(4, 0.1f);
    if (control.abj && control.izq)
      return new SoldierLeft(4, 0.1f);
    if (control.abj && control.der)
      return new SoldierRight(4, 0.1f);

    if (control.arr)
      return new SoldierUp(4, 0.1f);
    if (control.izq)
      return new SoldierLeft(4, 0.1f);
    if (control.der)
      return new SoldierRight(4, 0.1f);
    if (control.abj)
      return new SoldierDown(4, 0.1f);

    return new IdleSoldier();
  }

  return nullptr;
}

void SoldierUpLeftSpin::onEntrar(const Entidad &obj) {
  auto comp = obj.getComponente<CE::ISprite>();
  comp->m_sprite.setTexture(
      CE::GestorAssets::Get().getTextura("soldier_left_spin_sheet_K"));
  sprite = &comp->m_sprite;
  s_w = comp->width;
  s_h = comp->height;
  id_frame = 0;
  animacion_terminada = false;

  auto trans = obj.getComponente<CE::ITransform>();
  if (trans) {
    vel_original = trans->velocidad;
    float factor = CURVA_ROLL_UP_LEFT[0] * (mult_velocidad / 2.3f) * INV_SQRT2;
    trans->velocidad.escala(factor);
  }
}

void SoldierUpLeftSpin::onSalir(const Entidad &obj) {
  auto comp = obj.getComponente<CE::ISprite>();
  comp->m_sprite.setTexture(
      CE::GestorAssets::Get().getTextura("soldier_left_sheet_K"));

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
    control->sacc = sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);
  }
}

void SoldierUpLeftSpin::onUpdate(const Entidad &obj, float dt) {
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

  // Modulación dinámica de velocidad por fotograma normalizada para movimiento diagonal
  auto trans = obj.getComponente<CE::ITransform>();
  if (trans) {
    int f = (id_frame < max_frames) ? id_frame : (max_frames - 1);
    float factor = CURVA_ROLL_UP_LEFT[f] * (mult_velocidad / 2.3f) * INV_SQRT2;
    trans->velocidad = vel_original;
    trans->velocidad.escala(factor);
  }

  // Mantener dirección diagonal hacia arriba-izquierda en controles para SistemaMover
  auto control = obj.getComponente<CE::IControl>();
  if (control) {
    control->setActivo(true);
    control->arr = true;
    control->izq = true;
    control->der = false;
    control->abj = false;
  }
}
} // namespace IVJ
