#include "Escena_Camara.hpp"
#include "Motor/Utils/Vector2D.hpp"
#include <Juego/Componentes/IJComponentes.hpp>
#include <Juego/Figuras/Figuras.hpp>
#include <Juego/Sistemas/Sistemas.hpp>
#include <Juego/objetos/Entidad.hpp>
#include <Motor/Camaras/Camaras.hpp>
#include <Motor/Camaras/CamarasGestor.hpp>
#include <Motor/Componentes/IComponentes.hpp>
#include <Motor/Inputs/Botones.hpp>
#include <Motor/Render/Render.hpp>
#include <cmath>
#include <memory>
namespace IVJ {
Escena_Camara::Escena_Camara(std::shared_ptr<Entidad> &pref)
    : CE::Escena{}, jugador_ref{pref} {}
void Escena_Camara::onInit() {
  // la asiignamos antes, porque podemos entrar y salir de escenas
  // así la camara regresa a la que debería en la escena
  // ID = 1 = LERP es en el orden que se agreguen
  CE::GestorCamaras::Get().setCamaraActiva(3);
  // le decimos a quien persigue
  CE::GestorCamaras::Get().getCamaraActiva().lockEnObjeto(jugador_ref);
  if (!inicializar)
    return;
  registrarBotones(sf::Keyboard::Scancode::W, "arriba");
  registrarBotones(sf::Keyboard::Scancode::Up, "arriba");
  registrarBotones(sf::Keyboard::Scancode::S, "abajo");
  registrarBotones(sf::Keyboard::Scancode::Down, "abajo");
  registrarBotones(sf::Keyboard::Scancode::A, "izquierda");
  registrarBotones(sf::Keyboard::Scancode::Left, "izquierda");
  registrarBotones(sf::Keyboard::Scancode::D, "derecha");
  registrarBotones(sf::Keyboard::Scancode::Right, "derecha");
  registrarBotones(sf::Keyboard::Scancode::Enter, "aceptar");

  auto trans = jugador_ref->getTransformada();
  trans->velocidad = CE::Vector2D{500.f, 500.f};
  jugador_ref->setPosicion(300.f, 300.f);
  auto triangulo = std::make_shared<ITriangulo>(40.f, 0.f);
  triangulo->tri_shape.setFillColor(sf::Color(255, 191, 0));
  triangulo->tri_shape.setOutlineColor(sf::Color::Black);
  triangulo->tri_shape.setOutlineThickness(3.f);
  jugador_ref->addComponente(triangulo);
  jugador_ref->addComponente(std::make_shared<CE::IControl>());

  // objetos para que se muestre el movimiento
  int montes_count = 100;
  float dstd = 20.f;
  for (int i = 0; i < montes_count; i++) {
    // gauss (talves se tiene que agregar cmath)
    double por =
        std::exp(-0.5 * (((i - montes_count / 2.f) * (i - montes_count / 2.f)) /
                         (dstd * dstd)));
    int inc = 200;
    auto monte = std::make_shared<Rectangulo>(
        200.f, 200.f + (inc * por), sf::Color{184, 134, 11}, sf::Color::Black);
    monte->setPosicion(100 + (i * 200), 100 - (inc * por / 2.f));
    objetos.agregarPool(monte);
  }
  inicializar = false;
}

void Escena_Camara::onFinal() {
  // reseteamos la camara a la estática al salir/cambiar de escena
  CE::GestorCamaras::Get().setCamaraActiva(0);
}
void Escena_Camara::onUpdate(float dt) {
  jugador_ref->onUpdate(dt);
  SistemaMover(jugador_ref, dt);
  for (auto &obj : objetos.getPool()) {
    obj->onUpdate(dt);
  }
}
void Escena_Camara::onInputs(const CE::Botones &accion) {
  switch (accion.getTipo()) {
  case CE::Botones::TipoAccion::OnPress: {
    if (accion.getNombre() == "arriba") {
      jugador_ref->getComponente<CE::IControl>()->arr = true;
    }
    if (accion.getNombre() == "abajo") {
      jugador_ref->getComponente<CE::IControl>()->abj = true;
    }
    if (accion.getNombre() == "derecha") {
      jugador_ref->getComponente<CE::IControl>()->der = true;
    }
    if (accion.getNombre() == "izquierda") {
      jugador_ref->getComponente<CE::IControl>()->izq = true;
    }
    break;
  }
  case CE::Botones::TipoAccion::OnRelease: {
    if (accion.getNombre() == "arriba") {
      jugador_ref->getComponente<CE::IControl>()->arr = false;
    }
    if (accion.getNombre() == "abajo") {
      jugador_ref->getComponente<CE::IControl>()->abj = false;
    }
    if (accion.getNombre() == "derecha") {
      jugador_ref->getComponente<CE::IControl>()->der = false;
    }
    if (accion.getNombre() == "izquierda") {
      jugador_ref->getComponente<CE::IControl>()->izq = false;
    }
    break;
  }
  case CE::Botones::TipoAccion::None: {
    break;
  }
  }
}

void Escena_Camara::onRender() {
  for (auto &obj : objetos.getPool())
    CE::Render::Get().AddToDraw(*obj);
  CE::Render::Get().AddToDraw(*jugador_ref);

#if DEBUG
  auto cam = &CE::GestorCamaras::Get().getCamaraActiva();
  // Casteamos a nuestra nueva cámara
  auto csm = (CE::CamaraSnapMario *)cam;
  auto csvpos = csm->getTransformada().posicion;
  sf::RectangleShape debugcam{{csm->m_vdim.x, csm->m_vdim.y}};
  debugcam.setOrigin({csm->m_vdim.x / 2.f, csm->m_vdim.y / 2.f});
  debugcam.setPosition({csvpos.x, csvpos.y});
  debugcam.setOutlineThickness(3.f);
  debugcam.setOutlineColor(sf::Color::Yellow);
  debugcam.setFillColor(sf::Color::Transparent);
  CE::Render::Get().AddToDraw(debugcam);
#endif
}
} // namespace IVJ
