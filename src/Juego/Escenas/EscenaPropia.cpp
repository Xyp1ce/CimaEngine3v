#include "Juego/Escenas/EscenaPropia.hpp"
#include "Motor/Utils/Vector2D.hpp"
#include <Juego/Componentes/IJComponentes.hpp>
#include <Juego/Maquinas/Soldier/IdleSoldier.hpp>
#include <Juego/Sistemas/Sistemas.hpp>
#include <Juego/objetos/Entidad.hpp>
#include <Motor/Camaras/CamarasGestor.hpp>
#include <Motor/Componentes/IComponentes.hpp>
#include <Motor/Inputs/Botones.hpp>
#include <Motor/Primitivos/GestorAssets.hpp>
#include <Motor/Primitivos/GestorEscenas.hpp>
#include <Motor/Render/Render.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <memory>
#include <sstream>

namespace IVJ {

namespace {
constexpr float PI = 3.14159265358979323846f;
constexpr float TWO_PI = 6.28318530717958647692f;

sf::String toUtf8(const std::string &s) {
  return sf::String::fromUtf8(s.begin(), s.end());
}
} // namespace

Escena_Propia::Escena_Propia(std::shared_ptr<Entidad> &pref)
    : CE::Escena{}, jugador_soldier{pref} {
  nombre = "Propia";
}

void Escena_Propia::cargarAudio() {
  if (audio_cargado)
    return;

  const std::vector<std::pair<std::string, std::string>> sonidos = {
      {"disparo", ASSETS "/sonidos/expendable/disparo_jugador.wav"},
      {"recarga", ASSETS "/sonidos/expendable/recarga.wav"},
      {"dash", ASSETS "/sonidos/expendable/dash.wav"},
      {"hit_jefe", ASSETS "/sonidos/expendable/hit_jefe.wav"},
      {"hit_jugador", ASSETS "/sonidos/expendable/hit_jugador.wav"},
      {"hit_orbe", ASSETS "/sonidos/expendable/hit_orbe.wav"},
      {"pickup_morado", ASSETS "/sonidos/expendable/pickup_morado.wav"},
      {"boss_telegraph", ASSETS "/sonidos/expendable/boss_telegraph.wav"},
      {"boss_disparo", ASSETS "/sonidos/expendable/boss_disparo.wav"}};

  for (const auto &par : sonidos) {
    sf::SoundBuffer buf;
    if (buf.loadFromFile(par.second)) {
      sfx_buffers.emplace(par.first, std::move(buf));
    }
  }
  audio_cargado = true;
}

void Escena_Propia::reproducirSFX(const std::string &clave, float volumen,
                                  float pitch) {
  auto it = sfx_buffers.find(clave);
  if (it == sfx_buffers.end())
    return;

  for (auto &snd : sfx_pool) {
    if (snd.getStatus() == sf::Sound::Status::Stopped) {
      snd.setBuffer(it->second);
      snd.setVolume(volumen);
      snd.setPitch(pitch);
      snd.play();
      return;
    }
  }

  if (sfx_pool.size() < 24) {
    sfx_pool.emplace_back(it->second);
    sfx_pool.back().setVolume(volumen);
    sfx_pool.back().setPitch(pitch);
    sfx_pool.back().play();
  }
}

void Escena_Propia::onInit() {
  if (inicializar) {
    jugador_soldier = std::make_shared<Entidad>();

    // Registrar controles (GDD + controles existentes)
    registrarBotones(sf::Keyboard::Scancode::W, "arriba");
    registrarBotones(sf::Keyboard::Scancode::Up, "arriba");
    registrarBotones(sf::Keyboard::Scancode::S, "abajo");
    registrarBotones(sf::Keyboard::Scancode::Down, "abajo");
    registrarBotones(sf::Keyboard::Scancode::A, "izquierda");
    registrarBotones(sf::Keyboard::Scancode::Left, "izquierda");
    registrarBotones(sf::Keyboard::Scancode::D, "derecha");
    registrarBotones(sf::Keyboard::Scancode::Right, "derecha");
    registrarBotones(sf::Keyboard::Scancode::Enter, "aceptar");
    registrarBotones(sf::Keyboard::Scancode::Space, "espacio");
    registrarBotones(sf::Keyboard::Scancode::Escape, "pausa");
    registrarBotones(sf::Mouse::Button::Right, "accion_secundaria");
    registrarBotones(sf::Mouse::Button::Left, "disparar");
    registrarBotones(sf::Keyboard::Scancode::R, "recargar");

    // Cargar mapa de arena circular brutalista construido con tilemap2.png
    tiles_industrial.push_back(TileMap());
    if (!tiles_industrial[0].loadTileMap(
            ASSETS "/mapas/arena_expendable_piso1.txt", objetos)) {
      exit(EXIT_FAILURE);
    }

    auto &assets = CE::GestorAssets::Get();

    // Texturas del Soldado (caminata, idle respirando, volteretas y derrota)
    assets.agregarTextura("soldier_down_sheet_K",
                          ASSETS "/sprites/jugador/soldier_down_sheet.png",
                          CE::Vector2D{0.f, 0.f}, CE::Vector2D{256.f, 84.f});
    assets.agregarTextura("soldier_up_sheet_K",
                          ASSETS "/sprites/jugador/soldier_up_sheet.png",
                          CE::Vector2D{0.f, 0.f}, CE::Vector2D{256.f, 84.f});
    assets.agregarTextura("soldier_right_sheet_K",
                          ASSETS "/sprites/jugador/soldier_right_sheet.png",
                          CE::Vector2D{0.f, 0.f}, CE::Vector2D{256.f, 84.f});
    assets.agregarTextura("soldier_left_sheet_K",
                          ASSETS "/sprites/jugador/soldier_left_sheet.png",
                          CE::Vector2D{0.f, 0.f}, CE::Vector2D{256.f, 84.f});

    assets.agregarTextura("soldier_idle_down_sheet_K",
                          ASSETS "/sprites/jugador/soldier_idle_down_sheet.png",
                          CE::Vector2D{0.f, 0.f}, CE::Vector2D{256.f, 84.f});
    assets.agregarTextura("soldier_idle_up_sheet_K",
                          ASSETS "/sprites/jugador/soldier_idle_up_sheet.png",
                          CE::Vector2D{0.f, 0.f}, CE::Vector2D{256.f, 84.f});
    assets.agregarTextura("soldier_idle_right_sheet_K",
                          ASSETS "/sprites/jugador/soldier_idle_right_sheet.png",
                          CE::Vector2D{0.f, 0.f}, CE::Vector2D{256.f, 84.f});
    assets.agregarTextura("soldier_idle_left_sheet_K",
                          ASSETS "/sprites/jugador/soldier_idle_left_sheet.png",
                          CE::Vector2D{0.f, 0.f}, CE::Vector2D{256.f, 84.f});

    assets.agregarTextura("soldier_right_spin_sheet_K",
                          ASSETS "/sprites/jugador/soldier_right_spin_sheet.png",
                          CE::Vector2D{0.f, 0.f}, CE::Vector2D{384.f, 84.f});
    assets.agregarTextura("soldier_left_spin_sheet_K",
                          ASSETS "/sprites/jugador/soldier_left_spin_sheet.png",
                          CE::Vector2D{0.f, 0.f}, CE::Vector2D{384.f, 84.f});
    assets.agregarTextura("soldier_down_spin_sheet_K",
                          ASSETS "/sprites/jugador/soldier_down_spin_sheet.png",
                          CE::Vector2D{0.f, 0.f}, CE::Vector2D{384.f, 84.f});
    assets.agregarTextura("soldier_up_spin_sheet_K",
                          ASSETS "/sprites/jugador/soldier_up_spin_sheet.png",
                          CE::Vector2D{0.f, 0.f}, CE::Vector2D{384.f, 84.f});

    assets.agregarTextura("soldier_death_sheet_K",
                          ASSETS "/sprites/jugador/soldier_death_sheet.png",
                          CE::Vector2D{0.f, 0.f}, CE::Vector2D{384.f, 84.f});

    // Proyectiles azules del jugador, orbes estilo animación y arma HUD
    assets.agregarTextura("proyectil_neon_ai_K",
                          ASSETS "/sprites/proyectil_neon_ai.png",
                          CE::Vector2D{0.f, 0.f}, CE::Vector2D{64.f, 24.f});
    assets.agregarTextura("orbe_verde_ai_K",
                          ASSETS "/sprites/orbe_verde_ai.png",
                          CE::Vector2D{0.f, 0.f}, CE::Vector2D{128.f, 128.f});
    assets.agregarTextura("orbe_morado_ai_K",
                          ASSETS "/sprites/orbe_morado_ai.png",
                          CE::Vector2D{0.f, 0.f}, CE::Vector2D{128.f, 128.f});
    assets.agregarTextura("arma_soldado_ai_K",
                          ASSETS "/sprites/arma_soldado_ai.png",
                          CE::Vector2D{0.f, 0.f}, CE::Vector2D{200.f, 76.f});

    sprite_arma_hud.emplace(assets.getTextura("arma_soldado_ai_K"));
    sprite_arma_hud->setScale({0.72f, 0.72f});

    // Configurar componentes del Soldado (se conservan al reentrar)
    jugador_soldier->getNombre()->nombre = "Soldier_001";
    auto trans = jugador_soldier->getTransformada();
    trans->velocidad = CE::Vector2D{340.f, 340.f};
    jugador_soldier->setPosicion(pos_spawn_jugador.x, pos_spawn_jugador.y);

    auto sprite = std::make_shared<CE::ISprite>(
        assets.getTextura("soldier_up_sheet_K"), 64, 84, 1.f);
    sprite->m_sprite.setTexture(assets.getTextura("soldier_up_sheet_K"));
    jugador_soldier->addComponente(sprite);
    jugador_soldier->addComponente(std::make_shared<CE::IControl>());
    // Hitbox táctica ajustada al torso/pies del soldado para movilidad ágil
    jugador_soldier->addComponente(
        std::make_shared<CE::IBoundingBox>(CE::Vector2D{34.f, 44.f}));

    auto me = std::make_shared<IMaquinaEstado>();
    me->fsm = std::make_shared<IdleSoldier>();
    jugador_soldier->addComponente(me);
    jugador_soldier->setFSM(me->fsm);

    // Cargar texturas del Jefe AEGIS-01 y SFX
    boss.cargarTexturas();
    cargarAudio();

    inicializar = false;
  }

  reiniciarPartida();

  // Mantener EXACTAMENTE la cámara CamaraBulletHell que ya estaba en uso en Escena_Propia
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

void Escena_Propia::reiniciarPartida() {
  reglas = ConfigExpendable::Get().getReglasActuales();

  en_pausa = false;
  opcion_pausa = 0;
  opcion_fin = 0;
  fase_partida = FasePartida::INTRO;
  timer_intro = 0.f;
  tiempo_combate = 0.f;
  tiempo_global = 0.f;

  build.reset();
  hp_max_jugador = reglas.hp_jugador;
  hp_jugador = reglas.hp_jugador;
  timer_invuln_golpe = 0.f;
  timer_flash_danio_hud = 0.f;
  cooldown_dash_restante = 0.f;
  estaba_rodando = false;
  timer_afterimage = 0.f;
  frame_muerte_jugador = 0;
  timer_muerte_jugador = 0.f;

  disparos_totales = 0;
  disparos_acertados = 0;
  danio_total_infligido = 0;
  contador_salva = 0;
  ultima_salva_con_impacto = -1;

  proyectiles.clear();
  proyectiles_boss.clear();
  orbes.clear();
  particulas.clear();
  ondas.clear();
  textos_flotantes.clear();
  afterimages.clear();
  muzzle_flash = MuzzleFlashVFX{};

  impactos_consecutivos_jefe = 0;
  impactos_requeridos_drop = reglas.impactos_para_orbe;

  balas_cargador = build.tamCargadorMax();
  balas_reserva = balas_reserva_inicial;
  recargando = false;
  tiempo_recarga_actual = 0.f;
  tiempo_ultimo_disparo = build.cadenciaDisparo();

  // Posicionar jugador hasta abajo de la arena circular mirando arriba hacia el jefe
  if (jugador_soldier) {
    jugador_soldier->setPosicion(pos_spawn_jugador.x, pos_spawn_jugador.y);
    auto trans = jugador_soldier->getTransformada();
    if (trans) {
      float v = build.velocidadMovimiento();
      trans->velocidad = CE::Vector2D{v, v};
      trans->pos_prev = pos_spawn_jugador;
      trans->angulo = 0.f;
    }
    jugador_soldier->getStats()->hp_max = hp_max_jugador;
    jugador_soldier->getStats()->hp = hp_jugador;

    auto sprite = jugador_soldier->getComponente<CE::ISprite>();
    if (sprite) {
      sprite->m_sprite.setTexture(
          CE::GestorAssets::Get().getTextura("soldier_up_sheet_K"));
      sprite->m_sprite.setTextureRect(
          sf::IntRect{{0, 0}, {sprite->width, sprite->height}});
      sprite->m_sprite.setColor(sf::Color::White);
    }
    auto me = jugador_soldier->getComponente<IMaquinaEstado>();
    if (me) {
      me->fsm = std::make_shared<IdleSoldier>();
      jugador_soldier->setFSM(me->fsm);
    }
    limpiarControles();
  }

  // Inicializar Jefe AEGIS-01 exactamente en el centro de la arena circular
  boss.inicializar(centro_arena, reglas.boss_hp, reglas.mult_vel_balas_boss);

  // Recolocar cámara suavemente sobre el jugador al reiniciar
  auto &cam = CE::GestorCamaras::Get().getCamaraActiva();
  if (cam.nombre == "CamaraBulletHell" && jugador_soldier) {
    cam.lockEnObjeto(jugador_soldier);
    cam.getTransformada().posicion = pos_spawn_jugador;
    cam.onUpdate(0.f);
  }
}

void Escena_Propia::onFinal() {
  en_pausa = false;
  recargando = false;
  tiempo_recarga_actual = 0.0f;
  limpiarControles();
  proyectiles.clear();
  proyectiles_boss.clear();
  orbes.clear();
  particulas.clear();
  ondas.clear();
  textos_flotantes.clear();
  afterimages.clear();
  impactos_consecutivos_jefe = 0;
  CE::GestorCamaras::Get().setCamaraActiva(0);
}

void Escena_Propia::onUpdate(float dt) {
  if (en_pausa)
    return;

  // Limitar dt para estabilidad física ante pausas del SO o cambio de foco
  dt = std::min(dt, 0.05f);
  tiempo_global += dt;

  // Actualizar VFX siempre (incluso en Victoria/Derrota para ver explosiones finales)
  for (auto it = particulas.begin(); it != particulas.end();) {
    it->vida -= dt;
    if (it->vida <= 0.f) {
      it = particulas.erase(it);
      continue;
    }
    it->pos.x += it->vel.x * dt;
    it->pos.y += it->vel.y * dt;
    float damp = std::exp(-3.2f * dt);
    it->vel.x *= damp;
    it->vel.y *= damp;
    ++it;
  }

  for (auto it = ondas.begin(); it != ondas.end();) {
    it->vida -= dt;
    if (it->vida <= 0.f)
      it = ondas.erase(it);
    else
      ++it;
  }

  for (auto it = textos_flotantes.begin(); it != textos_flotantes.end();) {
    it->vida -= dt;
    it->pos.y -= 34.f * dt;
    if (it->vida <= 0.f)
      it = textos_flotantes.erase(it);
    else
      ++it;
  }

  for (auto it = afterimages.begin(); it != afterimages.end();) {
    it->vida -= dt;
    if (it->vida <= 0.f)
      it = afterimages.erase(it);
    else
      ++it;
  }

  if (muzzle_flash.vida > 0.f)
    muzzle_flash.vida = std::max(0.f, muzzle_flash.vida - dt);
  if (timer_flash_danio_hud > 0.f)
    timer_flash_danio_hud = std::max(0.f, timer_flash_danio_hud - dt);

  // Fase DERROTA: animar caída del soldado (soldier_death_sheet.png)
  if (fase_partida == FasePartida::DERROTA) {
    timer_muerte_jugador += dt;
    if (frame_muerte_jugador < 3 && timer_muerte_jugador >= 0.16f) {
      timer_muerte_jugador = 0.f;
      frame_muerte_jugador++;
    }
    return;
  }

  // Fase VICTORIA: terminar fade/explosión del jefe
  if (fase_partida == FasePartida::VICTORIA) {
    boss.actualizar(dt, jugador_soldier->getTransformada()->posicion,
                    proyectiles_boss, 1.0f);
    return;
  }

  // Fase INTRO breve (1.35s) donde el jugador ya puede moverse un poco pero el jefe activa sistemas
  if (fase_partida == FasePartida::INTRO) {
    timer_intro += dt;
    if (timer_intro >= duracion_intro) {
      fase_partida = FasePartida::COMBATE;
      boss.activarCombate();
      spawnOnda(boss.getPosicion(), 40.f, 220.f, sf::Color(255, 130, 30, 220),
                0.45f);
      reproducirSFX("boss_telegraph", 55.f, 0.9f);
    }
  } else {
    tiempo_combate += dt;
  }

  // Temporizadores del jugador y mejoras temporales
  build.actualizarTemporales(dt);
  if (timer_invuln_golpe > 0.f)
    timer_invuln_golpe = std::max(0.f, timer_invuln_golpe - dt);
  if (cooldown_dash_restante > 0.f)
    cooldown_dash_restante = std::max(0.f, cooldown_dash_restante - dt);

  // Si estamos en sobrecarga con munición infinita, cancelar recarga pendiente
  if (build.tieneMunicionInfinita() && recargando) {
    recargando = false;
    tiempo_recarga_actual = 0.f;
  }

  // Actualizar progreso de recarga
  float tiempo_recarga_req = build.tiempoRecarga();
  if (recargando) {
    tiempo_recarga_actual += dt;
    if (tiempo_recarga_actual >= tiempo_recarga_req) {
      tiempo_recarga_actual = tiempo_recarga_req;
      recargando = false;
      recargarArma();
    }
  }

  // Actualizar FSM del jugador
  bool rodando_antes = estaRodando();
  jugador_soldier->inputFSM();
  bool rodando_ahora = estaRodando();

  if (!rodando_antes && rodando_ahora) {
    // Acaba de iniciar un Dash / Voltereta
    cooldown_dash_restante = build.cooldownDash();
    reproducirSFX("dash", 58.f, 0.96f + (std::rand() % 12) * 0.01f);
    spawnParticulas(jugador_soldier->getTransformada()->posicion, 10,
                    sf::Color(90, 245, 255, 220), 60.f, 180.f, 3.5f, 0.28f);
  }
  estaba_rodando = rodando_ahora;

  // Mantener velocidad sincronizada con mejoras cuando NO está rodando
  if (!rodando_ahora) {
    auto trans = jugador_soldier->getTransformada();
    if (trans) {
      float v = build.velocidadMovimiento();
      trans->velocidad = CE::Vector2D{v, v};
    }
  } else {
    // Generar estela translúcida (afterimages) para señalar i-frames del Dash
    timer_afterimage -= dt;
    if (timer_afterimage <= 0.f) {
      timer_afterimage = 0.045f;
      auto sp = jugador_soldier->getComponente<CE::ISprite>();
      if (sp) {
        AfterImageDash ai;
        ai.pos = sp->m_sprite.getPosition();
        ai.textura = &sp->m_sprite.getTexture();
        ai.rect = sp->m_sprite.getTextureRect();
        ai.vida = 0.22f;
        ai.vida_max = 0.22f;
        afterimages.push_back(ai);
      }
    }
  }

  // Disparo continuo o recarga automática al mantener clic izquierdo
  tiempo_ultimo_disparo += dt;
  float cadencia_actual = build.cadenciaDisparo();
  auto ctrl = jugador_soldier->getComponente<CE::IControl>();
  if (ctrl && ctrl->acc && !rodando_ahora && !recargando) {
    if (balas_cargador > 0 || build.tieneMunicionInfinita()) {
      if (tiempo_ultimo_disparo >= cadencia_actual) {
        dispararBala();
      }
    } else if (tiempo_ultimo_disparo >= cadencia_actual) {
      iniciarRecarga();
    }
  }

  jugador_soldier->onUpdate(dt);
  moverJugadorConSliding(dt);

  // Actualizar feedback visual del sprite del jugador (i-frames dash o daño)
  auto sp_jug = jugador_soldier->getComponente<CE::ISprite>();
  if (sp_jug) {
    if (rodando_ahora) {
      sp_jug->m_sprite.setColor(sf::Color(150, 245, 255, 215));
    } else if (timer_invuln_golpe > 0.f) {
      bool blink = (static_cast<int>(timer_invuln_golpe * 20.f) % 2) == 0;
      sp_jug->m_sprite.setColor(blink ? sf::Color(255, 110, 110, 235)
                                      : sf::Color(255, 255, 255, 120));
    } else {
      sp_jug->m_sprite.setColor(sf::Color::White);
    }
  }

  // Actualizar Jefe AEGIS-01 (5 ataques únicos + animación + Fase 2)
  CE::Vector2D pos_jug = jugador_soldier->getTransformada()->posicion;
  auto ev_boss =
      boss.actualizar(dt, pos_jug, proyectiles_boss, build.escalaVelocidadEnemiga());

  if (ev_boss.sfx_telegraph) {
    reproducirSFX("boss_telegraph", 50.f, boss.esFase2() ? 1.12f : 1.0f);
  }
  if (ev_boss.sfx_disparo) {
    reproducirSFX("boss_disparo", 42.f, 0.92f + (std::rand() % 18) * 0.01f);
  }
  if (ev_boss.sfx_fase2) {
    spawnOnda(boss.getPosicion(), 40.f, 440.f, sf::Color(255, 110, 20, 240),
              0.75f);
    spawnParticulas(boss.getPosicion(), 36, sf::Color(255, 140, 35, 255), 110.f,
                    380.f, 5.5f, 0.65f);
  }
  if (ev_boss.limpiar_balas_por_fase2) {
    for (const auto &pb : proyectiles_boss) {
      spawnParticulas(pb.pos, 2, sf::Color(255, 150, 45, 180), 30.f, 90.f, 3.f,
                      0.25f);
    }
    proyectiles_boss.clear();
  }

  // Ataque Giratorio a corta distancia (-2 HP) y Colisión corporal con Aegis (-1 HP)
  if (boss.estaVivo() && fase_partida == FasePartida::COMBATE) {
    CE::Vector2D bpos = boss.getPosicion();
    float dx = pos_jug.x - bpos.x;
    float dy = pos_jug.y - bpos.y;
    float dist = std::sqrt(dx * dx + dy * dy);
    float nx = (dist > 0.001f) ? (dx / dist) : 0.f;
    float ny = (dist > 0.001f) ? (dy / dist) : 1.f;

    auto empujarJugador = [&](float dist_objetivo) {
      float tx = bpos.x + nx * dist_objetivo;
      float ty = bpos.y + ny * dist_objetivo;
      float rx = tx - centro_arena.x;
      float ry = ty - centro_arena.y;
      float rdist = std::sqrt(rx * rx + ry * ry);
      if (rdist > radio_arena_libre && rdist > 0.001f) {
        tx = centro_arena.x + (rx / rdist) * radio_arena_libre;
        ty = centro_arena.y + (ry / rdist) * radio_arena_libre;
      }
      jugador_soldier->setPosicion(tx, ty);
      auto sp = jugador_soldier->getComponente<CE::ISprite>();
      if (sp)
        sp->m_sprite.setPosition({tx, ty});
    };

    // 1) Evaluar primero el Ataque Giratorio a corta distancia (-2 HP)
    if (boss.estaEnGiroCercanoActivo() && dist <= boss.getRadioGiroCercano()) {
      if (!rodando_ahora && timer_invuln_golpe <= 0.f) {
        empujarJugador(boss.getRadioGiroCercano() + 26.f);
        spawnParticulas(pos_jug, 24, sf::Color(255, 110, 25), 100.f, 290.f, 5.f,
                        0.45f);
        spawnOnda(pos_jug, 16.f, 120.f, sf::Color(255, 95, 25, 240), 0.35f);
        daniarJugador(2, "-2 HP // GIRO!");
      }
    }

    // 2) Colisión corporal directa contra el chasis de Aegis (-1 HP + empuje)
    pos_jug = jugador_soldier->getTransformada()->posicion;
    dx = pos_jug.x - bpos.x;
    dy = pos_jug.y - bpos.y;
    dist = std::sqrt(dx * dx + dy * dy);
    float min_sep = boss.getRadioColisionJugador() + 22.f;
    if (dist < min_sep) {
      if (dist > 0.001f) {
        nx = dx / dist;
        ny = dy / dist;
      }
      if (!rodando_ahora) {
        empujarJugador(min_sep + 14.f);
        daniarJugador(1, "-1 HP");
      }
    }
  }

  // Actualizar Orbes de Mejora (GDD: 15s expiración, parpadeo últimos 5s, contacto directo)
  pos_jug = jugador_soldier->getTransformada()->posicion;
  for (auto it = orbes.begin(); it != orbes.end();) {
    it->tiempo_transcurrido += dt;
    it->rotacion += 1.9f * dt;
    if (it->timer_flash > 0.f)
      it->timer_flash = std::max(0.f, it->timer_flash - dt);

    if (it->tiempo_transcurrido >= it->tiempo_vida_max) {
      spawnParticulas(it->pos, 8,
                      (it->tipo == TipoOrbe::PERMANENTE_VERDE)
                          ? sf::Color(60, 240, 120, 180)
                          : sf::Color(195, 80, 255, 180),
                      40.f, 110.f, 3.f, 0.35f);
      it = orbes.erase(it);
      continue;
    }

    // Obtención por contacto directo (caminar o Dash sobre el orbe)
    float dx = pos_jug.x - it->pos.x;
    float dy = pos_jug.y - it->pos.y;
    if ((dx * dx + dy * dy) <= (46.f * 46.f)) {
      reclamarOrbe(*it);
      it = orbes.erase(it);
      continue;
    }
    ++it;
  }

  // Actualizar Proyectiles Azules del Jugador
  for (auto it = proyectiles.begin(); it != proyectiles.end();) {
    it->tiempo_transcurrido += dt;
    if (it->tiempo_transcurrido >= it->tiempo_vida) {
      if (it->id_salva != ultima_salva_con_impacto) {
        impactos_consecutivos_jefe = 0;
      }
      it = proyectiles.erase(it);
      continue;
    }

    // Comportamiento Homing (Orbe Morado: Munición Teledirigida)
    if (it->es_homing && boss.estaVivo()) {
      CE::Vector2D bpos = boss.getPosicion();
      float tx = bpos.x - it->pos.x;
      float ty = bpos.y - it->pos.y;
      float tlen = std::sqrt(tx * tx + ty * ty);
      if (tlen > 1.f) {
        tx /= tlen;
        ty /= tlen;
        float turn = std::clamp(6.8f * dt, 0.f, 1.f);
        it->direccion.x += (tx - it->direccion.x) * turn;
        it->direccion.y += (ty - it->direccion.y) * turn;
        float dlen = std::sqrt(it->direccion.x * it->direccion.x +
                               it->direccion.y * it->direccion.y);
        if (dlen > 0.001f) {
          it->direccion.x /= dlen;
          it->direccion.y /= dlen;
          it->angulo = std::atan2(it->direccion.y, it->direccion.x);
        }
      }
    }

    it->pos.x += it->direccion.x * it->velocidad * dt;
    it->pos.y += it->direccion.y * it->velocidad * dt;

    // Si es Railgun (Orbe Morado), vaporiza balas enemigas naranjas a su paso!
    if (it->es_railgun) {
      for (auto bit = proyectiles_boss.begin(); bit != proyectiles_boss.end();) {
        float dx = it->pos.x - bit->pos.x;
        float dy = it->pos.y - bit->pos.y;
        float r = 24.f + bit->radio_hitbox;
        if (dx * dx + dy * dy <= r * r) {
          spawnParticulas(bit->pos, 4, sf::Color(120, 215, 255, 220), 40.f,
                          130.f, 3.f, 0.22f);
          bit = proyectiles_boss.erase(bit);
        } else {
          ++bit;
        }
      }
    }

    bool colision = false;

    // 1) Impacto contra un Orbe (4 disparos para reclamar a distancia, independencia de fuego del GDD!)
    for (auto it_orbe = orbes.begin(); it_orbe != orbes.end(); ++it_orbe) {
      float dx = it->pos.x - it_orbe->pos.x;
      float dy = it->pos.y - it_orbe->pos.y;
      if ((dx * dx + dy * dy) <= (30.f * 30.f)) {
        colision = true;
        it_orbe->disparos_restantes--;
        it_orbe->timer_flash = 0.10f;
        reproducirSFX("hit_orbe", 48.f,
                      1.0f + (4 - it_orbe->disparos_restantes) * 0.12f);
        spawnParticulas(it->pos, 6,
                        (it_orbe->tipo == TipoOrbe::PERMANENTE_VERDE)
                            ? sf::Color(80, 255, 145)
                            : sf::Color(220, 110, 255),
                        55.f, 165.f, 3.2f, 0.28f);
        if (it_orbe->disparos_restantes <= 0) {
          reclamarOrbe(*it_orbe);
          orbes.erase(it_orbe);
        }
        break;
      }
    }

    // 2) Impacto contra el Jefe AEGIS-01
    if (!colision && boss.estaVivo()) {
      CE::Vector2D bpos = boss.getPosicion();
      float dx = it->pos.x - bpos.x;
      float dy = it->pos.y - bpos.y;
      float r = boss.getRadioHitboxBala() + (it->es_railgun ? 14.f : 8.f);
      if (dx * dx + dy * dy <= r * r) {
        colision = true;
        disparos_acertados++;
        danio_total_infligido += it->danio;

        bool murio = boss.recibirDanio(it->danio);
        reproducirSFX("hit_jefe", it->es_critico ? 52.f : 36.f,
                      it->es_critico ? 1.25f : (0.94f + (std::rand() % 14) * 0.01f));

        spawnParticulas(it->pos, it->es_critico ? 10 : 5,
                        it->es_critico ? sf::Color(115, 255, 255)
                                       : sf::Color(40, 195, 255),
                        70.f, 220.f, it->es_critico ? 4.2f : 3.0f, 0.30f);

        spawnTextoFlotante(
            CE::Vector2D{it->pos.x + static_cast<float>((std::rand() % 24) - 12),
                         it->pos.y - 16.f},
            it->es_critico ? (std::to_string(it->danio) + "!")
                           : std::to_string(it->danio),
            it->es_critico ? sf::Color(115, 255, 255) : sf::Color(195, 238, 255),
            it->es_critico ? 19u : 15u, 0.55f);

        registrarImpactoEnJefe(it->danio, it->id_salva);

        if (murio) {
          fase_partida = FasePartida::VICTORIA;
          opcion_fin = 0;
          limpiarControles();
          proyectiles_boss.clear();
          spawnOnda(bpos, 30.f, 520.f, sf::Color(255, 145, 35, 245), 0.9f);
          spawnOnda(bpos, 20.f, 320.f, sf::Color(80, 235, 255, 235), 0.7f);
          spawnParticulas(bpos, 65, sf::Color(255, 130, 30, 255), 80.f, 440.f,
                          6.f, 1.0f);
          spawnParticulas(bpos, 35, sf::Color(255, 235, 140, 255), 60.f, 340.f,
                          4.5f, 0.85f);
        }
      }
    }

    // 3) Colisión contra el borde circular de la arena
    if (!colision) {
      float dx = it->pos.x - centro_arena.x;
      float dy = it->pos.y - centro_arena.y;
      if (dx * dx + dy * dy >= (radio_arena_libre + 26.f) * (radio_arena_libre + 26.f)) {
        colision = true;
        spawnParticulas(it->pos, 4, sf::Color(40, 185, 255, 195), 40.f, 120.f,
                        2.5f, 0.22f);
        // Fallar contra la pared rompe la ráfaga consecutiva SOLO si ningún tiro de esta salva acertó
        if (it->id_salva != ultima_salva_con_impacto) {
          impactos_consecutivos_jefe = 0;
        }
      }
    }

    if (colision) {
      it = proyectiles.erase(it);
    } else {
      ++it;
    }
  }

  // Actualizar Proyectiles Naranjas del Jefe (con Campo Crono, fragmentación y colisión contra jugador)
  if (fase_partida == FasePartida::COMBATE) {
    actualizarProyectilesBoss(dt);
  }
}

void Escena_Propia::moverJugadorConSliding(float dt) {
  if (!jugador_soldier)
    return;
  auto trans = jugador_soldier->getTransformada();
  auto ctrl = jugador_soldier->getComponente<CE::IControl>();
  if (!trans || !ctrl || !ctrl->isActivo())
    return;

  float vx = 0.f;
  float vy = 0.f;
  if (ctrl->arr)
    vy -= 1.f;
  if (ctrl->abj)
    vy += 1.f;
  if (ctrl->izq)
    vx -= 1.f;
  if (ctrl->der)
    vx += 1.f;

  // Normalizar movimiento diagonal cuando camina (los spins diagonales ya aplican INV_SQRT2 internamente)
  if (!estaRodando() && vx != 0.f && vy != 0.f) {
    constexpr float INV_SQRT2 = 0.70710678f;
    vx *= INV_SQRT2;
    vy *= INV_SQRT2;
  }

  float dx = vx * trans->velocidad.x * dt;
  float dy = vy * trans->velocidad.y * dt;

  // Movimiento con deslizamiento por ejes separados para nunca atorarse en tiles diagonales
  CE::Vector2D pos_orig = trans->posicion;

  // Eje X
  trans->posicion.x += dx;
  bool col_x = false;
  for (auto &obj : objetos.getPool()) {
    if (SistemaColAABBMid(*jugador_soldier, *obj, false)) {
      col_x = true;
      break;
    }
  }
  if (col_x) {
    trans->posicion.x = pos_orig.x;
  }

  // Eje Y
  float x_despues = trans->posicion.x;
  trans->posicion.y += dy;
  bool col_y = false;
  for (auto &obj : objetos.getPool()) {
    if (SistemaColAABBMid(*jugador_soldier, *obj, false)) {
      col_y = true;
      break;
    }
  }
  if (col_y) {
    trans->posicion.y = pos_orig.y;
  }
  trans->posicion.x = x_despues;

  // Clamp circular suave para garantizar permanencia limpia dentro de la arena circular
  float rx = trans->posicion.x - centro_arena.x;
  float ry = trans->posicion.y - centro_arena.y;
  float dist = std::sqrt(rx * rx + ry * ry);
  if (dist > radio_arena_libre && dist > 0.001f) {
    trans->posicion.x = centro_arena.x + (rx / dist) * radio_arena_libre;
    trans->posicion.y = centro_arena.y + (ry / dist) * radio_arena_libre;
  }

  // Sincronizar posición visual inmediata del sprite
  auto sp = jugador_soldier->getComponente<CE::ISprite>();
  if (sp) {
    sp->m_sprite.setPosition({trans->posicion.x, trans->posicion.y});
  }
}

void Escena_Propia::actualizarProyectilesBoss(float dt) {
  float dt_eff = dt * build.escalaVelocidadEnemiga();
  CE::Vector2D pos_jug = jugador_soldier->getTransformada()->posicion;
  bool invuln_dash = estaRodando();

  std::vector<ProyectilBoss> nuevos_fragmentos;

  for (auto it = proyectiles_boss.begin(); it != proyectiles_boss.end();) {
    it->edad += dt_eff;

    // Fricción para morteros y cápsulas hexagonales
    if (it->friccion > 0.f) {
      float damp = std::exp(-it->friccion * dt_eff);
      it->vel.x *= damp;
      it->vel.y *= damp;
    }

    it->pos.x += it->vel.x * dt_eff;
    it->pos.y += it->vel.y * dt_eff;

    if (it->tipo == TipoProyectilBoss::MORTERO_NARANJA ||
        it->tipo == TipoProyectilBoss::CAPSULA_JAULA) {
      it->angulo += 4.5f * dt_eff;
    }

    // Detonación programada de morteros y cápsulas hexagonales
    if (it->tiempo_detonacion > 0.f && it->edad >= it->tiempo_detonacion) {
      ProyectilBoss copia = *it;
      it = proyectiles_boss.erase(it);
      // Generar fragmentos en un vector temporal para no invalidar iteradores
      if (copia.tipo == TipoProyectilBoss::MORTERO_NARANJA) {
        int n = copia.sub_cantidad;
        for (int i = 0; i < n; ++i) {
          float ang = copia.angulo + (TWO_PI * i) / static_cast<float>(n);
          ProyectilBoss frag;
          frag.tipo = TipoProyectilBoss::ORBE_NARANJA;
          frag.pos = copia.pos;
          float sp = 250.f * reglas.mult_vel_balas_boss;
          frag.vel = CE::Vector2D{std::cos(ang) * sp, std::sin(ang) * sp};
          frag.angulo = ang;
          frag.radio_hitbox = 7.5f;
          frag.escala = 0.78f;
          frag.tiempo_vida = 5.5f;
          nuevos_fragmentos.push_back(frag);
        }
        spawnOnda(copia.pos, 10.f, 58.f, sf::Color(255, 130, 25, 220), 0.25f);
        spawnParticulas(copia.pos, 10, sf::Color(255, 155, 45), 60.f, 180.f,
                        3.5f, 0.30f);
        reproducirSFX("boss_disparo", 34.f, 1.25f);
      } else if (copia.tipo == TipoProyectilBoss::CAPSULA_JAULA) {
        float dx = pos_jug.x - copia.pos.x;
        float dy = pos_jug.y - copia.pos.y;
        float ang_j = std::atan2(dy, dx);
        // 3 lanzas apuntadas hacia el jugador
        for (int k = -1; k <= 1; ++k) {
          float ang = ang_j + k * 0.24f;
          ProyectilBoss frag;
          frag.tipo = TipoProyectilBoss::LANZA_NARANJA;
          frag.pos = copia.pos;
          float sp = 315.f * reglas.mult_vel_balas_boss;
          frag.vel = CE::Vector2D{std::cos(ang) * sp, std::sin(ang) * sp};
          frag.angulo = ang;
          frag.radio_hitbox = 7.5f;
          frag.escala = 0.76f;
          frag.tiempo_vida = 5.5f;
          nuevos_fragmentos.push_back(frag);
        }
        // 2 orbes tangenciales creando geometría cruzada
        for (int s : {-1, 1}) {
          float ang = ang_j + s * (PI * 0.5f);
          ProyectilBoss frag;
          frag.tipo = TipoProyectilBoss::ORBE_NARANJA;
          frag.pos = copia.pos;
          float sp = 215.f * reglas.mult_vel_balas_boss;
          frag.vel = CE::Vector2D{std::cos(ang) * sp, std::sin(ang) * sp};
          frag.angulo = ang;
          frag.radio_hitbox = 7.5f;
          frag.escala = 0.78f;
          frag.tiempo_vida = 5.5f;
          nuevos_fragmentos.push_back(frag);
        }
        spawnOnda(copia.pos, 12.f, 64.f, sf::Color(255, 150, 35, 225), 0.28f);
        spawnParticulas(copia.pos, 8, sf::Color(255, 145, 30), 50.f, 160.f,
                        3.2f, 0.28f);
      }
      continue;
    }

    // Expiración o salida de la arena circular
    // Nota GDD: los proyectiles hostiles son INTANGIBLES ante los orbes (no se bloquean!)
    float rx = it->pos.x - centro_arena.x;
    float ry = it->pos.y - centro_arena.y;
    if (it->edad >= it->tiempo_vida ||
        (rx * rx + ry * ry) >= (radio_arena_libre + 34.f) * (radio_arena_libre + 34.f)) {
      it = proyectiles_boss.erase(it);
      continue;
    }

    // Colisión justa estilo Bullet Hell contra el núcleo del Jugador (hitbox compacta 11px)
    // Si el jugador está haciendo Dash/Voltereta -> INMUNE (i-frames del GDD!)
    if (!invuln_dash) {
      float dx = it->pos.x - pos_jug.x;
      float dy = it->pos.y - (pos_jug.y + 4.f);
      float r = it->radio_hitbox + 11.f;
      if (dx * dx + dy * dy <= r * r) {
        spawnParticulas(it->pos, 8, sf::Color(255, 120, 30), 60.f, 180.f, 3.5f,
                        0.3f);
        it = proyectiles_boss.erase(it);
        daniarJugador();
        if (fase_partida != FasePartida::COMBATE)
          break;
        continue;
      }
    }

    ++it;
  }

  for (auto &nf : nuevos_fragmentos) {
    proyectiles_boss.push_back(nf);
  }
}

void Escena_Propia::daniarJugador(int cantidad, const std::string &etiqueta) {
  if (fase_partida != FasePartida::COMBATE)
    return;
  if (estaRodando() || timer_invuln_golpe > 0.f)
    return;

  CE::Vector2D pos_jug = jugador_soldier->getTransformada()->posicion;

  // Verificar primero si tiene Escudo de Fase (Orbe Morado)
  if (build.consumirCargaEscudo()) {
    timer_invuln_golpe = 0.65f;
    reproducirSFX("pickup_morado", 60.f, 1.35f);
    spawnOnda(pos_jug, 20.f, 210.f, sf::Color(215, 95, 255, 240), 0.42f);
    spawnParticulas(pos_jug, 24, sf::Color(220, 110, 255), 80.f, 280.f, 4.5f,
                    0.45f);
    spawnTextoFlotante(CE::Vector2D{pos_jug.x, pos_jug.y - 46.f},
                       "ESCUDO ABSORBIO!", sf::Color(230, 140, 255), 17, 0.9f);
    // Efecto "Blank" estilo Enter the Gungeon: despeja balas naranjas cercanas!
    for (auto it = proyectiles_boss.begin(); it != proyectiles_boss.end();) {
      float dx = it->pos.x - pos_jug.x;
      float dy = it->pos.y - pos_jug.y;
      if (dx * dx + dy * dy <= 215.f * 215.f) {
        spawnParticulas(it->pos, 3, sf::Color(215, 110, 255), 35.f, 110.f, 3.f,
                        0.22f);
        it = proyectiles_boss.erase(it);
      } else {
        ++it;
      }
    }
    return;
  }

  int dmg = std::max(1, cantidad);
  hp_jugador = std::max(0, hp_jugador - dmg);
  jugador_soldier->getStats()->hp = hp_jugador;
  timer_invuln_golpe = 1.05f;
  timer_flash_danio_hud = (dmg >= 2) ? 0.45f : 0.35f;
  // Recibir golpe interrumpe la racha consecutiva
  impactos_consecutivos_jefe = 0;

  reproducirSFX("hit_jugador", dmg >= 2 ? 82.f : 72.f, dmg >= 2 ? 0.85f : 1.0f);
  spawnParticulas(pos_jug, dmg >= 2 ? 26 : 18, sf::Color(255, 75, 75), 75.f,
                  250.f, dmg >= 2 ? 4.8f : 4.f, 0.42f);
  spawnOnda(pos_jug, 16.f, dmg >= 2 ? 120.f : 88.f, sf::Color(255, 70, 70, 220),
            0.32f);
  std::string txt = etiqueta.empty() ? ("-" + std::to_string(dmg) + " HP") : etiqueta;
  spawnTextoFlotante(CE::Vector2D{pos_jug.x, pos_jug.y - 44.f}, txt,
                     dmg >= 2 ? sf::Color(255, 125, 55) : sf::Color(255, 85, 85),
                     dmg >= 2 ? 20u : 18u, 0.95f);

  if (hp_jugador <= 0) {
    fase_partida = FasePartida::DERROTA;
    opcion_fin = 0;
    frame_muerte_jugador = 0;
    timer_muerte_jugador = 0.f;
    limpiarControles();
  }
}

void Escena_Propia::intentarDash() {
  if (en_pausa || fase_partida == FasePartida::VICTORIA ||
      fase_partida == FasePartida::DERROTA)
    return;
  if (estaRodando() || cooldown_dash_restante > 0.f)
    return;
  auto ctrl = jugador_soldier ? jugador_soldier->getComponente<CE::IControl>()
                              : nullptr;
  if (!ctrl)
    return;
  ctrl->sacc = true;
}

void Escena_Propia::onInputs(const CE::Botones &accion) {
  // Manejo de overlays de Victoria / Derrota
  if (fase_partida == FasePartida::VICTORIA ||
      fase_partida == FasePartida::DERROTA) {
    if (accion.getTipo() == CE::Botones::TipoAccion::OnPress) {
      if (accion.getNombre() == "arriba" || accion.getNombre() == "abajo") {
        opcion_fin = (opcion_fin == 0) ? 1 : 0;
      } else if (accion.getNombre() == "aceptar" ||
                 accion.getNombre() == "espacio") {
        if (opcion_fin == 0) {
          reiniciarPartida();
        } else {
          limpiarControles();
          CE::GestorEscenas::Get().cambiarEscena("Escena_Menu_Principal");
        }
      } else if (accion.getNombre() == "pausa") {
        limpiarControles();
        CE::GestorEscenas::Get().cambiarEscena("Escena_Menu_Principal");
      }
    }
    return;
  }

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
      } else if (accion.getNombre() == "aceptar" ||
                 accion.getNombre() == "espacio") {
        if (opcion_pausa == 0) {
          en_pausa = false;
          resincronizarControles();
        } else if (opcion_pausa == 1) {
          en_pausa = false;
          limpiarControles();
          CE::GestorEscenas::Get().cambiarEscena("Escena_Menu_Principal");
        } else if (opcion_pausa == 2) {
          std::exit(EXIT_SUCCESS);
        }
      }
    }
    return;
  }

  // Activar pausa con Escape
  if (accion.getTipo() == CE::Botones::TipoAccion::OnPress &&
      accion.getNombre() == "pausa") {
    en_pausa = true;
    opcion_pausa = 0;
    limpiarControles();
    return;
  }

  auto ctrl =
      jugador_soldier ? jugador_soldier->getComponente<CE::IControl>() : nullptr;
  if (!ctrl)
    return;

  switch (accion.getTipo()) {
  case CE::Botones::TipoAccion::OnPress: {
    if (accion.getNombre() == "arriba")
      ctrl->arr = true;
    if (accion.getNombre() == "abajo")
      ctrl->abj = true;
    if (accion.getNombre() == "derecha")
      ctrl->der = true;
    if (accion.getNombre() == "izquierda")
      ctrl->izq = true;

    // Dash / Voltereta inmune: tanto Espacio (GDD) como Click Derecho!
    if (accion.getNombre() == "accion_secundaria" ||
        accion.getNombre() == "espacio") {
      intentarDash();
    }

    if (accion.getNombre() == "disparar") {
      ctrl->acc = true;
      if (!estaRodando() && !recargando &&
          (balas_cargador > 0 || build.tieneMunicionInfinita()) &&
          tiempo_ultimo_disparo >= build.cadenciaDisparo()) {
        dispararBala();
      }
    }
    if (accion.getNombre() == "recargar") {
      iniciarRecarga();
    }
    break;
  }
  case CE::Botones::TipoAccion::OnRelease: {
    if (accion.getNombre() == "arriba")
      ctrl->arr = false;
    if (accion.getNombre() == "abajo")
      ctrl->abj = false;
    if (accion.getNombre() == "derecha")
      ctrl->der = false;
    if (accion.getNombre() == "izquierda")
      ctrl->izq = false;
    if (accion.getNombre() == "accion_secundaria" ||
        accion.getNombre() == "espacio")
      ctrl->sacc = false;
    if (accion.getNombre() == "disparar")
      ctrl->acc = false;
    break;
  }
  case CE::Botones::TipoAccion::None:
    break;
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
    ctrl->sacc = false;
    ctrl->acc = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
  }
}

