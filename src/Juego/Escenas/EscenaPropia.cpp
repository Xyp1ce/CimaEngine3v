#include "Juego/Escenas/EscenaPropia.hpp"
#include "Motor/Utils/Vector2D.hpp"
#include <Juego/Componentes/IJComponentes.hpp>
#include <Juego/Figuras/Figuras.hpp>
#include <Juego/Sistemas/Sistemas.hpp>
#include <Juego/objetos/Entidad.hpp>
#include <Motor/Camaras/CamarasGestor.hpp>
#include <Motor/Componentes/IComponentes.hpp>
#include <Motor/Inputs/Botones.hpp>
#include <Motor/Primitivos/GestorAssets.hpp>
#include <Motor/Render/Render.hpp>
#include <cstdlib>
#include <memory>
namespace IVJ {

Escena_Propia::Escena_Propia(std::shared_ptr<Entidad> &pref)
    : CE::Escena{}, jugador_soldier{pref} {}
void Escena_Propia::onInit() {
  jugador_soldier = std::make_shared<Entidad>();
  CE::GestorCamaras::Get().setCamaraActiva(2);
  CE::GestorCamaras::Get().getCamaraActiva().lockEnObjeto(jugador_soldier);
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

  // Cargar mapa 3 layers
  tiles_industrial.push_back(TileMap());

  if (!tiles_industrial[0].loadTileMap(ASSETS "/mapas/lab7/plataforma1.txt"))
    exit(EXIT_FAILURE);

  // Cargar el sprite
  CE::GestorAssets::Get().agregarTextura(
      "soldierP",                             // llave
      ASSETS "/sprites/jugador/soldier1.png", // path del sprite
      CE::Vector2D{0.f, 0.f},                 // pos dentro de la hoja
      CE::Vector2D{64.f, 85.f});              // dimensiones

  auto trans = jugador_soldier->getTransformada();
  trans->velocidad = CE::Vector2D{500.f, 500.f};
  jugador_soldier->setPosicion(300.f, 300.f);

  auto sprite = std::make_shared<CE::ISprite>(
      CE::GestorAssets::Get().getTextura("soldierP"), // textura
      64, 85,                                         // dim
      1.f);                                           // escala

  jugador_soldier->addComponente(sprite);
  jugador_soldier->addComponente(std::make_shared<CE::IControl>());

  inicializar = false;
}
void Escena_Propia::onFinal() {
  // reseteamos la camara a la estática al salir/cambiar de escena
  CE::GestorCamaras::Get().setCamaraActiva(2);
}
void Escena_Propia::onUpdate(float dt) {
  jugador_soldier->onUpdate(dt);
  SistemaMover(jugador_soldier, dt);
  for (auto &obj : objetos.getPool()) {
    obj->onUpdate(dt);
  }
}
void Escena_Propia::onInputs(const CE::Botones &accion) {
  switch (accion.getTipo()) {
  case CE::Botones::TipoAccion::OnPress: {
    if (accion.getNombre() == "arriba") {
      jugador_soldier->getComponente<CE::IControl>()->arr = true;
    }
    if (accion.getNombre() == "abajo") {
      jugador_soldier->getComponente<CE::IControl>()->abj = true;
    }
    if (accion.getNombre() == "derecha") {
      jugador_soldier->getComponente<CE::IControl>()->der = true;
    }
    if (accion.getNombre() == "izquierda") {
      jugador_soldier->getComponente<CE::IControl>()->izq = true;
    }
    break;
  }
  case CE::Botones::TipoAccion::OnRelease: {
    if (accion.getNombre() == "arriba") {
      jugador_soldier->getComponente<CE::IControl>()->arr = false;
    }
    if (accion.getNombre() == "abajo") {
      jugador_soldier->getComponente<CE::IControl>()->abj = false;
    }
    if (accion.getNombre() == "derecha") {
      jugador_soldier->getComponente<CE::IControl>()->der = false;
    }
    if (accion.getNombre() == "izquierda") {
      jugador_soldier->getComponente<CE::IControl>()->izq = false;
    }
    break;
  }
  case CE::Botones::TipoAccion::None: {
    break;
  }
  }
}

void Escena_Propia::onRender() {

  for (auto &al : tiles_industrial)
    CE::Render::Get().AddToDraw(al);
  for (auto &obj : objetos.getPool())
    CE::Render::Get().AddToDraw(*obj);
  CE::Render::Get().AddToDraw(*jugador_soldier);

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
