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

  // 5.- En la escena, instanciar el EnteVibora
  vibora =
      std::make_shared<EnteVibora>(20.f, sf::Color::Green, sf::Color::White);
  vibora->setPosicion(400.f, 300.f);

  auto partesCuerpo = std::make_shared<ICPartesCuerpo>(30, 30);
  vibora->addComponente(partesCuerpo);

  vibora->agregarNuevaParte(partesCuerpo.get());
  vibora->agregarNuevaParte(partesCuerpo.get());
  vibora->agregarNuevaParte(partesCuerpo.get());

  objetos.agregarPool(vibora);
  inicializar = false;
}

void Escena_Vibora::onFinal() {}

void Escena_Vibora::onUpdate(float dt) {
  for (auto &obj : objetos.getPool()) {
    obj->onUpdate(dt);
  }
}

// Ya no procesamos teclas para mover a la víbora, se mueve sola
void Escena_Vibora::onInputs(const CE::Botones &accion) {
  (void)accion; // Evita el warning de "variable sin usar"
}

void Escena_Vibora::onRender() {
  for (auto &obj : objetos.getPool())
    CE::Render::Get().AddToDraw(*obj);
}
} // namespace IVJ