void Escena_Propia::dispararBala() {
  if (en_pausa || !jugador_soldier || recargando)
    return;
  if (fase_partida != FasePartida::COMBATE && fase_partida != FasePartida::INTRO)
    return;
  if (estaRodando())
    return;
  if (balas_cargador <= 0 && !build.tieneMunicionInfinita())
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
    dir = CE::Vector2D{0.f, -1.f};
  } else {
    dir.x /= len;
    dir.y /= len;
  }

  float angulo_base = std::atan2(dir.y, dir.x);

  // Orientar sprite en reposo hacia la dirección de disparo (usando hojas Idle respirando)
  auto me = jugador_soldier->getComponente<IMaquinaEstado>();
  if (me && me->fsm && me->fsm->getNombre() == "IdleSoldier") {
    auto sprite = jugador_soldier->getComponente<CE::ISprite>();
    if (sprite) {
      auto &assets = CE::GestorAssets::Get();
      if (std::abs(dir.x) > std::abs(dir.y)) {
        sprite->m_sprite.setTexture(assets.getTextura(
            dir.x > 0.f ? "soldier_idle_right_sheet_K"
                        : "soldier_idle_left_sheet_K"));
      } else {
        sprite->m_sprite.setTexture(assets.getTextura(
            dir.y > 0.f ? "soldier_idle_down_sheet_K"
                        : "soldier_idle_up_sheet_K"));
      }
    }
  }

  CE::Vector2D spawnBase{playerPos.x + dir.x * 28.f, playerPos.y + dir.y * 28.f};
  muzzle_flash.pos = spawnBase;
  muzzle_flash.angulo = angulo_base;
  muzzle_flash.vida = 0.065f;

  int cant = build.cantidadProyectiles();
  int danio_base = build.danioPorBala();
  float vel_bala = build.velocidadProyectil();
  bool es_homing = build.tieneHoming();
  bool es_railgun = build.tieneRailgun();
  float prob_crit = build.probCritico();

  contador_salva++;
  float spread_paso = (cant <= 3) ? 0.11f : 0.09f;

  for (int i = 0; i < cant; ++i) {
    float offset_idx =
        static_cast<float>(i) - static_cast<float>(cant - 1) * 0.5f;
    float ang = angulo_base + offset_idx * spread_paso;
    CE::Vector2D d{std::cos(ang), std::sin(ang)};

    // Pequeño desplazamiento perpendicular para que múltiples proyectiles salgan en paralelo limpio
    float perpX = -dir.y * offset_idx * 7.f;
    float perpY = dir.x * offset_idx * 7.f;

    bool crit = ((std::rand() % 1000) / 1000.f) < prob_crit;
    int dmg = crit ? static_cast<int>(std::round(danio_base * 1.8f)) : danio_base;

    ProyectilBala p;
    p.pos = CE::Vector2D{spawnBase.x + perpX, spawnBase.y + perpY};
    p.direccion = d;
    p.velocidad = vel_bala;
    p.angulo = ang;
    p.tiempo_vida = 2.0f;
    p.tiempo_transcurrido = 0.f;
    p.danio = dmg;
    p.es_critico = crit;
    p.es_homing = es_homing;
    p.es_railgun = es_railgun;
    p.escala = es_railgun ? 1.15f : (crit ? 0.90f : 0.74f);
    p.id_salva = contador_salva;

    proyectiles.push_back(p);
    disparos_totales++;
  }

  if (!build.tieneMunicionInfinita()) {
    balas_cargador = std::max(0, balas_cargador - 1);
  }
  tiempo_ultimo_disparo = 0.f;

  reproducirSFX("disparo", 42.f,
                es_railgun ? 0.78f : (0.95f + (std::rand() % 14) * 0.01f));
}

