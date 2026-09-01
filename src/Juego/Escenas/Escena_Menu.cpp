#include "Escena_Menu.hpp"
#include "Motor/Inputs/Botones.hpp"
#include "Motor/Primitivos/GestorAssets.hpp"
#include "Motor/Primitivos/GestorEscenas.hpp"
#include <Juego/Componentes/IJComponentes.hpp>
#include <Juego/Figuras/Figuras.hpp>
#include <Juego/Sistemas/Sistemas.hpp>
#include <Juego/objetos/Texto.hpp>
#include <Motor/Primitivos/GestorEscenas.hpp>
#include <Motor/Render/Render.hpp>
#include <memory>

namespace IVJ {
Escena_Menu::Escena_Menu() : CE::Escena{} {}
void Escena_Menu::onInit() {
  if (!inicializar)
    return;
  registrarBotones(sf::Keyboard::Scancode::W, "arriba");
  registrarBotones(sf::Keyboard::Scancode::Up, "arriba");
  registrarBotones(sf::Keyboard::Scancode::S, "abajo");
  registrarBotones(sf::Keyboard::Scancode::Down, "abajo");
  registrarBotones(sf::Keyboard::Scancode::Enter, "aceptar");
  salto = 48.f;
  opcion = 1u;
  auto marcador =
      std::make_shared<Circulo>(15.f, sf::Color::Green, sf::Color::Black);
  marcador->setPosicion(470.f, 295.f);

  auto strIniciar = std::make_shared<Texto>(
      CE::GestorAssets::Get().getFont("default_font"), "Iniciar");
  strIniciar->setPosicion(510.f, 250.f);
  strIniciar->setColor(sf::Color::Green);

  auto strSalir = std::make_shared<Texto>(
      CE::GestorAssets::Get().getFont("default_font"), "Salir");
  strSalir->setPosicion(510.f, 300.f);

  auto cuadro = std::make_shared<Rectangulo>(150, 150, sf::Color::White,
                                             sf::Color::Black);
  cuadro->addComponente(std::make_shared<IGirar>(3.1416, 5.5f));
  cuadro->setPosicion(500, 300);

  objetos.agregarPool(marcador);
  objetos.agregarPool(strIniciar);
  objetos.agregarPool(strSalir);
  objetos.agregarPool(cuadro);
  inicializar = false;
}
void Escena_Menu::onFinal() {}
void Escena_Menu::onUpdate(float dt) {
  for (auto &obj : objetos.getPool()) {
    obj->onUpdate(dt);
    SistemaGirar(*obj, dt);
  }
}
void Escena_Menu::onInputs(const CE::Botones &accion) {
  switch (accion.getTipo()) {
  case CE::Botones::TipoAccion::OnPress: {
    if (accion.getNombre() == "arriba" || accion.getNombre() == "abajo") {
      auto select = objetos.getPool()[0];
      auto pos = select->getTransformada()->posicion;
      pos.y = (opcion) ? pos.y + salto : pos.y - salto;
      select->setPosicion(pos.x, pos.y);
      opcion ^= 1;
    }

    if (accion.getNombre() == "aceptar") {
      switch (opcion) {
      case 0:
        exit(EXIT_SUCCESS);
        break;
      case 1:
        CE::GestorEscenas::Get().cambiarEscena("Init");
        break;
      }
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
void Escena_Menu::onRender() {
  for (auto &obj : objetos.getPool())
    CE::Render::Get().AddToDraw(*obj);
}
} // namespace IVJ
