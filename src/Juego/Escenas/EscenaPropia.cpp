#include "Juego/Escenas/EscenaPropia.hpp"
#include "Motor/Utils/Vector2D.hpp"
#include <Juego/Componentes/IJComponentes.hpp>
#include <Juego/Figuras/Figuras.hpp>
#include <Juego/Maquinas/Soldier/IdleSoldier.hpp>
#include <Juego/Maquinas/Soldier/SoldierDown.hpp>
#include <Juego/Sistemas/Sistemas.hpp>
#include <Juego/objetos/Entidad.hpp>
#include <Motor/Camaras/CamarasGestor.hpp>
#include <Motor/Componentes/IComponentes.hpp>
#include <Motor/Inputs/Botones.hpp>
#include <Motor/Primitivos/GestorAssets.hpp>
#include <Motor/Primitivos/GestorEscenas.hpp>
#include <Motor/Render/Render.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cmath>
#include <cstdlib>
#include <memory>
namespace IVJ {

Escena_Propia::Escena_Propia(std::shared_ptr<Entidad> &pref)
    : CE::Escena{}, jugador_soldier{pref} {
  nombre = "Propia";
}
void Escena_Propia::onInit() {
  en_pausa = false;
  opcion_pausa = 0;
  proyectiles.clear();
  tiempo_ultimo_disparo = 0.2f;

  if (inicializar) {
    jugador_soldier = std::make_shared<Entidad>();

    registrarBotones(sf::Keyboard::Scancode::W, "arriba");
    registrarBotones(sf::Keyboard::Scancode::Up, "arriba");
    registrarBotones(sf::Keyboard::Scancode::S, "abajo");
    registrarBotones(sf::Keyboard::Scancode::Down, "abajo");
    registrarBotones(sf::Keyboard::Scancode::A, "izquierda");
    registrarBotones(sf::Keyboard::Scancode::Left, "izquierda");
    registrarBotones(sf::Keyboard::Scancode::D, "derecha");
    registrarBotones(sf::Keyboard::Scancode::Right, "derecha");
    registrarBotones(sf::Keyboard::Scancode::Enter, "aceptar");
    registrarBotones(sf::Keyboard::Scancode::Space, "aceptar");
    registrarBotones(sf::Keyboard::Scancode::Escape, "pausa");
    registrarBotones(sf::Mouse::Button::Right, "accion_secundaria");
    registrarBotones(sf::Mouse::Button::Left, "disparar");

    // Cargar mapa con colisiones en los bordes
    tiles_industrial.push_back(TileMap());

    if (!tiles_industrial[0].loadTileMap(ASSETS "/mapas/lab7/plataforma1_col.txt", objetos))
      exit(EXIT_FAILURE);

    // Cargar el sprite
    CE::GestorAssets::Get().agregarTextura(
        "soldier_down_sheet_K",                           // llave
        ASSETS "/sprites/jugador/soldier_down_sheet.png", // path del sprite
        CE::Vector2D{0.f, 0.f},                           // pos dentro de la hoja
        CE::Vector2D{256.f, 84.f});                       // dimensiones
    CE::GestorAssets::Get().agregarTextura(
        "soldier_up_sheet_K",                           // llave
        ASSETS "/sprites/jugador/soldier_up_sheet.png", // path del sprite
        CE::Vector2D{0.f, 0.f},                         // pos dentro de la hoja
        CE::Vector2D{256.f, 84.f});                     // dimensiones
    CE::GestorAssets::Get().agregarTextura(
        "soldier_right_sheet_K",                           // llave
        ASSETS "/sprites/jugador/soldier_right_sheet.png", // path del sprite
        CE::Vector2D{0.f, 0.f},     // pos dentro de la hoja
        CE::Vector2D{256.f, 84.f}); // dimensiones
    CE::GestorAssets::Get().agregarTextura(
        "soldier_left_sheet_K",                           // llave
        ASSETS "/sprites/jugador/soldier_left_sheet.png", // path del sprite
        CE::Vector2D{0.f, 0.f},                           // pos dentro de la hoja
        CE::Vector2D{256.f, 84.f});                       // dimensiones
    CE::GestorAssets::Get().agregarTextura(
        "soldier_right_spin_sheet_K",                           // llave
        ASSETS "/sprites/jugador/soldier_right_spin_sheet.png", // path del sprite
        CE::Vector2D{0.f, 0.f},     // pos dentro de la hoja
        CE::Vector2D{384.f, 84.f}); // dimensiones
    CE::GestorAssets::Get().agregarTextura(
        "soldier_left_spin_sheet_K",                           // llave
        ASSETS "/sprites/jugador/soldier_left_spin_sheet.png", // path del sprite
        CE::Vector2D{0.f, 0.f},     // pos dentro de la hoja
        CE::Vector2D{384.f, 84.f}); // dimensiones
    CE::GestorAssets::Get().agregarTextura(
        "soldier_down_spin_sheet_K",                           // llave
        ASSETS "/sprites/jugador/soldier_down_spin_sheet.png", // path del sprite
        CE::Vector2D{0.f, 0.f},     // pos dentro de la hoja
        CE::Vector2D{384.f, 84.f}); // dimensiones
    CE::GestorAssets::Get().agregarTextura(
        "soldier_up_spin_sheet_K",                           // llave
        ASSETS "/sprites/jugador/soldier_up_spin_sheet.png", // path del sprite
        CE::Vector2D{0.f, 0.f},     // pos dentro de la hoja
        CE::Vector2D{384.f, 84.f}); // dimensiones

    CE::GestorAssets::Get().agregarTextura(
        "proyectil_azul_neon_K",
        ASSETS "/sprites/proyectil_azul_neon.png",
        CE::Vector2D{0.f, 0.f},
        CE::Vector2D{64.f, 32.f});

    CE::GestorAssets::Get().agregarTextura(
        "proyectil_pixel_neon_K",
        ASSETS "/sprites/proyectil_pixel_neon.png",
        CE::Vector2D{0.f, 0.f},
        CE::Vector2D{32.f, 16.f});

    CE::GestorAssets::Get().agregarTextura(
        "proyectil_neon_ai_K",
        ASSETS "/sprites/proyectil_neon_ai.png",
        CE::Vector2D{0.f, 0.f},
        CE::Vector2D{64.f, 24.f});

    auto trans = jugador_soldier->getTransformada();
    trans->velocidad = CE::Vector2D{350.f, 350.f};
    jugador_soldier->setPosicion(300.f, 300.f);

    auto sprite = std::make_shared<CE::ISprite>(
        CE::GestorAssets::Get().getTextura("soldier_down_sheet_K"), // textura
        64, 84,                                                     // dim
        1.f);                                                       // escala

    jugador_soldier->addComponente(sprite);

    jugador_soldier->addComponente(std::make_shared<CE::IControl>());
    jugador_soldier->addComponente(std::make_shared<CE::IBoundingBox>(
        CE::Vector2D{64.f, 84.f}
    ));

    auto me = std::make_shared<IMaquinaEstado>();
    me->fsm = std::make_shared<IdleSoldier>();
    jugador_soldier->addComponente(me);
    jugador_soldier->setFSM(me->fsm);

    inicializar = false;
  } else {
    // Si ya estaba inicializado, restablecer posición y estado del jugador al entrar de nuevo
    jugador_soldier->setPosicion(300.f, 300.f);
    auto trans = jugador_soldier->getTransformada();
    if (trans) {
      trans->velocidad = CE::Vector2D{350.f, 350.f};
      trans->pos_prev = trans->posicion;
    }
    auto sprite = jugador_soldier->getComponente<CE::ISprite>();
    if (sprite) {
      sprite->m_sprite.setTexture(CE::GestorAssets::Get().getTextura("soldier_down_sheet_K"));
      sprite->m_sprite.setTextureRect(sf::IntRect{{0, 0}, {sprite->width, sprite->height}});
    }
    auto me = jugador_soldier->getComponente<IMaquinaEstado>();
    if (me) {
      me->fsm = std::make_shared<IdleSoldier>();
      jugador_soldier->setFSM(me->fsm);
    }
    limpiarControles();
  }

  // Buscar o registrar CamaraFija con lock al centro
  int id_camara = -1;
  auto &camaras = CE::GestorCamaras::Get().getListaCamaras();
  for (int i = 0; i < (int)camaras.size(); ++i) {
    if (camaras[i]->nombre == "CamaraFija" || camaras[i]->nombre == "CamafaFija") {
      id_camara = i;
      break;
    }
  }
  if (id_camara == -1) {
    auto cam_fija = std::make_shared<CE::CamaraFija>(
        CE::Vector2D{0.f, 0.f}, CE::Vector2D{1024, 720});
    CE::GestorCamaras::Get().agregarCamara(cam_fija);
    id_camara = (int)camaras.size() - 1;
  }

  CE::GestorCamaras::Get().setCamaraActiva(id_camara);
  auto &cam = CE::GestorCamaras::Get().getCamaraActiva();
  cam.lockEnObjeto(jugador_soldier);
  cam.getTransformada().posicion = jugador_soldier->getTransformada()->posicion;
  cam.onUpdate(0.f);
}
void Escena_Propia::onFinal() {
  en_pausa = false;
  limpiarControles();
  proyectiles.clear();
  // reseteamos la camara a la estática al salir/cambiar de escena
  CE::GestorCamaras::Get().setCamaraActiva(0);
}
void Escena_Propia::onUpdate(float dt) {
  if (en_pausa) {
    // Si el juego está en pausa, no actualizamos lógica de juego ni físicas
    return;
  }

  // Manejo de disparo continuo/sostenido
  tiempo_ultimo_disparo += dt;
  auto ctrl = jugador_soldier ? jugador_soldier->getComponente<CE::IControl>() : nullptr;
  if (ctrl && ctrl->acc && tiempo_ultimo_disparo >= cadencia_disparo) {
    dispararBala();
  }

  jugador_soldier->inputFSM();
  jugador_soldier->onUpdate(dt);
  SistemaMover(jugador_soldier, dt);
  for (auto &obj : objetos.getPool()) {
    obj->onUpdate(dt);
    SistemaColAABBMid(*jugador_soldier, *obj, true);
  }

  // Actualizar proyectiles
  for (auto it = proyectiles.begin(); it != proyectiles.end();) {
    it->tiempo_transcurrido += dt;
    if (it->tiempo_transcurrido >= it->tiempo_vida) {
      it = proyectiles.erase(it);
      continue;
    }

    auto trans = it->entidad->getTransformada();
    trans->pos_prev = trans->posicion;
    trans->posicion.x += it->direccion.x * it->velocidad * dt;
    trans->posicion.y += it->direccion.y * it->velocidad * dt;
    it->entidad->onUpdate(dt);

    // Colisión con paredes del mapa después de unos milisegundos de gracia
    bool colision = false;
    if (it->tiempo_transcurrido > 0.05f) {
      float bx = trans->posicion.x;
      float by = trans->posicion.y;
      for (auto &obj : objetos.getPool()) {
        if (!obj->tieneComponente<CE::IBoundingBox>())
          continue;
        auto bound = obj->getComponente<CE::IBoundingBox>();
        auto objPos = obj->getTransformada()->posicion;
        float minX = objPos.x - bound->mitad.x - 6.f;
        float maxX = objPos.x - bound->mitad.x + bound->tam.x + 6.f;
        float minY = objPos.y - bound->mitad.y - 6.f;
        float maxY = objPos.y - bound->mitad.y + bound->tam.y + 6.f;

        if (bx >= minX && bx <= maxX && by >= minY && by <= maxY) {
          colision = true;
          break;
        }
      }
    }

    if (colision) {
      it = proyectiles.erase(it);
    } else {
      ++it;
    }
  }
}
void Escena_Propia::onInputs(const CE::Botones &accion) {
  // Manejo de inputs durante la pausa
  if (en_pausa) {
    if (accion.getTipo() == CE::Botones::TipoAccion::OnPress) {
      if (accion.getNombre() == "pausa") {
        en_pausa = false;
        resincronizarControles();
      } else if (accion.getNombre() == "arriba") {
        opcion_pausa = (opcion_pausa == 0) ? 2 : (opcion_pausa - 1);
      } else if (accion.getNombre() == "abajo") {
        opcion_pausa = (opcion_pausa == 2) ? 0 : (opcion_pausa + 1);
      } else if (accion.getNombre() == "aceptar") {
        if (opcion_pausa == 0) {
          // Reanudar el juego
          en_pausa = false;
          resincronizarControles();
        } else if (opcion_pausa == 1) {
          // Regresar al menu principal
          en_pausa = false;
          limpiarControles();
          CE::GestorEscenas::Get().cambiarEscena("Escena_Menu_Principal");
        } else if (opcion_pausa == 2) {
          // Regresar al escritorio
          std::exit(EXIT_SUCCESS);
        }
      }
    }
    return;
  }

  // Activar pausa al presionar Escape
  if (accion.getTipo() == CE::Botones::TipoAccion::OnPress &&
      accion.getNombre() == "pausa") {
    en_pausa = true;
    opcion_pausa = 0;
    limpiarControles();
    return;
  }

  // Controles normales del jugador con verificación defensiva
  auto ctrl = jugador_soldier ? jugador_soldier->getComponente<CE::IControl>() : nullptr;
  if (!ctrl)
    return;

  switch (accion.getTipo()) {
  case CE::Botones::TipoAccion::OnPress: {
    if (accion.getNombre() == "arriba") {
      ctrl->arr = true;
    }
    if (accion.getNombre() == "abajo") {
      ctrl->abj = true;
    }
    if (accion.getNombre() == "derecha") {
      ctrl->der = true;
    }
    if (accion.getNombre() == "izquierda") {
      ctrl->izq = true;
    }
    if (accion.getNombre() == "accion_secundaria") {
      ctrl->sacc = true;
    }
    if (accion.getNombre() == "disparar") {
      ctrl->acc = true;
      if (tiempo_ultimo_disparo >= 0.12f) {
        dispararBala();
      }
    }
    break;
  }
  case CE::Botones::TipoAccion::OnRelease: {
    if (accion.getNombre() == "arriba") {
      ctrl->arr = false;
    }
    if (accion.getNombre() == "abajo") {
      ctrl->abj = false;
    }
    if (accion.getNombre() == "derecha") {
      ctrl->der = false;
    }
    if (accion.getNombre() == "izquierda") {
      ctrl->izq = false;
    }
    if (accion.getNombre() == "accion_secundaria") {
      ctrl->sacc = false;
    }
    if (accion.getNombre() == "disparar") {
      ctrl->acc = false;
    }
    break;
  }
  case CE::Botones::TipoAccion::None: {
    break;
  }
  }
}

void Escena_Propia::limpiarControles() {
  if (!jugador_soldier)
    return;
  auto ctrl = jugador_soldier->getComponente<CE::IControl>();
  if (ctrl) {
    ctrl->arr = false;
    ctrl->abj = false;
    ctrl->izq = false;
    ctrl->der = false;
    ctrl->sacc = false;
    ctrl->acc = false;
  }
}

void Escena_Propia::resincronizarControles() {
  if (!jugador_soldier)
    return;
  auto ctrl = jugador_soldier->getComponente<CE::IControl>();
  if (ctrl) {
    ctrl->arr = sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::W) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Up);
    ctrl->abj = sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::S) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Down);
    ctrl->izq = sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left);
    ctrl->der = sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right);
    ctrl->sacc = sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);
    ctrl->acc = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
  }
}