bool Escena_Propia::iniciarRecarga() {
  if (recargando || en_pausa || estaRodando() || build.tieneMunicionInfinita())
    return false;
  if (fase_partida != FasePartida::COMBATE && fase_partida != FasePartida::INTRO)
    return false;

  int max_mag = build.tamCargadorMax();
  if (balas_cargador >= max_mag || balas_reserva <= 0)
    return false;

  recargando = true;
  tiempo_recarga_actual = 0.0f;
  reproducirSFX("recarga", 55.f, 1.0f);
  return true;
}

void Escena_Propia::recargarArma() {
  if (en_pausa)
    return;
  int max_mag = build.tamCargadorMax();
  if (balas_cargador >= max_mag || balas_reserva <= 0)
    return;

  if (balas_reserva >= max_mag) {
    balas_cargador = max_mag;
    balas_reserva -= max_mag;
  } else {
    balas_cargador = balas_reserva;
    balas_reserva = 0;
  }
}

bool Escena_Propia::estaRodando() const {
  if (!jugador_soldier)
    return false;
  auto me = jugador_soldier->getComponente<IMaquinaEstado>();
  if (!me || !me->fsm)
    return false;
  return (me->fsm->getNombre().find("Spin") != std::string::npos);
}

void Escena_Propia::registrarImpactoEnJefe(int danio_aplicado, int id_salva) {
  (void)danio_aplicado;
  ultima_salva_con_impacto = id_salva;
  // Cada 5 impactos consecutivos contra Aegis evalúa el spawn de orbes (30% perm / 10% temp)
  impactos_consecutivos_jefe++;
  if (impactos_consecutivos_jefe >= impactos_requeridos_drop) {
    impactos_consecutivos_jefe = 0;
    evaluarDropOrbe();
  }
}

