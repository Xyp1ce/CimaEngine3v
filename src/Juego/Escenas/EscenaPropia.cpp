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
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <algorithm>
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
  orbes.clear();
  danio_continuo_jefe = 0;
  impactos_consecutivos_jefe = 0;
  fallos_rng_orbe = 0;
  tiempo_ultimo_disparo = cadencia_disparo;
  balas_cargador = balas_cargador_max;
  balas_reserva = balas_reserva_max;
  recargando = false;
  tiempo_recarga_actual = 0.0f;

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
    registrarBotones(sf::Keyboard::Scancode::R, "recargar");

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

    CE::GestorAssets::Get().agregarTextura(
        "cursor_crosshair_K",
        ASSETS "/sprites/cursor_crosshair.png",
        CE::Vector2D{0.f, 0.f},
        CE::Vector2D{32.f, 32.f});

    CE::GestorAssets::Get().agregarTextura(
        "orbe_verde_K",
        ASSETS "/sprites/orbe_verde.png",
        CE::Vector2D{0.f, 0.f},
        CE::Vector2D{64.f, 64.f});

    CE::GestorAssets::Get().agregarTextura(
        "orbe_morado_K",
        ASSETS "/sprites/orbe_morado.png",
        CE::Vector2D{0.f, 0.f},
        CE::Vector2D{64.f, 64.f});

    CE::GestorAssets::Get().agregarTextura(
        "arma_soldado_K",
        ASSETS "/sprites/arma_soldado.png",
        CE::Vector2D{0.f, 0.f},
        CE::Vector2D{240.f, 96.f});

    sprite_arma_hud.emplace(CE::GestorAssets::Get().getTextura("arma_soldado_K"));
    sprite_arma_hud->setScale({0.6f, 0.6f});

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

    // Inicializar enemigo (cuadro rojo con 200 de vida, sin IA aún)
    enemigo_rojo.entidad = std::make_shared<Entidad>();
    enemigo_rojo.entidad->getNombre()->nombre = "Enemigo_Cuadro_Rojo";
    enemigo_rojo.entidad->setPosicion(600.f, 300.f);
    enemigo_rojo.entidad->getStats()->hp_max = enemigo_rojo.vida_max;
    enemigo_rojo.entidad->getStats()->hp = enemigo_rojo.vida_max;
    enemigo_rojo.entidad->addComponente(
        std::make_shared<CE::IBoundingBox>(CE::Vector2D{56.f, 56.f}));

    enemigo_rojo.vida = enemigo_rojo.vida_max;
    enemigo_rojo.vivo = true;
    enemigo_rojo.cuadro.setSize({56.f, 56.f});
    enemigo_rojo.cuadro.setOrigin({28.f, 28.f});
    enemigo_rojo.cuadro.setPosition({600.f, 300.f});
    enemigo_rojo.cuadro.setFillColor(sf::Color(220, 35, 35));
    enemigo_rojo.cuadro.setOutlineThickness(2.f);
    enemigo_rojo.cuadro.setOutlineColor(sf::Color(255, 95, 95));

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
    balas_cargador = balas_cargador_max;
    balas_reserva = balas_reserva_max;
    recargando = false;
    tiempo_recarga_actual = 0.0f;
    sprite_arma_hud.emplace(CE::GestorAssets::Get().getTextura("arma_soldado_K"));
    sprite_arma_hud->setScale({0.6f, 0.6f});

    // Restablecer estado y vida del enemigo al reentrar a la escena
    enemigo_rojo.vida = enemigo_rojo.vida_max;
    enemigo_rojo.vivo = true;
    if (enemigo_rojo.entidad) {
      enemigo_rojo.entidad->setPosicion(600.f, 300.f);
      enemigo_rojo.entidad->getStats()->hp = enemigo_rojo.vida_max;
    }
    enemigo_rojo.cuadro.setPosition({600.f, 300.f});

    limpiarControles();
  }

  // Buscar o registrar CamaraBulletHell con desplazamiento al cursor y Caja Roja
  int id_camara = -1;
  auto &camaras = CE::GestorCamaras::Get().getListaCamaras();
  for (int i = 0; i < (int)camaras.size(); ++i) {
    if (camaras[i]->nombre == "CamaraBulletHell") {
      id_camara = i;
      break;
    }
  }
  if (id_camara == -1) {
    auto cam_bh = std::make_shared<CE::CamaraBulletHell>(
        CE::Vector2D{0.f, 0.f}, CE::Vector2D{1024, 720},
        CE::Vector2D{180.f, 120.f}, 0.30f, 7.0f);
    CE::GestorCamaras::Get().agregarCamara(cam_bh);
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
  recargando = false;
  tiempo_recarga_actual = 0.0f;
  limpiarControles();
  proyectiles.clear();
  orbes.clear();
  danio_continuo_jefe = 0;
  impactos_consecutivos_jefe = 0;
  fallos_rng_orbe = 0;
  // reseteamos la camara a la estática al salir/cambiar de escena
  CE::GestorCamaras::Get().setCamaraActiva(0);
}
void Escena_Propia::onUpdate(float dt) {
  if (en_pausa) {
    // Si el juego está en pausa, no actualizamos lógica de juego ni físicas
    return;
  }

  // Actualizar progreso de recarga
  if (recargando) {
    tiempo_recarga_actual += dt;
    if (tiempo_recarga_actual >= tiempo_recarga_total) {
      tiempo_recarga_actual = tiempo_recarga_total;
      recargando = false;
      recargarArma();
    }
  }

  jugador_soldier->inputFSM();

  // Manejo de disparo continuo/sostenido o recarga al continuar presionando con cargador vacío
  tiempo_ultimo_disparo += dt;
  auto ctrl = jugador_soldier ? jugador_soldier->getComponente<CE::IControl>() : nullptr;
  if (ctrl && ctrl->acc && !estaRodando() && !recargando) {
    if (balas_cargador > 0) {
      if (tiempo_ultimo_disparo >= cadencia_disparo) {
        dispararBala();
      }
    } else if (tiempo_ultimo_disparo >= cadencia_disparo) {
      iniciarRecarga();
    }
  }

  jugador_soldier->onUpdate(dt);
  SistemaMover(jugador_soldier, dt);
  for (auto &obj : objetos.getPool()) {
    obj->onUpdate(dt);
    SistemaColAABBMid(*jugador_soldier, *obj, true);
  }
  if (enemigo_rojo.vivo && enemigo_rojo.entidad) {
    enemigo_rojo.entidad->onUpdate(dt);
    auto epos = enemigo_rojo.entidad->getTransformada()->posicion;
    enemigo_rojo.cuadro.setPosition({epos.x, epos.y});
    SistemaColAABBMid(*jugador_soldier, *enemigo_rojo.entidad, true);
  }

  // Actualizar orbes de mejora (expiración a los 15s, parpadeo progresivo y obtención por contacto)
  for (auto it = orbes.begin(); it != orbes.end();) {
    it->tiempo_transcurrido += dt;
    if (it->tiempo_transcurrido >= it->tiempo_vida_max) {
      it = orbes.erase(it);
      continue;
    }

    if (it->entidad) {
      it->entidad->onUpdate(dt);

      // Parpadeo progresivo hacia el final de su duración (últimos 5 segundos de los 15s)
      auto sprite_orbe = it->entidad->getComponente<CE::ISprite>();
      if (sprite_orbe) {
        float tiempo_restante = it->tiempo_vida_max - it->tiempo_transcurrido;
        if (tiempo_restante < 5.0f) {
          float urgencia = std::clamp((5.0f - tiempo_restante) / 5.0f, 0.0f, 1.0f);
          float frec = 8.0f + urgencia * 24.0f;
          float onda = 0.5f * (1.0f + std::sin(it->tiempo_transcurrido * frec));
          std::uint8_t alpha = static_cast<std::uint8_t>(45.f + onda * 210.f);
          sprite_orbe->m_sprite.setColor(sf::Color(255, 255, 255, alpha));
        } else {
          sprite_orbe->m_sprite.setColor(sf::Color::White);
        }
      }

      // Obtención por contacto directo (caminar o usar Dash/Spin sobre el orbe)
      if (jugador_soldier) {
        auto jpos = jugador_soldier->getTransformada()->posicion;
        auto opos = it->entidad->getTransformada()->posicion;
        float dx = jpos.x - opos.x;
        float dy = jpos.y - opos.y;
        if ((dx * dx + dy * dy) <= (42.f * 42.f)) {
          reclamarOrbe(*it);
          it = orbes.erase(it);
          continue;
        }
      }
    }

    ++it;
  }

  // Actualizar proyectiles
  for (auto it = proyectiles.begin(); it != proyectiles.end();) {
    it->tiempo_transcurrido += dt;
    if (it->tiempo_transcurrido >= it->tiempo_vida) {
      // Bala expiró sin impactar al jefe ni a un orbe: reinicia ráfaga consecutiva
      danio_continuo_jefe = 0;
      impactos_consecutivos_jefe = 0;
      it = proyectiles.erase(it);
      continue;
    }

    auto trans = it->entidad->getTransformada();
    trans->pos_prev = trans->posicion;
    trans->posicion.x += it->direccion.x * it->velocidad * dt;
    trans->posicion.y += it->direccion.y * it->velocidad * dt;
    it->entidad->onUpdate(dt);

    float bx = trans->posicion.x;
    float by = trans->posicion.y;
    bool colision = false;
    bool impacto_orbe = false;

    // 1. Verificar impacto de bala contra un Orbe (4 disparos para reclamar a distancia, independencia de fuego)
    for (auto it_orbe = orbes.begin(); it_orbe != orbes.end(); ++it_orbe) {
      if (!it_orbe->entidad)
        continue;
      auto opos = it_orbe->entidad->getTransformada()->posicion;
      float dx = bx - opos.x;
      float dy = by - opos.y;
      if ((dx * dx + dy * dy) <= (26.f * 26.f)) {
        colision = true;
        impacto_orbe = true;
        it_orbe->disparos_restantes--;
        if (it_orbe->disparos_restantes <= 0) {
          reclamarOrbe(*it_orbe);
          orbes.erase(it_orbe);
        }
        break;
      }
    }

    // 2. Verificar impacto de bala contra el enemigo (cuadro rojo)
    if (!colision && enemigo_rojo.vivo && enemigo_rojo.vida > 0 && enemigo_rojo.entidad) {
      auto bound = enemigo_rojo.entidad->getComponente<CE::IBoundingBox>();
      auto epos = enemigo_rojo.entidad->getTransformada()->posicion;
      if (bound) {
        float minX = epos.x - bound->mitad.x - 6.f;
        float maxX = epos.x + bound->mitad.x + 6.f;
        float minY = epos.y - bound->mitad.y - 6.f;
        float maxY = epos.y + bound->mitad.y + 6.f;

        if (bx >= minX && bx <= maxX && by >= minY && by <= maxY) {
          colision = true;
          enemigo_rojo.vida = std::max(0, enemigo_rojo.vida - danio_bala);
          enemigo_rojo.entidad->getStats()->hp = enemigo_rojo.vida;

          // Reglas de drop de Orbes según GDD:
          // Primero alcanzar el umbral de daño continuo (x), luego cada ráfaga de 3 impactos seguidos
          if (danio_continuo_jefe < umbral_danio_drops) {
            danio_continuo_jefe += danio_bala;
          } else {
            impactos_consecutivos_jefe++;
            if (impactos_consecutivos_jefe >= impactos_requeridos_drop) {
              impactos_consecutivos_jefe = 0; // Cadencia constante de 3 impactos
              evaluarDropOrbe();
            }
          }

          if (enemigo_rojo.vida <= 0) {
            enemigo_rojo.vivo = false;
          }
        }
      }
    }

    // 3. Colisión con paredes del mapa después de unos milisegundos de gracia
    if (!colision && it->tiempo_transcurrido > 0.05f) {
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
          // Fallar un tiro contra la pared rompe la ráfaga consecutiva contra el jefe
          if (!impacto_orbe) {
            danio_continuo_jefe = 0;
            impactos_consecutivos_jefe = 0;
          }
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
      if (!estaRodando() && !recargando && balas_cargador > 0 && tiempo_ultimo_disparo >= cadencia_disparo) {
        dispararBala();
      }
    }
    if (accion.getNombre() == "recargar") {
      iniciarRecarga();
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
  if (enemigo_rojo.vivo && enemigo_rojo.vida > 0) {
    CE::Render::Get().AddToDraw(enemigo_rojo.cuadro);
  }
  for (auto &orbe : orbes) {
    if (!orbe.entidad)
      continue;
    CE::Render::Get().AddToDraw(*orbe.entidad);

    // 4 cuadritos sobre el orbe para indicar cuántos golpes le quedan antes de romperse
    auto opos = orbe.entidad->getTransformada()->posicion;
    const int max_golpes = 4;
    const float tam_cuadrito = 7.f;
    const float espacio = 3.5f;
    const float ancho_total =
        max_golpes * tam_cuadrito + (max_golpes - 1) * espacio;
    float inicioX = opos.x - (ancho_total / 2.f);
    float posY = opos.y - 34.f;

    sf::Color colorActivo = (orbe.tipo == TipoOrbe::PERMANENTE_VERDE)
                                ? sf::Color(40, 235, 95, 240)
                                : sf::Color(195, 65, 255, 240);
    sf::Color colorBorde = (orbe.tipo == TipoOrbe::PERMANENTE_VERDE)
                               ? sf::Color(15, 70, 30, 240)
                               : sf::Color(55, 15, 85, 240);

    for (int i = 0; i < max_golpes; ++i) {
      sf::RectangleShape cuadrito{{tam_cuadrito, tam_cuadrito}};
      cuadrito.setPosition({inicioX + i * (tam_cuadrito + espacio), posY});
      cuadrito.setOutlineThickness(1.2f);
      if (i < orbe.disparos_restantes) {
        cuadrito.setFillColor(colorActivo);
        cuadrito.setOutlineColor(colorBorde);
      } else {
        cuadrito.setFillColor(sf::Color(16, 20, 30, 185));
        cuadrito.setOutlineColor(sf::Color(60, 70, 90, 190));
      }
      CE::Render::Get().AddToDraw(cuadrito);
    }
  }
  for (auto &bala : proyectiles)
    CE::Render::Get().AddToDraw(*bala.entidad);
  CE::Render::Get().AddToDraw(*jugador_soldier);

  // Flechas indicadoras de dirección para orbes fuera del rango de la cámara
  if (jugador_soldier && !orbes.empty()) {
    auto &cam_activa = CE::GestorCamaras::Get().getCamaraActiva();
    sf::Vector2f camCenter = cam_activa.getView().getCenter();
    sf::Vector2f camSize = cam_activa.getView().getSize();
    float halfW = camSize.x / 2.f;
    float halfH = camSize.y / 2.f;

    auto jpos = jugador_soldier->getTransformada()->posicion;

    for (const auto &orbe : orbes) {
      if (!orbe.entidad)
        continue;

      auto opos = orbe.entidad->getTransformada()->posicion;
      bool dentro_camara = (std::abs(opos.x - camCenter.x) <= halfW) &&
                           (std::abs(opos.y - camCenter.y) <= halfH);

      // Solo dibujar la flecha si el orbe NO está dentro del rango de la cámara
      if (!dentro_camara) {
        float dx = opos.x - jpos.x;
        float dy = opos.y - jpos.y;
        float angulo = std::atan2(dy, dx);
        float radio_flecha = 58.f;

        sf::ConvexShape flecha{4};
        flecha.setPoint(0, sf::Vector2f{11.f, 0.f});    // Punta de la flecha
        flecha.setPoint(1, sf::Vector2f{-7.f, -7.f});   // Aleta superior
        flecha.setPoint(2, sf::Vector2f{-3.f, 0.f});    // Muesca central
        flecha.setPoint(3, sf::Vector2f{-7.f, 7.f});    // Aleta inferior

        if (orbe.tipo == TipoOrbe::PERMANENTE_VERDE) {
          flecha.setFillColor(sf::Color(40, 235, 95, 240));
          flecha.setOutlineColor(sf::Color(10, 60, 25, 245));
        } else {
          flecha.setFillColor(sf::Color(195, 65, 255, 240));
          flecha.setOutlineColor(sf::Color(45, 10, 75, 245));
        }
        flecha.setOutlineThickness(1.5f);
        flecha.setPosition({jpos.x + std::cos(angulo) * radio_flecha,
                            jpos.y + std::sin(angulo) * radio_flecha});
        flecha.setRotation(sf::radians(angulo));

        CE::Render::Get().AddToDraw(flecha);
      }
    }
  }

  // Barra de progreso de recarga sobre el jugador
  if (recargando && jugador_soldier) {
    auto trans = jugador_soldier->getTransformada();
    if (trans) {
      auto pos = trans->posicion;
      float ancho_barra = 48.f;
      float alto_barra = 6.f;
      float progreso = std::clamp(tiempo_recarga_actual / tiempo_recarga_total, 0.0f, 1.0f);

      float barX = pos.x - (ancho_barra / 2.f);
      float barY = pos.y - 56.f;

      // Fondo oscuro con contorno
      sf::RectangleShape barra_fondo{{ancho_barra, alto_barra}};
      barra_fondo.setPosition({barX, barY});
      barra_fondo.setFillColor(sf::Color(14, 18, 28, 220));
      barra_fondo.setOutlineThickness(1.5f);
      barra_fondo.setOutlineColor(sf::Color(45, 65, 95, 230));
      CE::Render::Get().AddToDraw(barra_fondo);

      // Barra de relleno según el progreso de la recarga
      if (progreso > 0.001f) {
        sf::RectangleShape barra_progreso{{ancho_barra * progreso, alto_barra}};
        barra_progreso.setPosition({barX, barY});
        barra_progreso.setFillColor(sf::Color(0, 230, 255, 240));
        CE::Render::Get().AddToDraw(barra_progreso);
      }
    }
  }

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
  } else if (auto cbh = dynamic_cast<CE::CamaraBulletHell *>(cam)) {
    if (jugador_soldier) {
      auto jpos = jugador_soldier->getTransformada()->posicion;
      sf::Vector2f caja_tam{cbh->max_offset.x * 2.f, cbh->max_offset.y * 2.f};
      sf::RectangleShape caja_roja{caja_tam};
      caja_roja.setOrigin({cbh->max_offset.x, cbh->max_offset.y});
      caja_roja.setPosition({jpos.x, jpos.y});
      caja_roja.setOutlineThickness(2.f);
      caja_roja.setOutlineColor(sf::Color(255, 50, 50, 180));
      caja_roja.setFillColor(sf::Color::Transparent);
      CE::Render::Get().AddToDraw(caja_roja);
    }
  }
#endif

  // --- Renderizado del HUD en coordenadas de pantalla fija ---
  auto &tex = CE::Render::Get().GetTextura();
  sf::View prevView = tex.getView();
  tex.setView(tex.getDefaultView());

  float screenW = (float)tex.getSize().x;
  float screenH = (float)tex.getSize().y;

  // Panel contenedor en la esquina inferior derecha
  float panelW = 320.f;
  float panelH = 75.f;
  float panelX = screenW - panelW - 20.f;
  float panelY = screenH - panelH - 18.f;

  // Fondo semi-transparente del HUD
  sf::RectangleShape panelHUD{{panelW, panelH}};
  panelHUD.setPosition({panelX, panelY});
  panelHUD.setFillColor(sf::Color(14, 18, 28, 205));
  panelHUD.setOutlineThickness(1.5f);
  panelHUD.setOutlineColor(sf::Color(45, 65, 95, 210));
  tex.draw(panelHUD);

  // Línea decorativa superior neón cyan
  sf::RectangleShape lineaNeon{{panelW, 2.f}};
  lineaNeon.setPosition({panelX, panelY});
  lineaNeon.setFillColor(sf::Color(0, 230, 255, 220));
  tex.draw(lineaNeon);

  // 1. Imagen del arma del soldado en la parte derecha del panel
  if (sprite_arma_hud) {
    sprite_arma_hud->setPosition({panelX + 165.f, panelY + 9.f});
    tex.draw(*sprite_arma_hud);
  }

  // 2. Balas restantes al lado izquierdo de la imagen
  const auto &font = CE::GestorAssets::Get().getFont("default_font");

  // Número del cargador actual (más grande)
  sf::Text txtBalas{font, std::to_string(balas_cargador), 36};
  txtBalas.setStyle(sf::Text::Style::Bold);

  if (balas_cargador > 6) {
    txtBalas.setFillColor(sf::Color(255, 255, 255, 255));
  } else if (balas_cargador > 0) {
    txtBalas.setFillColor(sf::Color(255, 190, 40, 255)); // Ámbar
  } else {
    txtBalas.setFillColor(sf::Color(255, 50, 60, 255)); // Rojo crítico
  }
  txtBalas.setPosition({panelX + 20.f, panelY + 12.f});
  tex.draw(txtBalas);

  // Separador y reserva (ej. " / 360")
  float wBalas = txtBalas.getLocalBounds().size.x;
  sf::Text txtReserva{font, " / " + std::to_string(balas_reserva), 22};
  txtReserva.setStyle(sf::Text::Style::Bold);
  txtReserva.setFillColor(sf::Color(135, 150, 175, 210));
  txtReserva.setPosition({panelX + 22.f + wBalas, panelY + 23.f});
  tex.draw(txtReserva);

  // Indicador inferior
  if (recargando) {
    sf::Text txtAlerta{font, "RECARGANDO...", 13};
    txtAlerta.setStyle(sf::Text::Style::Bold);
    txtAlerta.setFillColor(sf::Color(0, 230, 255, 240));
    txtAlerta.setPosition({panelX + 20.f, panelY + 52.f});
    tex.draw(txtAlerta);
  } else if (balas_cargador == 0) {
    sf::Text txtAlerta{font, "[ R ] RECARGAR", 13};
    txtAlerta.setStyle(sf::Text::Style::Bold);
    txtAlerta.setFillColor(sf::Color(255, 70, 70, 240));
    txtAlerta.setPosition({panelX + 20.f, panelY + 52.f});
    tex.draw(txtAlerta);
  } else {
    sf::Text txtEtiqueta{font, "MUNICION", 11};
    txtEtiqueta.setFillColor(sf::Color(105, 125, 155, 180));
    txtEtiqueta.setPosition({panelX + 20.f, panelY + 54.f});
    tex.draw(txtEtiqueta);
  }

  // --- Barra de vida del enemigo (cuadro rojo) en la parte inferior de la pantalla ---
  if (enemigo_rojo.vivo && enemigo_rojo.vida > 0) {
    float maxDisponibleW = panelX - 40.f;
    float enemyBarW = std::clamp(maxDisponibleW, 180.f, 420.f);
    float enemyBarH = 22.f;
    float enemyBarX = (panelX - enemyBarW) / 2.f;
    float enemyBarY = screenH - enemyBarH - 26.f;

    float ratioVida = std::clamp(
        (float)enemigo_rojo.vida / (float)enemigo_rojo.vida_max, 0.0f, 1.0f);

    // Fondo de la barra de vida
    sf::RectangleShape barraVidaFondo{{enemyBarW, enemyBarH}};
    barraVidaFondo.setPosition({enemyBarX, enemyBarY});
    barraVidaFondo.setFillColor(sf::Color(14, 18, 28, 215));
    barraVidaFondo.setOutlineThickness(2.f);
    barraVidaFondo.setOutlineColor(sf::Color(90, 35, 45, 230));
    tex.draw(barraVidaFondo);

    // Relleno rojo proporcional a la vida actual
    if (ratioVida > 0.0f) {
      sf::RectangleShape barraVidaRelleno{{enemyBarW * ratioVida, enemyBarH}};
      barraVidaRelleno.setPosition({enemyBarX, enemyBarY});
      barraVidaRelleno.setFillColor(sf::Color(220, 35, 45, 240));
      tex.draw(barraVidaRelleno);
    }

    // Etiqueta superior e indicador numérico de vida (ej. "200 / 200")
    sf::Text txtEnemigo{font, "ENEMIGO", 13};
    txtEnemigo.setStyle(sf::Text::Style::Bold);
    txtEnemigo.setFillColor(sf::Color(255, 110, 110, 235));
    txtEnemigo.setPosition({enemyBarX, enemyBarY - 20.f});
    tex.draw(txtEnemigo);

    std::string strVida = std::to_string(enemigo_rojo.vida) + " / " +
                          std::to_string(enemigo_rojo.vida_max);
    sf::Text txtVidaNum{font, strVida, 14};
    txtVidaNum.setStyle(sf::Text::Style::Bold);
    txtVidaNum.setFillColor(sf::Color(255, 255, 255, 245));
    auto boundsVida = txtVidaNum.getLocalBounds();
    txtVidaNum.setPosition({enemyBarX + (enemyBarW - boundsVida.size.x) / 2.f,
                            enemyBarY + 2.f});
    tex.draw(txtVidaNum);
  }

  tex.setView(prevView);

  // Renderizar menú de pausa cuando el juego esté pausado
  if (en_pausa) {
    prevView = tex.getView();
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
  if (en_pausa || !jugador_soldier || recargando)
    return;

  // Si el soldado está realizando una voltereta (esquive), el arma deja de disparar
  if (estaRodando())
    return;

  // Si el cargador está vacío, no se puede disparar (requiere recargar con 'R')
  if (balas_cargador <= 0)
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
  balas_cargador--;
  tiempo_ultimo_disparo = 0.f;
}

bool Escena_Propia::iniciarRecarga() {
  if (recargando || en_pausa || estaRodando())
    return false;

  // Evita recargar si el cargador está completo o si no hay munición en reserva
  if (balas_cargador >= balas_cargador_max || balas_reserva <= 0)
    return false;

  recargando = true;
  tiempo_recarga_actual = 0.0f;
  return true;
}

void Escena_Propia::recargarArma() {
  if (en_pausa)
    return;

  // Si el cargador ya está lleno o no hay munición en reserva, no recargar
  if (balas_cargador >= balas_cargador_max || balas_reserva <= 0)
    return;

  // Se pierden las balas restantes del cargador actual dando pie a un nuevo cargador de 24
  if (balas_reserva >= balas_cargador_max) {
    balas_cargador = balas_cargador_max;
    balas_reserva -= balas_cargador_max;
  } else {
    // Si la reserva tiene menos de 24 balas
    balas_cargador = balas_reserva;
    balas_reserva = 0;
  }
}

bool Escena_Propia::estaRodando() const {
  if (!jugador_soldier)
    return false;
  auto ctrl = jugador_soldier->getComponente<CE::IControl>();
  if (ctrl && ctrl->sacc)
    return true;
  auto me = jugador_soldier->getComponente<IMaquinaEstado>();
  if (!me || !me->fsm)
    return false;
  return (me->fsm->getNombre().find("Spin") != std::string::npos);
}

void Escena_Propia::evaluarDropOrbe() {
  // Tasas de probabilidad según GDD:
  // - Mejora permanente (Orbe Verde): 1/9 (~11.1%)
  // - Mejora temporal (Orbe Morado): 1/4 (25.0%)
  bool drop_permanente = ((std::rand() % 9) == 0);
  bool drop_temporal = !drop_permanente && ((std::rand() % 4) == 0);

  if (drop_permanente) {
    fallos_rng_orbe = 0;
    generarOrbe(TipoOrbe::PERMANENTE_VERDE);
    return;
  }

  if (drop_temporal) {
    fallos_rng_orbe = 0;
    generarOrbe(TipoOrbe::TEMPORAL_MORADO);
    return;
  }

  // Sistema de Mitigación de Mala Racha (Pity Timer):
  // A los 3 chequeos consecutivos de RNG sin drop, garantiza una mejora con resolución 50/50
  fallos_rng_orbe++;
  if (fallos_rng_orbe >= max_fallos_pity) {
    fallos_rng_orbe = 0;
    TipoOrbe tipo_garantizado = ((std::rand() % 2) == 0)
                                    ? TipoOrbe::PERMANENTE_VERDE
                                    : TipoOrbe::TEMPORAL_MORADO;
    generarOrbe(tipo_garantizado);
  }
}

void Escena_Propia::generarOrbe(TipoOrbe tipo) {
  float spawnX = 600.f;
  float spawnY = 300.f;

  // Seleccionar cualquier celda libre del TileMap (donde id != -1 y sin colisión 'c')
  if (!tiles_industrial.empty() && !tiles_industrial[0].getCeldasLibres().empty()) {
    const auto &celdas = tiles_industrial[0].getCeldasLibres();
    for (int intento = 0; intento < 20; ++intento) {
      const auto &celda = celdas[std::rand() % celdas.size()];
      spawnX = celda.x;
      spawnY = celda.y;

      bool muy_cerca = false;
      if (jugador_soldier) {
        auto jpos = jugador_soldier->getTransformada()->posicion;
        float dx = spawnX - jpos.x;
        float dy = spawnY - jpos.y;
        if ((dx * dx + dy * dy) < (65.f * 65.f))
          muy_cerca = true;
      }
      if (!muy_cerca && enemigo_rojo.vivo && enemigo_rojo.entidad) {
        auto epos = enemigo_rojo.entidad->getTransformada()->posicion;
        float dx = spawnX - epos.x;
        float dy = spawnY - epos.y;
        if ((dx * dx + dy * dy) < (60.f * 60.f))
          muy_cerca = true;
      }
      if (!muy_cerca)
        break;
    }
  }

  auto orbe_ent = std::make_shared<Entidad>();
  orbe_ent->getNombre()->nombre =
      (tipo == TipoOrbe::PERMANENTE_VERDE) ? "Orbe_Verde_Permanente" : "Orbe_Morado_Temporal";
  orbe_ent->setPosicion(spawnX, spawnY);

  const std::string llave_tex =
      (tipo == TipoOrbe::PERMANENTE_VERDE) ? "orbe_verde_K" : "orbe_morado_K";

  auto sprite_orbe = std::make_shared<CE::ISprite>(
      CE::GestorAssets::Get().getTextura(llave_tex),
      64, 64,
      0.75f);
  orbe_ent->addComponente(sprite_orbe);
  orbe_ent->addComponente(std::make_shared<CE::IBoundingBox>(CE::Vector2D{44.f, 44.f}));
  orbe_ent->onUpdate(0.f);

  OrbeMejora nuevo_orbe;
  nuevo_orbe.entidad = orbe_ent;
  nuevo_orbe.tipo = tipo;
  nuevo_orbe.disparos_restantes = 4;
  nuevo_orbe.tiempo_vida_max = 15.0f;
  nuevo_orbe.tiempo_transcurrido = 0.0f;

  orbes.push_back(nuevo_orbe);
}

void Escena_Propia::reclamarOrbe(const OrbeMejora &orbe) {
  // Por el momento solo se absorbe/reclama el orbe sin aplicar mejoras al jugador
  (void)orbe;
}

} // namespace IVJ