void Escena_Propia::onRender() {
  for (auto &al : tiles_industrial)
    CE::Render::Get().AddToDraw(al);
  for (auto &obj : objetos.getPool())
    CE::Render::Get().AddToDraw(*obj);
  for (auto &bala : proyectiles)
    CE::Render::Get().AddToDraw(*bala.entidad);
  CE::Render::Get().AddToDraw(*jugador_soldier);

#if DEBUG
  auto cam = &CE::GestorCamaras::Get().getCamaraActiva();
  if (auto csm = dynamic_cast<CE::CamaraSnapMario *>(cam)) {
    auto csvpos = csm->getTransformada().posicion;
    sf::RectangleShape debugcam{{csm->m_vdim.x, csm->m_vdim.y}};
    debugcam.setOrigin({csm->m_vdim.x / 2.f, csm->m_vdim.y / 2.f});
    debugcam.setPosition({csvpos.x, csvpos.y});
    debugcam.setOutlineThickness(3.f);
    debugcam.setOutlineColor(sf::Color::Yellow);
    debugcam.setFillColor(sf::Color::Transparent);
    CE::Render::Get().AddToDraw(debugcam);
  }
#endif

  // Renderizar menú de pausa cuando el juego esté pausado
  if (en_pausa) {
    auto &tex = CE::Render::Get().GetTextura();
    sf::View prevView = tex.getView();
    tex.setView(tex.getDefaultView());

    float screenW = (float)tex.getSize().x;
    float screenH = (float)tex.getSize().y;

    // 1. Overlay oscuro semi-transparente que cubre toda la pantalla
    sf::RectangleShape overlay{{screenW, screenH}};
    overlay.setFillColor(sf::Color(10, 10, 18, 215));
    tex.draw(overlay);

    // 2. Panel central
    float panelW = 620.f;
    float panelH = 460.f;
    float panelX = (screenW - panelW) / 2.f;
    float panelY = (screenH - panelH) / 2.f;

    sf::RectangleShape panel{{panelW, panelH}};
    panel.setPosition({panelX, panelY});
    panel.setFillColor(sf::Color(22, 24, 34, 245));
    panel.setOutlineThickness(3.f);
    panel.setOutlineColor(sf::Color(85, 95, 135, 230));
    tex.draw(panel);

    const auto &font = CE::GestorAssets::Get().getFont("default_font");

    // 3. Título del juego "Expendable" y subtítulo "PAUSA"
    sf::Text txtTitulo{font, "Expendable", 46};
    txtTitulo.setFillColor(sf::Color(255, 255, 255, 250));
    txtTitulo.setStyle(sf::Text::Style::Bold);
    float tW = txtTitulo.getLocalBounds().size.x;
    txtTitulo.setPosition({(screenW - tW) / 2.f, panelY + 25.f});
    tex.draw(txtTitulo);

    sf::Text txtSubtitulo{font, "- PAUSA -", 22};
    txtSubtitulo.setFillColor(sf::Color(160, 175, 215, 220));
    txtSubtitulo.setStyle(sf::Text::Style::Bold);
    float subW = txtSubtitulo.getLocalBounds().size.x;
    txtSubtitulo.setPosition({(screenW - subW) / 2.f, panelY + 80.f});
    tex.draw(txtSubtitulo);

    // Línea separadora
    sf::RectangleShape linea{{panelW - 80.f, 2.f}};
    linea.setPosition({panelX + 40.f, panelY + 115.f});
    linea.setFillColor(sf::Color(75, 85, 120, 170));
    tex.draw(linea);

    // 4. Opciones del menú
    // Opción 0: Reanudar (tamaño considerablemente más grande: 52px)
    sf::Text txtReanudar{font, "Reanudar", 52};
    txtReanudar.setStyle(sf::Text::Style::Bold);
    float rW = txtReanudar.getLocalBounds().size.x;
    float rX = (screenW - rW) / 2.f;
    float rY = panelY + 140.f;
    txtReanudar.setPosition({rX, rY});

    // Opción 1: Regresar al menu principal (tamaño menor: 32px)
    sf::Text txtMenuPrincipal{font, "Regresar al menu principal", 32};
    txtMenuPrincipal.setStyle(sf::Text::Style::Bold);
    float mpW = txtMenuPrincipal.getLocalBounds().size.x;
    float mpX = (screenW - mpW) / 2.f;
    float mpY = panelY + 235.f;
    txtMenuPrincipal.setPosition({mpX, mpY});

    // Opción 2: Regresar al escritorio (tamaño menor: 32px)
    sf::Text txtEscritorio{font, "Regresar al escritorio", 32};
    txtEscritorio.setStyle(sf::Text::Style::Bold);
    float eW = txtEscritorio.getLocalBounds().size.x;
    float eX = (screenW - eW) / 2.f;
    float eY = panelY + 310.f;
    txtEscritorio.setPosition({eX, eY});

    // Colores según selección activa
    if (opcion_pausa == 0) {
      txtReanudar.setFillColor(sf::Color(255, 255, 255, 255));
      txtMenuPrincipal.setFillColor(sf::Color(140, 145, 160, 180));
      txtEscritorio.setFillColor(sf::Color(140, 145, 160, 180));
    } else if (opcion_pausa == 1) {
      txtReanudar.setFillColor(sf::Color(170, 175, 190, 200));
      txtMenuPrincipal.setFillColor(sf::Color(255, 255, 255, 255));
      txtEscritorio.setFillColor(sf::Color(140, 145, 160, 180));
    } else {
      txtReanudar.setFillColor(sf::Color(170, 175, 190, 200));
      txtMenuPrincipal.setFillColor(sf::Color(140, 145, 160, 180));
      txtEscritorio.setFillColor(sf::Color(255, 255, 255, 255));
    }

    // Indicador / Cursor selector
    sf::Text txtCursor{font, ">", (opcion_pausa == 0) ? 46u : 32u};
    txtCursor.setFillColor(sf::Color(255, 215, 60, 255));
    txtCursor.setStyle(sf::Text::Style::Bold);

    if (opcion_pausa == 0) {
      txtCursor.setPosition({rX - 40.f, rY + 2.f});
    } else if (opcion_pausa == 1) {
      txtCursor.setPosition({mpX - 35.f, mpY});
    } else {
      txtCursor.setPosition({eX - 35.f, eY});
    }

    tex.draw(txtReanudar);
    tex.draw(txtMenuPrincipal);
    tex.draw(txtEscritorio);
    tex.draw(txtCursor);

    // Instrucciones al pie del panel
    sf::Text txtAyuda{font, "[ W / S ] Seleccionar     [ Enter / Espacio ] Confirmar     [ Esc ] Reanudar", 18};
    txtAyuda.setFillColor(sf::Color(135, 145, 170, 200));
    float aW = txtAyuda.getLocalBounds().size.x;
    txtAyuda.setPosition({(screenW - aW) / 2.f, panelY + panelH - 45.f});
    tex.draw(txtAyuda);

    tex.setView(prevView);
  }
}