void Escena_Propia::evaluarDropOrbe() {
  // Evitar saturar la arena con más de 4 orbes simultáneos
  if (orbes.size() >= 4)
    return;

  // Probabilidad pura: 30% Orbe Verde (permanente), 10% Orbe Morado (temporal)
  int roll = std::rand() % 100;
  if (roll < reglas.prob_permanente_pct) {
    generarOrbe(TipoOrbe::PERMANENTE_VERDE);
    return;
  }
  if (roll < reglas.prob_permanente_pct + reglas.prob_temporal_pct) {
    generarOrbe(TipoOrbe::TEMPORAL_MORADO);
    return;
  }
}

void Escena_Propia::generarOrbe(TipoOrbe tipo) {
  float spawnX = centro_arena.x;
  float spawnY = centro_arena.y + 220.f;

  // Seleccionar una celda libre dentro de la arena circular lejos del jugador y el jefe
  if (!tiles_industrial.empty() && !tiles_industrial[0].getCeldasLibres().empty()) {
    const auto &celdas = tiles_industrial[0].getCeldasLibres();
    for (int intento = 0; intento < 35; ++intento) {
      const auto &celda = celdas[std::rand() % celdas.size()];
      // Asegurar que está bien adentro del área cómoda (no pegado al muro exterior)
      float rcx = celda.x - centro_arena.x;
      float rcy = celda.y - centro_arena.y;
      if ((rcx * rcx + rcy * rcy) > (485.f * 485.f))
        continue;

      bool muy_cerca = false;
      if (jugador_soldier) {
        auto jpos = jugador_soldier->getTransformada()->posicion;
        float dx = celda.x - jpos.x;
        float dy = celda.y - jpos.y;
        if ((dx * dx + dy * dy) < (110.f * 110.f))
          muy_cerca = true;
      }
      if (!muy_cerca) {
        auto bpos = boss.getPosicion();
        float dx = celda.x - bpos.x;
        float dy = celda.y - bpos.y;
        if ((dx * dx + dy * dy) < (125.f * 125.f))
          muy_cerca = true;
      }
      for (const auto &otro : orbes) {
        float dx = celda.x - otro.pos.x;
        float dy = celda.y - otro.pos.y;
        if ((dx * dx + dy * dy) < (85.f * 85.f)) {
          muy_cerca = true;
          break;
        }
      }
      if (!muy_cerca) {
        spawnX = celda.x;
        spawnY = celda.y;
        break;
      }
    }
  }

  OrbeMejora nuevo;
  nuevo.pos = CE::Vector2D{spawnX, spawnY};
  nuevo.tipo = tipo;
  nuevo.disparos_restantes = 4;
  nuevo.tiempo_vida_max = 15.0f;
  nuevo.tiempo_transcurrido = 0.f;
  nuevo.rotacion = 0.f;
  nuevo.timer_flash = 0.25f;
  orbes.push_back(nuevo);

  sf::Color col = (tipo == TipoOrbe::PERMANENTE_VERDE)
                      ? sf::Color(55, 250, 125)
                      : sf::Color(205, 85, 255);
  spawnOnda(nuevo.pos, 10.f, 75.f, col, 0.45f);
  spawnParticulas(nuevo.pos, 14, col, 55.f, 175.f, 3.8f, 0.45f);
}

