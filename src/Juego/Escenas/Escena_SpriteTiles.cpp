#include "Escena_SpriteTiles.hpp"
#include "Motor/Utils/Vector2D.hpp"
#include <Juego/Componentes/IJComponentes.hpp>
#include <Juego/Figuras/Figuras.hpp>
#include <Juego/Maquinas/Bosses/IdleBoss.hpp>
#include <Juego/Maquinas/Naves/IdleJugadores.hpp>
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

Escena_Sprite::Escena_Sprite(std::shared_ptr<Entidad> &pref)
    : CE::Escena{}, jugador_ref{pref} {}
void Escena_Sprite::onInit() {
  CE::GestorCamaras::Get().setCamaraActiva(2);
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

  // cargar mapa
  tiles_layers.push_back(TileMap());
  tiles_layers.push_back(TileMap());
  tiles_layers.push_back(TileMap());

  if (!tiles_layers[0].loadTileMap(ASSETS "/mapas/playa_layer1.txt"))
    exit(EXIT_FAILURE);
  if (!tiles_layers[1].loadTileMap(ASSETS "/mapas/playa_layer2.txt"))
    exit(EXIT_FAILURE);
  if (!tiles_layers[2].loadTileMap(ASSETS "/mapas/playa_layer3.txt"))
    exit(EXIT_FAILURE);

  // Lab 8 animacinoes
  CE::GestorAssets::Get().agregarTextura(
      "nave_sheet",                             // key
      ASSETS "/sprites/naves/player_sheet.png", // hoja
      CE::Vector2D{0.f, 0.f},                   // inicio
      CE::Vector2D{320.f, 64.f}                 // dim de la hoja
  );

  CE::GestorAssets::Get().agregarTextura(
      "boss_sheet", ASSETS "/sprites/bosses/VileCrustaceanIdleAttack.png",
      CE::Vector2D{0.f, 0.f}, CE::Vector2D{924.f, 260.f});

  auto trans = jugador_ref->getTransformada();
  trans->velocidad = CE::Vector2D{500.f, 500.f};
  jugador_ref->setPosicion(300.f, 300.f);

  // Lab 8 animaciones
  auto sprite = std::make_shared<CE::ISprite>(
      CE::GestorAssets::Get().getTextura("nave_sheet"), // textura
      64, 64,                                           // dim
      1.f);                                             // escala

  jugador_ref->addComponente(sprite);
  jugador_ref->addComponente(std::make_shared<CE::IControl>());

  // lab 8 animaciones
  auto me = std::make_shared<IMaquinaEstado>();
  me->fsm = std::make_shared<IdleJugadores>();
  jugador_ref->addComponente(me);
  // ejecuta onEntrar para inicializar variables
  jugador_ref->setFSM(me->fsm);

  // boss entidad
  auto boss = std::make_shared<Entidad>();
  auto boss_sprite = std::make_shared<CE::ISprite>(
      CE::GestorAssets::Get().getTextura("boss_sheet"), 154.f, 130.f, 1.f);
  auto boss_me = std::make_shared<IVJ::IMaquinaEstado>();
  // probar patch con nodo IdleBoss
  boss_me->fsm = std::make_shared<IVJ::IdleBoss>(6, 0.25f);
  auto boss_target = std::make_shared<ITarget>(nullptr);
  boss_target->setTarget(*jugador_ref);

  boss->addComponente(boss_sprite)
      .addComponente(boss_me)
      .addComponente(std::make_shared<IRangoAggro>(500.f))
      .addComponente(boss_target)
      .setPosicion(500.f, 500.f);
  boss->setFSM(boss_me->fsm);

  objetos.agregarPool(boss);

  inicializar = false;
}
void Escena_Sprite::onFinal() {
  // reseteamos la camara a la estática al salir/cambiar de escena
  CE::GestorCamaras::Get().setCamaraActiva(2);
}

void Escena_Sprite::onUpdate(float dt) {
  jugador_ref->inputFSM();
  jugador_ref->onUpdate(dt);
  SistemaMover(jugador_ref, dt);
  for (auto &obj : objetos.getPool()) {
    obj->inputFSM();
    obj->onUpdate(dt);
    if (obj->tieneComponente<ITarget>()) {
      auto &tpos = *obj->getComponente<ITarget>()->pos;
      SistemaNPCLookTarget(*obj, tpos);
    }
  }
}

void Escena_Sprite::onInputs(const CE::Botones &accion) {
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

void Escena_Sprite::onRender() {
  for (auto &al : tiles_layers)
    CE::Render::Get().AddToDraw(al);

  for (auto &obj : objetos.getPool()) {
    // Lab 8 animaciones
    if (obj->tieneComponente<IRangoAggro>()) {
      auto rango = obj->getComponente<IRangoAggro>();
      auto pos = obj->getTransformada()->posicion;
      rango->marcador.setPosition({pos.x, pos.y});
      CE::Render::Get().AddToDraw(rango->marcador);
    }

    CE::Render::Get().AddToDraw(*obj);
  }
  CE::Render::Get().AddToDraw(*jugador_ref);

#if DEBUG
  auto cam = &CE::GestorCamaras::Get().getCamaraActiva();
  auto csv = (CE::CamaraSnapVentana *)cam;
  auto csvpos = csv->getTransformada().posicion;
  sf::RectangleShape debugcam{{csv->m_vdim.x, csv->m_vdim.y}};
  debugcam.setOrigin({csv->m_vdim.x / 2.f, csv->m_vdim.y / 2.f});
  debugcam.setPosition({csvpos.x, csvpos.y});
  debugcam.setOutlineThickness(3.f);
  debugcam.setOutlineColor(sf::Color::Yellow);
  debugcam.setFillColor(sf::Color::Transparent);
  CE::Render::Get().AddToDraw(debugcam);
#endif
}
} // namespace IVJ
