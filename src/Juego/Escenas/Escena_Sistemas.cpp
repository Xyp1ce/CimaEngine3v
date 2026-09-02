#include "Juego/Escenas/Escena_Sistemas.hpp"
#include "Escena_Sistemas.hpp"
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
Escena_Sistemas::Escena_Sistemas() : CE::Escena{} {}
void Escena_Sistemas::onInit() {
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

  auto cuadro_vertical = std::make_shared<Rectangulo>(
      150, 150, sf::Color::White, sf::Color::Black);
  cuadro_vertical->addComponente(std::make_shared<IVertical>(3.1416, 5.f));
  cuadro_vertical->setPosicion(800, 200);

  auto cuadro_circulo = std::make_shared<Rectangulo>(150, 150, sf::Color::White,
                                                     sf::Color::Black);
  cuadro_circulo->addComponente(std::make_shared<IGirar>(3.1416, 6.f));
  cuadro_circulo->setPosicion(500, 300);

  auto cuadro_onda = std::make_shared<Rectangulo>(150, 150, sf::Color::White,
                                                  sf::Color::Black);
  cuadro_onda->addComponente(std::make_shared<IOnda>(3.1416, 6.f, 1));
  cuadro_onda->setPosicion(200, 500);

  objetos.agregarPool(marcador);
  objetos.agregarPool(strIniciar);
  objetos.agregarPool(strSalir);
  objetos.agregarPool(cuadro_vertical);
  objetos.agregarPool(cuadro_circulo);
  objetos.agregarPool(cuadro_onda);
  inicializar = false;
}
void Escena_Sistemas::onFinal() {}
void Escena_Sistemas::onUpdate(float dt) {
  for (auto &obj : objetos.getPool()) {
    obj->onUpdate(dt);
    SistemaGirar(*obj, dt);
    SistemaVertical(*obj, dt);
    SistemaOnda(*obj, dt);
  }
}
void Escena_Sistemas::onInputs(const CE::Botones &accion) {
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
void Escena_Sistemas::onRender() {
  for (auto &obj : objetos.getPool())
    CE::Render::Get().AddToDraw(*obj);
}
} // namespace IVJ