void Escena_Propia::reclamarOrbe(const OrbeMejora &orbe) {
  auto res = build.aplicarOrbeAleatorio(orbe.tipo, reglas.permite_curacion,
                                        hp_jugador, hp_max_jugador);

  if (res.recargo_reserva > 0) {
    balas_reserva += res.recargo_reserva;
  }
  // Sincronizar estadísticas visibles del jugador
  if (jugador_soldier) {
    jugador_soldier->getStats()->hp = hp_jugador;
    jugador_soldier->getStats()->hp_max = hp_max_jugador;
  }

  // Feedback visual y sonoro de alto impacto
  bool es_verde = (orbe.tipo == TipoOrbe::PERMANENTE_VERDE);
  sf::Color col = es_verde ? sf::Color(60, 255, 130) : sf::Color(215, 95, 255);

  spawnOnda(orbe.pos, 14.f, 115.f, col, 0.45f);
  spawnParticulas(orbe.pos, 22, col, 70.f, 240.f, 4.2f, 0.50f);

  CE::Vector2D jpos = jugador_soldier->getTransformada()->posicion;
  spawnOnda(jpos, 18.f, 92.f, col, 0.35f);
  spawnTextoFlotante(CE::Vector2D{jpos.x, jpos.y - 52.f},
                     "+" + res.info.nombre, col, 18, 1.35f);
}

void Escena_Propia::spawnParticulas(const CE::Vector2D &pos, int cant,
                                    const sf::Color &color, float vel_min,
                                    float vel_max, float radio, float vida) {
  for (int i = 0; i < cant; ++i) {
    if (particulas.size() >= 320)
      break;
    float ang = static_cast<float>(std::rand() % 3600) * (TWO_PI / 3600.f);
    float t = static_cast<float>(std::rand() % 1000) / 1000.f;
    float spd = vel_min + t * (vel_max - vel_min);
    ParticulaVFX p;
    p.pos = pos;
    p.vel = CE::Vector2D{std::cos(ang) * spd, std::sin(ang) * spd};
    p.radio = radio * (0.7f + 0.6f * (static_cast<float>(std::rand() % 100) / 100.f));
    p.vida = vida * (0.75f + 0.5f * (static_cast<float>(std::rand() % 100) / 100.f));
    p.vida_max = p.vida;
    p.color = color;
    particulas.push_back(p);
  }
}

void Escena_Propia::spawnOnda(const CE::Vector2D &pos, float r0, float r1,
                              const sf::Color &color, float vida) {
  OndaExpansivaVFX o;
  o.pos = pos;
  o.radio_inicial = r0;
  o.radio_final = r1;
  o.vida = vida;
  o.vida_max = vida;
  o.color = color;
  ondas.push_back(o);
}

void Escena_Propia::spawnTextoFlotante(const CE::Vector2D &pos,
                                       const std::string &txt,
                                       const sf::Color &color, unsigned int tam,
                                       float vida) {
  if (textos_flotantes.size() >= 36)
    textos_flotantes.erase(textos_flotantes.begin());
  TextoFlotanteVFX tf;
  tf.pos = pos;
  tf.texto = txt;
  tf.color = color;
  tf.tam = tam;
  tf.vida = vida;
  tf.vida_max = vida;
  textos_flotantes.push_back(tf);
}

