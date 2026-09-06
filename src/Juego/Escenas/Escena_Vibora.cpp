#include "Escena_Vibora.hpp"
#include "Motor/Inputs/Botones.hpp"
#include "Motor/Render/Render.hpp"
#include <Juego/Componentes/IJComponentes.hpp> 
#include <Juego/Figuras/Figuras.hpp>

namespace IVJ {
Escena_Vibora::Escena_Vibora() : CE::Escena{} {}

void Escena_Vibora::onInit() {
  if (!inicializar)
    return;
    
  // Registramos los botones de movimiento (WASD y flechas)[cite: 6]
  registrarBotones(sf::Keyboard::Scancode::W, "arriba");
  registrarBotones(sf::Keyboard::Scancode::Up, "arriba");
  registrarBotones(sf::Keyboard::Scancode::S, "abajo");
  registrarBotones(sf::Keyboard::Scancode::Down, "abajo");
  registrarBotones(sf::Keyboard::Scancode::A, "izquierda");
  registrarBotones(sf::Keyboard::Scancode::Left, "izquierda");
  registrarBotones(sf::Keyboard::Scancode::D, "derecha");
  registrarBotones(sf::Keyboard::Scancode::Right, "derecha");

  // 5.- En la escena, instanciar el EnteVibora
  vibora = std::make_shared<EnteVibora>(20.f, sf::Color::Green, sf::Color::White);
  vibora->setPosicion(400.f, 300.f);

  // Agregar el componente ICPartesCuerpo
  auto partesCuerpo = std::make_shared<ICPartesCuerpo>(30, 30);
  vibora->addComponente(partesCuerpo);

  // Iniciar con 3 partes
  vibora->agregarNuevaParte(partesCuerpo.get());
  vibora->agregarNuevaParte(partesCuerpo.get());
  vibora->agregarNuevaParte(partesCuerpo.get());

  // Agregamos la víbora al gestor de la escena[cite: 6]
  objetos.agregarPool(vibora);
  
  inicializar = false;
}

void Escena_Vibora::onFinal() {}

void Escena_Vibora::onUpdate(float dt) {
  // Actualizamos todos los objetos en la escena[cite: 6]
  for (auto &obj : objetos.getPool()) {
    obj->onUpdate(dt);
  }
}

void Escena_Vibora::onInputs(const CE::Botones &accion) {
  switch (accion.getTipo()) {
  case CE::Botones::TipoAccion::OnPress: {
    if (!vibora) break;

    // Cambiamos la dirección de la víbora según el input[cite: 6]
    if (accion.getNombre() == "arriba") {
      vibora->setDireccion(0, -1);
    } else if (accion.getNombre() == "abajo") {
      vibora->setDireccion(0, 1);
    } else if (accion.getNombre() == "izquierda") {
      vibora->setDireccion(-1, 0);
    } else if (accion.getNombre() == "derecha") {
      vibora->setDireccion(1, 0);
    }
    break;
  }
  case CE::Botones::TipoAccion::OnRelease: {
    break;
  }
  case CE::Botones::TipoAccion::None: {
    break;
  }
  }
}

void Escena_Vibora::onRender() {
  // Renderizamos los objetos de la escena[cite: 6]
  for (auto &obj : objetos.getPool())
    CE::Render::Get().AddToDraw(*obj);
}
} // namespace IVJ