void Escena_Propia::dispararBala() {
  if (en_pausa || !jugador_soldier)
    return;

  if (!CE::Render::Get().isMouseInsideViewport())
    return;

  auto trans_jugador = jugador_soldier->getTransformada();
  if (!trans_jugador)
    return;

  CE::Vector2D playerPos = trans_jugador->posicion;
  sf::Vector2f mouseWorld = CE::Render::Get().getMouseWorldPos();

  CE::Vector2D dir{mouseWorld.x - playerPos.x, mouseWorld.y - playerPos.y};
  float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
  if (len < 0.001f) {
    dir = CE::Vector2D{0.f, 1.f};
  } else {
    dir.x /= len;
    dir.y /= len;
  }

  float angulo = std::atan2(dir.y, dir.x);

  // Si el soldado está en reposo (IdleSoldier), orientar su sprite hacia la dirección de disparo
  auto me = jugador_soldier->getComponente<IMaquinaEstado>();
  if (me && me->fsm && me->fsm->getNombre() == "IdleSoldier") {
    auto sprite = jugador_soldier->getComponente<CE::ISprite>();
    if (sprite) {
      if (std::abs(dir.x) > std::abs(dir.y)) {
        if (dir.x > 0) {
          sprite->m_sprite.setTexture(
              CE::GestorAssets::Get().getTextura("soldier_right_sheet_K"));
        } else {
          sprite->m_sprite.setTexture(
              CE::GestorAssets::Get().getTextura("soldier_left_sheet_K"));
        }
      } else {
        if (dir.y > 0) {
          sprite->m_sprite.setTexture(
              CE::GestorAssets::Get().getTextura("soldier_down_sheet_K"));
        } else {
          sprite->m_sprite.setTexture(
              CE::GestorAssets::Get().getTextura("soldier_up_sheet_K"));
        }
      }
      sprite->m_sprite.setTextureRect(
          sf::IntRect{{0, 0}, {sprite->width, sprite->height}});
    }
  }

  // Posición inicial del proyectil saliendo del soldado
  CE::Vector2D spawnPos{playerPos.x + dir.x * 25.f, playerPos.y + dir.y * 25.f};

  auto bala_entidad = std::make_shared<Entidad>();
  bala_entidad->setPosicion(spawnPos.x, spawnPos.y);
  auto trans_bala = bala_entidad->getTransformada();
  trans_bala->posicion = spawnPos;
  trans_bala->pos_prev = spawnPos;
  trans_bala->angulo = angulo;
  trans_bala->velocidad = CE::Vector2D{dir.x * 850.f, dir.y * 850.f};

  auto sprite_bala = std::make_shared<CE::ISprite>(
      CE::GestorAssets::Get().getTextura("proyectil_azul_neon_K"),
      64, 32,
      0.65f);
  bala_entidad->addComponente(sprite_bala);
  bala_entidad->onUpdate(0.f);

  ProyectilBala p;
  p.entidad = bala_entidad;
  p.direccion = dir;
  p.velocidad = 850.f;
  p.tiempo_vida = 2.5f;
  p.tiempo_transcurrido = 0.f;

  proyectiles.push_back(p);
  tiempo_ultimo_disparo = 0.f;
}

} // namespace IVJ