void Escena_Propia::onRender() {
  auto &tex = CE::Render::Get().GetTextura();
  auto &assets = CE::GestorAssets::Get();

  // 0) Fondo industrial oscuro alrededor de la arena circular (cubre el clear azul del motor)
  sf::RectangleShape fondo_vacio{{3200.f, 3200.f}};
  fondo_vacio.setPosition({-800.f, -800.f});
  fondo_vacio.setFillColor(sf::Color(11, 14, 21, 255));
  CE::Render::Get().AddToDraw(fondo_vacio);

  // 1) Mapa circular de tiles (tilemap2.png)
  for (auto &al : tiles_industrial)
    CE::Render::Get().AddToDraw(al);

  // Anillo holográfico brutalista sutil demarcando el perímetro táctico circular
  sf::CircleShape anillo_arena{radio_arena_libre + 16.f, 72};
  anillo_arena.setOrigin({anillo_arena.getRadius(), anillo_arena.getRadius()});
  anillo_arena.setPosition({centro_arena.x, centro_arena.y});
  anillo_arena.setFillColor(sf::Color::Transparent);
  anillo_arena.setOutlineThickness(2.f);
  anillo_arena.setOutlineColor(sf::Color(255, 130, 30, 52));
  CE::Render::Get().AddToDraw(anillo_arena);

  // Efecto visual sutil de Campo Crono si está activo
  if (build.escalaVelocidadEnemiga() < 0.99f) {
    sf::CircleShape filtro_crono{radio_arena_libre + 20.f, 64};
    filtro_crono.setOrigin({filtro_crono.getRadius(), filtro_crono.getRadius()});
    filtro_crono.setPosition({centro_arena.x, centro_arena.y});
    filtro_crono.setFillColor(sf::Color(145, 70, 255, 22));
    filtro_crono.setOutlineThickness(3.f);
    filtro_crono.setOutlineColor(sf::Color(190, 110, 255, 110));
    CE::Render::Get().AddToDraw(filtro_crono);
  }

  // 2) Ondas expansivas en el suelo
  for (const auto &o : ondas) {
    float p = 1.0f - std::clamp(o.vida / std::max(0.001f, o.vida_max), 0.f, 1.f);
    float r = o.radio_inicial + (o.radio_final - o.radio_inicial) * p;
    sf::CircleShape c{r};
    c.setOrigin({r, r});
    c.setPosition({o.pos.x, o.pos.y});
    c.setFillColor(sf::Color::Transparent);
    c.setOutlineThickness(3.5f * (1.f - p * 0.65f));
    sf::Color col = o.color;
    col.a = static_cast<std::uint8_t>(static_cast<float>(o.color.a) * (1.f - p));
    c.setOutlineColor(col);
    CE::Render::Get().AddToDraw(c);
  }

  // 3) Telegraphs y sombra del Jefe AEGIS-01
  CE::Vector2D pos_jug = jugador_soldier->getTransformada()->posicion;
  boss.dibujarTelegraph(tex, pos_jug);

  // 4) Orbes de Mejora (Verde Permanente / Morado Temporal) + 4 cuadritos de vida
  for (const auto &orbe : orbes) {
    bool es_verde = (orbe.tipo == TipoOrbe::PERMANENTE_VERDE);
    float tiempo_restante = orbe.tiempo_vida_max - orbe.tiempo_transcurrido;
    std::uint8_t alpha = 255;
    if (tiempo_restante < 5.0f) {
      float urg = std::clamp((5.0f - tiempo_restante) / 5.0f, 0.f, 1.f);
      float frec = 8.0f + urg * 24.0f;
      float onda = 0.5f * (1.0f + std::sin(orbe.tiempo_transcurrido * frec));
      alpha = static_cast<std::uint8_t>(45.f + onda * 210.f);
    }

    float pulso = 1.0f + 0.08f * std::sin(orbe.tiempo_transcurrido * 6.f);

    // Halo exterior luminoso en suelo
    sf::CircleShape halo{28.f * pulso};
    halo.setOrigin({halo.getRadius(), halo.getRadius()});
    halo.setPosition({orbe.pos.x, orbe.pos.y});
    sf::Color colHalo = es_verde ? sf::Color(45, 245, 110, static_cast<std::uint8_t>(alpha * 0.28f))
                                 : sf::Color(200, 75, 255, static_cast<std::uint8_t>(alpha * 0.28f));
    halo.setFillColor(colHalo);
    halo.setOutlineThickness(1.8f);
    halo.setOutlineColor(es_verde ? sf::Color(70, 255, 135, static_cast<std::uint8_t>(alpha * 0.7f))
                                  : sf::Color(220, 110, 255, static_cast<std::uint8_t>(alpha * 0.7f)));
    CE::Render::Get().AddToDraw(halo);

    // Sprite animado (rotando suavemente) de orbe_verde_ai / orbe_morado_ai
    sf::Sprite sp_orbe{assets.getTextura(es_verde ? "orbe_verde_ai_K"
                                                  : "orbe_morado_ai_K")};
    sp_orbe.setOrigin({64.f, 64.f});
    sp_orbe.setPosition({orbe.pos.x, orbe.pos.y});
    float sc = 0.42f * pulso * (orbe.timer_flash > 0.f ? 1.18f : 1.0f);
    sp_orbe.setScale({sc, sc});
    sp_orbe.setRotation(sf::radians(orbe.rotacion));
    sp_orbe.setColor(orbe.timer_flash > 0.f ? sf::Color(255, 255, 255, 255)
                                            : sf::Color(255, 255, 255, alpha));
    CE::Render::Get().AddToDraw(sp_orbe);

    // 4 cuadritos sobre el orbe indicando los golpes restantes para romperlo a distancia
    const int max_golpes = 4;
    const float tam_c = 7.f;
    const float esp = 3.5f;
    const float ancho_tot = max_golpes * tam_c + (max_golpes - 1) * esp;
    float iniX = orbe.pos.x - ancho_tot * 0.5f;
    float posY = orbe.pos.y - 38.f;
    sf::Color colAct =
        es_verde ? sf::Color(55, 245, 115, alpha) : sf::Color(205, 80, 255, alpha);
    sf::Color colB =
        es_verde ? sf::Color(12, 65, 28, alpha) : sf::Color(52, 14, 80, alpha);

    for (int i = 0; i < max_golpes; ++i) {
      sf::RectangleShape cuad{{tam_c, tam_c}};
      cuad.setPosition({iniX + i * (tam_c + esp), posY});
      cuad.setOutlineThickness(1.2f);
      if (i < orbe.disparos_restantes) {
        cuad.setFillColor(colAct);
        cuad.setOutlineColor(colB);
      } else {
        cuad.setFillColor(sf::Color(16, 20, 30, 185));
        cuad.setOutlineColor(sf::Color(60, 70, 90, 190));
      }
      CE::Render::Get().AddToDraw(cuad);
    }
  }

  // 5) Jefe AEGIS-01
  boss.dibujar(tex);

  // 6) Estela (afterimages) del Dash del Jugador
  for (const auto &ai : afterimages) {
    if (!ai.textura)
      continue;
    float frac = std::clamp(ai.vida / std::max(0.001f, ai.vida_max), 0.f, 1.f);
    sf::Sprite ghost{*ai.textura};
    ghost.setTextureRect(ai.rect);
    ghost.setOrigin({32.f, 42.f});
    ghost.setPosition(ai.pos);
    ghost.setColor(sf::Color(70, 235, 255, static_cast<std::uint8_t>(frac * 135.f)));
    CE::Render::Get().AddToDraw(ghost);
  }

  // 7) Jugador (sombra + halo i-frames/escudo + sprite o animación de derrota)
  sf::CircleShape sombra_jug{20.f};
  sombra_jug.setOrigin({20.f, 20.f});
  sombra_jug.setScale({1.0f, 0.45f});
  sombra_jug.setPosition({pos_jug.x, pos_jug.y + 34.f});
  sombra_jug.setFillColor(sf::Color(6, 10, 16, 130));
  CE::Render::Get().AddToDraw(sombra_jug);

  if (fase_partida == FasePartida::DERROTA) {
    sf::Sprite sp_dead{assets.getTextura("soldier_death_sheet_K")};
    sp_dead.setTextureRect(sf::IntRect{{frame_muerte_jugador * 96, 0}, {96, 84}});
    sp_dead.setOrigin({48.f, 42.f});
    sp_dead.setPosition({pos_jug.x, pos_jug.y});
    CE::Render::Get().AddToDraw(sp_dead);
  } else {
    // Halo cyan de inmunidad durante el Dash
    if (estaRodando()) {
      sf::CircleShape halo_dash{34.f};
      halo_dash.setOrigin({34.f, 34.f});
      halo_dash.setPosition({pos_jug.x, pos_jug.y});
      halo_dash.setFillColor(sf::Color(40, 235, 255, 34));
      halo_dash.setOutlineThickness(2.2f);
      halo_dash.setOutlineColor(sf::Color(80, 245, 255, 195));
      CE::Render::Get().AddToDraw(halo_dash);
    }

    CE::Render::Get().AddToDraw(*jugador_soldier);

    // Escudo de Fase morado alrededor del soldado (Orbe Morado)
    if (build.tieneEscudoFase()) {
      float p = 0.5f + 0.5f * std::sin(tiempo_global * 8.f);
      sf::CircleShape shield{40.f + p * 3.f};
      shield.setOrigin({shield.getRadius(), shield.getRadius()});
      shield.setPosition({pos_jug.x, pos_jug.y});
      shield.setFillColor(sf::Color(195, 70, 255, 28));
      shield.setOutlineThickness(2.6f);
      shield.setOutlineColor(
          sf::Color(225, 120, 255, static_cast<std::uint8_t>(160 + p * 85)));
      CE::Render::Get().AddToDraw(shield);
    }
  }

  // Destello azul de disparo (muzzle flash)
  if (muzzle_flash.vida > 0.f) {
    float f = muzzle_flash.vida / 0.065f;
    sf::CircleShape flash{14.f * f};
    flash.setOrigin({flash.getRadius(), flash.getRadius()});
    flash.setPosition({muzzle_flash.pos.x, muzzle_flash.pos.y});
    flash.setFillColor(
        sf::Color(145, 245, 255, static_cast<std::uint8_t>(f * 230.f)));
    CE::Render::Get().AddToDraw(flash);
  }

  // 8) Proyectiles Azules del Jugador
  sf::Sprite sp_bala_jug{assets.getTextura("proyectil_neon_ai_K")};
  sp_bala_jug.setOrigin({32.f, 12.f});
  for (const auto &b : proyectiles) {
    sp_bala_jug.setPosition({b.pos.x, b.pos.y});
    sp_bala_jug.setRotation(sf::radians(b.angulo));
    sp_bala_jug.setScale({b.escala, b.escala});
    if (b.es_railgun) {
      sp_bala_jug.setColor(sf::Color(175, 215, 255, 255));
    } else if (b.es_critico) {
      sp_bala_jug.setColor(sf::Color(180, 255, 255, 255));
    } else if (b.es_homing) {
      sp_bala_jug.setColor(sf::Color(140, 210, 255, 255));
    } else {
      sp_bala_jug.setColor(sf::Color::White);
    }
    CE::Render::Get().AddToDraw(sp_bala_jug);
  }

  // 9) Proyectiles Naranjas del Jefe AEGIS-01 (alto contraste contra el azul del jugador)
  sf::Sprite sp_orbe_n{assets.getTextura("boss_proj_orbe_K")};
  sp_orbe_n.setOrigin({20.f, 20.f});
  sf::Sprite sp_lanza_n{assets.getTextura("boss_proj_lanza_K")};
  sp_lanza_n.setOrigin({36.f, 16.f});
  sf::Sprite sp_mort_n{assets.getTextura("boss_proj_mortero_K")};
  sp_mort_n.setOrigin({28.f, 28.f});

  for (const auto &pb : proyectiles_boss) {
    if (pb.tipo == TipoProyectilBoss::ORBE_NARANJA) {
      sp_orbe_n.setPosition({pb.pos.x, pb.pos.y});
      sp_orbe_n.setScale({pb.escala, pb.escala});
      CE::Render::Get().AddToDraw(sp_orbe_n);
    } else if (pb.tipo == TipoProyectilBoss::LANZA_NARANJA) {
      sp_lanza_n.setPosition({pb.pos.x, pb.pos.y});
      sp_lanza_n.setRotation(sf::radians(pb.angulo));
      sp_lanza_n.setScale({pb.escala, pb.escala});
      CE::Render::Get().AddToDraw(sp_lanza_n);
    } else {
      // MORTERO_NARANJA o CAPSULA_JAULA: pulso de aviso justo antes de detonar
      float aviso = (pb.tiempo_detonacion > 0.f)
                        ? std::clamp(pb.edad / pb.tiempo_detonacion, 0.f, 1.f)
                        : 0.f;
      if (aviso > 0.55f) {
        float r_av = 20.f + (1.f - aviso) * 42.f;
        sf::CircleShape ring{r_av};
        ring.setOrigin({r_av, r_av});
        ring.setPosition({pb.pos.x, pb.pos.y});
        ring.setFillColor(sf::Color::Transparent);
        ring.setOutlineThickness(2.f);
        ring.setOutlineColor(sf::Color(255, 160, 40,
                                       static_cast<std::uint8_t>(aviso * 235.f)));
        CE::Render::Get().AddToDraw(ring);
      }
      float sc = pb.escala * (1.0f + 0.16f * std::sin(tiempo_global * 18.f) * aviso);
      sp_mort_n.setPosition({pb.pos.x, pb.pos.y});
      sp_mort_n.setRotation(sf::radians(pb.angulo));
      sp_mort_n.setScale({sc, sc});
      CE::Render::Get().AddToDraw(sp_mort_n);
    }
  }

  // 10) Partículas VFX
  for (const auto &pt : particulas) {
    float frac = std::clamp(pt.vida / std::max(0.001f, pt.vida_max), 0.f, 1.f);
    sf::CircleShape c{pt.radio * frac};
    c.setOrigin({c.getRadius(), c.getRadius()});
    c.setPosition({pt.pos.x, pt.pos.y});
    sf::Color col = pt.color;
    col.a = static_cast<std::uint8_t>(static_cast<float>(pt.color.a) * frac);
    c.setFillColor(col);
    CE::Render::Get().AddToDraw(c);
  }

  // 11) Textos flotantes de daño / mejoras
  const auto &font = assets.getFont("default_font");
  for (const auto &tf : textos_flotantes) {
    float frac = std::clamp(tf.vida / std::max(0.001f, tf.vida_max), 0.f, 1.f);
    sf::Text t{font, toUtf8(tf.texto), tf.tam};
    t.setStyle(sf::Text::Style::Bold);
    sf::Color col = tf.color;
    col.a = static_cast<std::uint8_t>(255.f * frac);
    t.setFillColor(col);
    t.setOutlineThickness(1.5f);
    t.setOutlineColor(sf::Color(10, 14, 22, col.a));
    auto b = t.getLocalBounds();
    t.setPosition({tf.pos.x - b.size.x * 0.5f, tf.pos.y});
    CE::Render::Get().AddToDraw(t);
  }

  // 12) Flechas indicadoras para Orbes fuera de la vista de cámara
  if (!orbes.empty() && fase_partida == FasePartida::COMBATE) {
    auto &cam_activa = CE::GestorCamaras::Get().getCamaraActiva();
    sf::Vector2f camCenter = cam_activa.getView().getCenter();
    sf::Vector2f camSize = cam_activa.getView().getSize();
    float halfW = camSize.x * 0.5f;
    float halfH = camSize.y * 0.5f;

    for (const auto &orbe : orbes) {
      bool dentro = (std::abs(orbe.pos.x - camCenter.x) <= halfW) &&
                    (std::abs(orbe.pos.y - camCenter.y) <= halfH);
      if (!dentro) {
        float ang = std::atan2(orbe.pos.y - pos_jug.y, orbe.pos.x - pos_jug.x);
        float radio_f = 58.f;
        sf::ConvexShape flecha{4};
        flecha.setPoint(0, sf::Vector2f{12.f, 0.f});
        flecha.setPoint(1, sf::Vector2f{-7.f, -7.f});
        flecha.setPoint(2, sf::Vector2f{-3.f, 0.f});
        flecha.setPoint(3, sf::Vector2f{-7.f, 7.f});
        if (orbe.tipo == TipoOrbe::PERMANENTE_VERDE) {
          flecha.setFillColor(sf::Color(45, 240, 105, 240));
          flecha.setOutlineColor(sf::Color(10, 60, 25, 245));
        } else {
          flecha.setFillColor(sf::Color(200, 70, 255, 240));
          flecha.setOutlineColor(sf::Color(45, 10, 75, 245));
        }
        flecha.setOutlineThickness(1.5f);
        flecha.setPosition({pos_jug.x + std::cos(ang) * radio_f,
                            pos_jug.y + std::sin(ang) * radio_f});
        flecha.setRotation(sf::radians(ang));
        CE::Render::Get().AddToDraw(flecha);
      }
    }
  }

  // 13) Barra de progreso de recarga sobre el jugador
  if (recargando && fase_partida != FasePartida::DERROTA) {
    float ancho_b = 50.f;
    float alto_b = 6.f;
    float prog = std::clamp(
        tiempo_recarga_actual / std::max(0.01f, build.tiempoRecarga()), 0.f, 1.f);
    float bx = pos_jug.x - ancho_b * 0.5f;
    float by = pos_jug.y - 54.f;

    sf::RectangleShape fondo{{ancho_b, alto_b}};
    fondo.setPosition({bx, by});
    fondo.setFillColor(sf::Color(14, 18, 28, 220));
    fondo.setOutlineThickness(1.5f);
    fondo.setOutlineColor(sf::Color(45, 65, 95, 230));
    CE::Render::Get().AddToDraw(fondo);

    if (prog > 0.001f) {
      sf::RectangleShape rel{{ancho_b * prog, alto_b}};
      rel.setPosition({bx, by});
      rel.setFillColor(sf::Color(0, 230, 255, 240));
      CE::Render::Get().AddToDraw(rel);
    }
  }

  // 14) HUD en coordenadas fijas de pantalla + overlays
  renderHUD(tex);

  if (en_pausa) {
    renderOverlayPausa(tex);
  } else if (fase_partida == FasePartida::VICTORIA ||
             fase_partida == FasePartida::DERROTA) {
    renderOverlayFinPartida(tex);
  }
}

void Escena_Propia::renderHUD(sf::RenderTexture &tex) {
  sf::View prevView = tex.getView();
  tex.setView(tex.getDefaultView());

  float W = static_cast<float>(tex.getSize().x);
  float H = static_cast<float>(tex.getSize().y);
  const auto &font = CE::GestorAssets::Get().getFont("default_font");

  // Viñeta roja sutil al recibir daño
  if (timer_flash_danio_hud > 0.f) {
    float f = timer_flash_danio_hud / 0.35f;
    sf::RectangleShape flash{{W, H}};
    flash.setFillColor(sf::Color(255, 25, 35, static_cast<std::uint8_t>(f * 75.f)));
    tex.draw(flash);
  }

  // --- A) SUPERIOR IZQUIERDA: INTEGRIDAD DEL SOLDADO + PERMANENTES + TEMPORALES ---
  float leftX = 16.f;
  float leftY = 14.f;
  float hudLeftW = 248.f;
  float hudLeftH = 52.f;

  sf::RectangleShape panelJug{{hudLeftW, hudLeftH}};
  panelJug.setPosition({leftX, leftY});
  panelJug.setFillColor(sf::Color(13, 17, 26, 212));
  panelJug.setOutlineThickness(1.5f);
  panelJug.setOutlineColor(sf::Color(48, 66, 94, 215));
  tex.draw(panelJug);

  sf::RectangleShape lineaTopJug{{hudLeftW, 2.f}};
  lineaTopJug.setPosition({leftX, leftY});
  lineaTopJug.setFillColor(sf::Color(60, 235, 130, 225));
  tex.draw(lineaTopJug);

  sf::Text txtSoldado{font, "RECLUTA 001", 12};
  txtSoldado.setStyle(sf::Text::Style::Bold);
  txtSoldado.setFillColor(sf::Color(175, 200, 225, 235));
  txtSoldado.setPosition({leftX + 10.f, leftY + 7.f});
  tex.draw(txtSoldado);

  // Contador total de mejoras permanentes recogidas
  sf::Text txtPerm{font,
                   "MEJORAS: " + std::to_string(build.getTotalVerdes()), 11};
  txtPerm.setStyle(sf::Text::Style::Bold);
  txtPerm.setFillColor(sf::Color(95, 245, 150, 230));
  auto bP = txtPerm.getLocalBounds();
  txtPerm.setPosition({leftX + hudLeftW - bP.size.x - 10.f, leftY + 8.f});
  tex.draw(txtPerm);

  // Segmentos de vida (HP)
  float segW = std::min(38.f, (hudLeftW - 24.f - (hp_max_jugador - 1) * 5.f) /
                                   std::max(1, hp_max_jugador));
  for (int i = 0; i < hp_max_jugador; ++i) {
    sf::RectangleShape seg{{segW, 14.f}};
    seg.setPosition({leftX + 10.f + i * (segW + 5.f), leftY + 28.f});
    seg.setOutlineThickness(1.2f);
    if (i < hp_jugador) {
      seg.setFillColor(hp_jugador <= 2 ? sf::Color(255, 65, 75, 245)
                                       : sf::Color(55, 235, 135, 240));
      seg.setOutlineColor(sf::Color(20, 80, 45, 245));
    } else {
      seg.setFillColor(sf::Color(26, 30, 42, 200));
      seg.setOutlineColor(sf::Color(70, 78, 96, 200));
    }
    tex.draw(seg);
  }

  // Badges compactos de Mejoras Permanentes (Orbes Verdes) debajo del panel izquierdo
  float badgeY = leftY + hudLeftH + 8.f;
  float badgeX = leftX;
  for (int i = 0; i < static_cast<int>(MejoraPermanenteId::COUNT); ++i) {
    auto id = static_cast<MejoraPermanenteId>(i);
    int lvl = build.getNivelPermanente(id);
    if (lvl <= 0)
      continue;
    auto info = BuildJugador::getInfoPermanente(id);
    std::string label = info.codigo_corto + " x" + std::to_string(lvl);
    sf::Text tb{font, label, 11};
    tb.setStyle(sf::Text::Style::Bold);
    tb.setFillColor(sf::Color(115, 255, 160, 245));
    float bw = tb.getLocalBounds().size.x + 12.f;
    if (badgeX + bw > leftX + hudLeftW + 24.f) {
      badgeX = leftX;
      badgeY += 22.f;
    }
    sf::RectangleShape bgB{{bw, 18.f}};
    bgB.setPosition({badgeX, badgeY});
    bgB.setFillColor(sf::Color(16, 38, 26, 220));
    bgB.setOutlineThickness(1.f);
    bgB.setOutlineColor(sf::Color(55, 235, 120, 210));
    tex.draw(bgB);

    tb.setPosition({badgeX + 6.f, badgeY + 2.f});
    tex.draw(tb);
    badgeX += bw + 5.f;
  }

  // Barras de Buffs Temporales Activos (Orbes Morados)
  if (badgeX > leftX)
    badgeY += 24.f;
  for (const auto &buff : build.getBuffsTemporales()) {
    auto info = BuildJugador::getInfoTemporal(buff.id);
    float frac = std::clamp(buff.tiempo_restante / std::max(0.1f, buff.duracion_total), 0.f, 1.f);
    sf::RectangleShape bgT{{hudLeftW, 20.f}};
    bgT.setPosition({leftX, badgeY});
    bgT.setFillColor(sf::Color(28, 16, 44, 220));
    bgT.setOutlineThickness(1.2f);
    bgT.setOutlineColor(sf::Color(200, 80, 255, 225));
    tex.draw(bgT);

    sf::RectangleShape fillT{{(hudLeftW - 2.f) * frac, 18.f}};
    fillT.setPosition({leftX + 1.f, badgeY + 1.f});
    fillT.setFillColor(sf::Color(175, 55, 235, 125));
    tex.draw(fillT);

    std::ostringstream oss;
    oss << info.codigo_corto;
    if (buff.id == MejoraTemporalId::ESCUDO_DE_FASE) {
      oss << " x" << buff.cargas_extra;
    }
    oss << "  " << std::fixed << std::setprecision(1) << buff.tiempo_restante << "s";
    sf::Text tt{font, oss.str(), 12};
    tt.setStyle(sf::Text::Style::Bold);
    tt.setFillColor(sf::Color(242, 205, 255, 250));
    tt.setPosition({leftX + 8.f, badgeY + 2.f});
    tex.draw(tt);
    badgeY += 24.f;
  }

  // --- B) SUPERIOR CENTRO: BARRA DE VIDA DEL JEFE AEGIS-01 ---
  if (boss.estaVivo() || fase_partida == FasePartida::INTRO) {
    float barW = std::clamp(W - 320.f, 260.f, 480.f);
    float barH = 20.f;
    float barX = std::max(leftX + hudLeftW + 18.f, (W - barW) * 0.5f);
    if (barX + barW > W - 16.f)
      barX = W - barW - 16.f;
    float barY = 30.f;

    sf::Text txtBossName{font, "AEGIS-01 // CENTINELA DE TRITON", 13};
    txtBossName.setStyle(sf::Text::Style::Bold);
    txtBossName.setFillColor(boss.esFase2() ? sf::Color(255, 130, 45, 250)
                                            : sf::Color(255, 175, 85, 240));
    txtBossName.setPosition({barX, barY - 18.f});
    tex.draw(txtBossName);

    float ratio =
        std::clamp(static_cast<float>(boss.getHP()) /
                       static_cast<float>(std::max(1, boss.getHPMax())),
                   0.f, 1.f);

    sf::RectangleShape barBg{{barW, barH}};
    barBg.setPosition({barX, barY});
    barBg.setFillColor(sf::Color(14, 18, 28, 225));
    barBg.setOutlineThickness(2.f);
    barBg.setOutlineColor(boss.esFase2() ? sf::Color(255, 110, 25, 235)
                                         : sf::Color(125, 65, 30, 230));
    tex.draw(barBg);

    if (ratio > 0.f) {
      sf::RectangleShape barFill{{barW * ratio, barH}};
      barFill.setPosition({barX, barY});
      barFill.setFillColor(boss.esFase2() ? sf::Color(255, 95, 20, 245)
                                          : sf::Color(245, 130, 30, 240));
      tex.draw(barFill);
    }

    // Marca divisoria de Fase 2 en el 50%
    sf::RectangleShape marcaF2{{2.f, barH}};
    marcaF2.setPosition({barX + barW * 0.5f, barY});
    marcaF2.setFillColor(sf::Color(255, 240, 200, 185));
    tex.draw(marcaF2);

    std::string strHP =
        std::to_string(boss.getHP()) + " / " + std::to_string(boss.getHPMax());
    sf::Text txtHP{font, strHP, 13};
    txtHP.setStyle(sf::Text::Style::Bold);
    txtHP.setFillColor(sf::Color(255, 255, 255, 245));
    auto bHP = txtHP.getLocalBounds();
    txtHP.setPosition({barX + (barW - bHP.size.x) * 0.5f, barY + 1.f});
    tex.draw(txtHP);

    // Subtítulo del patrón actual del jefe
    sf::Text txtPat{font, boss.getNombreAtaqueActual(), 11};
    txtPat.setFillColor(sf::Color(225, 180, 130, 215));
    txtPat.setPosition({barX, barY + barH + 4.f});
    tex.draw(txtPat);
  }

  // Banner de INTRO al empezar la arena
  if (fase_partida == FasePartida::INTRO) {
    float f = std::sin(std::clamp(timer_intro / duracion_intro, 0.f, 1.f) * PI);
    std::uint8_t a = static_cast<std::uint8_t>(std::clamp(f * 255.f, 0.f, 255.f));
    sf::RectangleShape strip{{W, 82.f}};
    strip.setPosition({0.f, H * 0.42f});
    strip.setFillColor(sf::Color(10, 14, 22, static_cast<std::uint8_t>(a * 0.85f)));
    tex.draw(strip);

    sf::Text tIntro{font, "PISO 01 // AEGIS-01: CENTINELA DE TRITON", 30};
    tIntro.setStyle(sf::Text::Style::Bold);
    tIntro.setFillColor(sf::Color(255, 145, 40, a));
    auto bi = tIntro.getLocalBounds();
    tIntro.setPosition({(W - bi.size.x) * 0.5f, H * 0.42f + 12.f});
    tex.draw(tIntro);

    sf::Text tSub{font, "SOBREVIVE AL PROTOCOLO DE PRUEBA  |  RECOGE ORBES PARA EVOLUCIONAR", 14};
    tSub.setFillColor(sf::Color(180, 225, 255, a));
    auto bs = tSub.getLocalBounds();
    tSub.setPosition({(W - bs.size.x) * 0.5f, H * 0.42f + 52.f});
    tex.draw(tSub);
  }

  // --- D) INFERIOR DERECHA: PANEL DE ARMA Y MUNICIÓN ---
  float panelW = 324.f;
  float panelH = 78.f;
  float panelX = W - panelW - 18.f;
  float panelY = H - panelH - 16.f;

  sf::RectangleShape panelHUD{{panelW, panelH}};
  panelHUD.setPosition({panelX, panelY});
  panelHUD.setFillColor(sf::Color(14, 18, 28, 212));
  panelHUD.setOutlineThickness(1.5f);
  panelHUD.setOutlineColor(sf::Color(45, 65, 95, 215));
  tex.draw(panelHUD);

  sf::RectangleShape lineaNeon{{panelW, 2.f}};
  lineaNeon.setPosition({panelX, panelY});
  lineaNeon.setFillColor(build.tieneMunicionInfinita()
                             ? sf::Color(210, 85, 255, 235)
                             : sf::Color(0, 230, 255, 225));
  tex.draw(lineaNeon);

  if (sprite_arma_hud) {
    sprite_arma_hud->setPosition({panelX + 168.f, panelY + 12.f});
    tex.draw(*sprite_arma_hud);
  }

  std::string strCarg =
      build.tieneMunicionInfinita() ? "INF" : std::to_string(balas_cargador);
  sf::Text txtBalas{font, strCarg, 34};
  txtBalas.setStyle(sf::Text::Style::Bold);
  if (build.tieneMunicionInfinita()) {
    txtBalas.setFillColor(sf::Color(225, 125, 255, 255));
  } else if (balas_cargador > 6) {
    txtBalas.setFillColor(sf::Color::White);
  } else if (balas_cargador > 0) {
    txtBalas.setFillColor(sf::Color(255, 190, 40, 255));
  } else {
    txtBalas.setFillColor(sf::Color(255, 55, 65, 255));
  }
  txtBalas.setPosition({panelX + 16.f, panelY + 12.f});
  tex.draw(txtBalas);

  float wBalas = txtBalas.getLocalBounds().size.x;
  sf::Text txtReserva{font, " / " + std::to_string(balas_reserva), 20};
  txtReserva.setStyle(sf::Text::Style::Bold);
  txtReserva.setFillColor(sf::Color(135, 150, 175, 210));
  txtReserva.setPosition({panelX + 18.f + wBalas, panelY + 23.f});
  tex.draw(txtReserva);

  if (recargando) {
    sf::Text txtAl{font, "RECARGANDO...", 12};
    txtAl.setStyle(sf::Text::Style::Bold);
    txtAl.setFillColor(sf::Color(0, 230, 255, 240));
    txtAl.setPosition({panelX + 16.f, panelY + 55.f});
    tex.draw(txtAl);
  } else if (balas_cargador == 0 && !build.tieneMunicionInfinita()) {
    sf::Text txtAl{font, "[ R ] RECARGAR", 12};
    txtAl.setStyle(sf::Text::Style::Bold);
    txtAl.setFillColor(sf::Color(255, 70, 70, 240));
    txtAl.setPosition({panelX + 16.f, panelY + 55.f});
    tex.draw(txtAl);
  } else {
    std::string stat = "DMG " + std::to_string(build.danioPorBala()) + "  |  x" +
                       std::to_string(build.cantidadProyectiles()) + " PROY.";
    sf::Text txtEt{font, stat, 11};
    txtEt.setFillColor(sf::Color(125, 155, 185, 205));
    txtEt.setPosition({panelX + 16.f, panelY + 56.f});
    tex.draw(txtEt);
  }

  tex.setView(prevView);
}

void Escena_Propia::renderOverlayPausa(sf::RenderTexture &tex) {
  sf::View prevView = tex.getView();
  tex.setView(tex.getDefaultView());

  float screenW = static_cast<float>(tex.getSize().x);
  float screenH = static_cast<float>(tex.getSize().y);
  const auto &font = CE::GestorAssets::Get().getFont("default_font");

  sf::RectangleShape overlay{{screenW, screenH}};
  overlay.setFillColor(sf::Color(10, 10, 18, 215));
  tex.draw(overlay);

  float panelW = std::min(screenW * 0.82f, 580.f);
  float panelH = 420.f;
  float panelX = (screenW - panelW) * 0.5f;
  float panelY = (screenH - panelH) * 0.5f;

  sf::RectangleShape panel{{panelW, panelH}};
  panel.setPosition({panelX, panelY});
  panel.setFillColor(sf::Color(20, 24, 34, 245));
  panel.setOutlineThickness(2.5f);
  panel.setOutlineColor(sf::Color(80, 98, 138, 230));
  tex.draw(panel);

  sf::Text txtTitulo{font, "EXPENDABLE", 44};
  txtTitulo.setFillColor(sf::Color::White);
  txtTitulo.setStyle(sf::Text::Style::Bold);
  txtTitulo.setPosition({(screenW - txtTitulo.getLocalBounds().size.x) * 0.5f,
                         panelY + 22.f});
  tex.draw(txtTitulo);

  sf::Text txtSub{font, "- PAUSA -", 20};
  txtSub.setFillColor(sf::Color(0, 225, 255, 225));
  txtSub.setStyle(sf::Text::Style::Bold);
  txtSub.setPosition({(screenW - txtSub.getLocalBounds().size.x) * 0.5f,
                      panelY + 76.f});
  tex.draw(txtSub);

  sf::RectangleShape linea{{panelW - 80.f, 2.f}};
  linea.setPosition({panelX + 40.f, panelY + 110.f});
  linea.setFillColor(sf::Color(70, 86, 120, 180));
  tex.draw(linea);

  const std::string labels[3] = {"Reanudar", "Regresar al menu principal",
                                 "Regresar al escritorio"};
  const unsigned int sizes[3] = {42u, 28u, 28u};
  const float ys[3] = {panelY + 135.f, panelY + 215.f, panelY + 280.f};

  for (int i = 0; i < 3; ++i) {
    sf::Text t{font, labels[i], sizes[i]};
    t.setStyle(sf::Text::Style::Bold);
    t.setFillColor(opcion_pausa == i ? sf::Color::White
                                     : sf::Color(145, 152, 172, 185));
    float tx = (screenW - t.getLocalBounds().size.x) * 0.5f;
    t.setPosition({tx, ys[i]});
    tex.draw(t);

    if (opcion_pausa == i) {
      sf::Text cur{font, ">", sizes[i]};
      cur.setStyle(sf::Text::Style::Bold);
      cur.setFillColor(sf::Color(255, 215, 60, 255));
      cur.setPosition({tx - 34.f, ys[i]});
      tex.draw(cur);
    }
  }

  sf::Text txtAyuda{
      font, "[ W / S ] Seleccionar    [ Enter / Espacio ] Confirmar    [ Esc ] Reanudar",
      15};
  txtAyuda.setFillColor(sf::Color(135, 148, 175, 205));
  txtAyuda.setPosition({(screenW - txtAyuda.getLocalBounds().size.x) * 0.5f,
                        panelY + panelH - 38.f});
  tex.draw(txtAyuda);

  tex.setView(prevView);
}

void Escena_Propia::renderOverlayFinPartida(sf::RenderTexture &tex) {
  sf::View prevView = tex.getView();
  tex.setView(tex.getDefaultView());

  float W = static_cast<float>(tex.getSize().x);
  float H = static_cast<float>(tex.getSize().y);
  const auto &font = CE::GestorAssets::Get().getFont("default_font");
  bool victoria = (fase_partida == FasePartida::VICTORIA);

  sf::RectangleShape overlay{{W, H}};
  overlay.setFillColor(victoria ? sf::Color(8, 18, 22, 200)
                                : sf::Color(24, 8, 12, 205));
  tex.draw(overlay);

  float panelW = std::min(W * 0.82f, 580.f);
  float panelH = 420.f;
  float panelX = (W - panelW) * 0.5f;
  float panelY = (H - panelH) * 0.5f;

  sf::Color acento = victoria ? sf::Color(55, 245, 145) : sf::Color(255, 80, 75);

  sf::RectangleShape panel{{panelW, panelH}};
  panel.setPosition({panelX, panelY});
  panel.setFillColor(sf::Color(16, 20, 30, 245));
  panel.setOutlineThickness(2.5f);
  panel.setOutlineColor(acento);
  tex.draw(panel);

  sf::Text txtTit{font, victoria ? "PRUEBA SUPERADA" : "RECLUTA CAIDO", 44};
  txtTit.setStyle(sf::Text::Style::Bold);
  txtTit.setFillColor(acento);
  txtTit.setPosition({(W - txtTit.getLocalBounds().size.x) * 0.5f, panelY + 22.f});
  tex.draw(txtTit);

  sf::Text txtSub{
      font,
      victoria ? "AEGIS-01 NEUTRALIZADO  //  ACCESO AL PISO 02 CONCEDIDO"
               : "PROTOCOLO DE TRITON FALLIDO  //  RECLUTA EXPENDABLE",
      15};
  txtSub.setFillColor(sf::Color(200, 212, 232, 230));
  txtSub.setPosition({(W - txtSub.getLocalBounds().size.x) * 0.5f, panelY + 80.f});
  tex.draw(txtSub);

  // Estadísticas de la partida
  int mins = static_cast<int>(tiempo_combate) / 60;
  int secs = static_cast<int>(tiempo_combate) % 60;
  char bufTiempo[32];
  std::snprintf(bufTiempo, sizeof(bufTiempo), "%02d:%02d", mins, secs);
  int precision = (disparos_totales > 0)
                      ? (disparos_acertados * 100 / disparos_totales)
                      : 0;

  std::vector<std::string> stats = {
      "Tiempo: " + std::string(bufTiempo) +
          "    |    Precision: " + std::to_string(precision) + "%",
      "Daño infligido: " + std::to_string(danio_total_infligido),
      "Orbes Verdes (Perm.): " + std::to_string(build.getTotalVerdes()) +
          "    |    Orbes Morados (Temp.): " + std::to_string(build.getTotalMorados())};

  float sy = panelY + 124.f;
  for (const auto &s : stats) {
    sf::Text ts{font, toUtf8(s), 16};
    ts.setFillColor(sf::Color(220, 230, 245, 235));
    ts.setPosition({(W - ts.getLocalBounds().size.x) * 0.5f, sy});
    tex.draw(ts);
    sy += 28.f;
  }

  // Opciones: Reintentar / Menu principal
  const std::string opts[2] = {"Jugar otra partida", "Regresar al menu principal"};
  const float oys[2] = {panelY + 245.f, panelY + 305.f};
  for (int i = 0; i < 2; ++i) {
    sf::Text to{font, opts[i], i == 0 ? 32u : 26u};
    to.setStyle(sf::Text::Style::Bold);
    to.setFillColor(opcion_fin == i ? sf::Color::White
                                    : sf::Color(140, 150, 170, 185));
    float ox = (W - to.getLocalBounds().size.x) * 0.5f;
    to.setPosition({ox, oys[i]});
    tex.draw(to);

    if (opcion_fin == i) {
      sf::Text cur{font, ">", i == 0 ? 32u : 26u};
      cur.setStyle(sf::Text::Style::Bold);
      cur.setFillColor(sf::Color(255, 215, 65, 255));
      cur.setPosition({ox - 32.f, oys[i]});
      tex.draw(cur);
    }
  }

  sf::Text txtHelp{font, "[ W / S ] Seleccionar    [ Enter / Espacio ] Confirmar", 14};
  txtHelp.setFillColor(sf::Color(135, 148, 175, 200));
  txtHelp.setPosition({(W - txtHelp.getLocalBounds().size.x) * 0.5f,
                       panelY + panelH - 34.f});
  tex.draw(txtHelp);

  tex.setView(prevView);
}

} // namespace IVJ
