#include "BossAegis.hpp"
#include <Motor/Primitivos/GestorAssets.hpp>
#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace IVJ {

namespace {
constexpr float PI = 3.14159265358979323846f;
constexpr float TWO_PI = 6.28318530717958647692f;
constexpr int BOSS_FRAME_W = 180;
constexpr int BOSS_FRAME_H = 180;
constexpr int BOSS_FRAMES = 4;
constexpr float BOSS_SPRITE_SCALE = 0.96f;
} // namespace

void BossAegis::cargarTexturas() {
  if (texturas_cargadas)
    return;

  auto &assets = CE::GestorAssets::Get();
  assets.agregarTextura("boss_aegis_idle_K",
                        ASSETS "/sprites/bosses/aegis01/boss_idle_sheet.png",
                        CE::Vector2D{0.f, 0.f}, CE::Vector2D{720.f, 180.f});
  assets.agregarTextura("boss_aegis_walk_K",
                        ASSETS "/sprites/bosses/aegis01/boss_walk_sheet.png",
                        CE::Vector2D{0.f, 0.f}, CE::Vector2D{720.f, 180.f});
  assets.agregarTextura("boss_aegis_attack_K",
                        ASSETS "/sprites/bosses/aegis01/boss_attack_sheet.png",
                        CE::Vector2D{0.f, 0.f}, CE::Vector2D{720.f, 180.f});
  assets.agregarTextura("boss_aegis_enrage_K",
                        ASSETS "/sprites/bosses/aegis01/boss_enrage_sheet.png",
                        CE::Vector2D{0.f, 0.f}, CE::Vector2D{720.f, 180.f});

  assets.agregarTextura(
      "boss_proj_orbe_K",
      ASSETS "/sprites/bosses/aegis01/proyectil_boss_orbe_naranja.png",
      CE::Vector2D{0.f, 0.f}, CE::Vector2D{40.f, 40.f});
  assets.agregarTextura(
      "boss_proj_lanza_K",
      ASSETS "/sprites/bosses/aegis01/proyectil_boss_lanza_naranja.png",
      CE::Vector2D{0.f, 0.f}, CE::Vector2D{72.f, 32.f});
  assets.agregarTextura(
      "boss_proj_mortero_K",
      ASSETS "/sprites/bosses/aegis01/proyectil_boss_mortero_naranja.png",
      CE::Vector2D{0.f, 0.f}, CE::Vector2D{56.f, 56.f});

  texturas_cargadas = true;
}

void BossAegis::inicializar(const CE::Vector2D &centro, int hp_max_inicial,
                            float mult_vel) {
  cargarTexturas();

  centro_arena = centro;
  pos = centro;
  destino_patrulla = centro;
  hp_max = hp_max_inicial;
  hp = hp_max_inicial;
  fase2 = false;
  mult_vel_balas = mult_vel;

  estado = EstadoBoss::INTRO;
  bolsa_ataques.clear();
  timer_estado = 0.f;
  duracion_estado = 1.2f;
  timer_disparo = 0.f;
  timer_secundario = 0.f;
  angulo_patron = 0.f;
  angulo_fijado = PI * 0.5f;
  cooldown_giro_cercano = 0.f;
  rotacion_sprite = 0.f;
  paso_ataque = 0;

  frame_actual = 0;
  timer_anim = 0.f;
  timer_flash_hit = 0.f;
  tiempo_global = 0.f;
  alpha_derrota = 255.f;
  textura_actual.clear();

  sprite.emplace(CE::GestorAssets::Get().getTextura("boss_aegis_idle_K"));
  sprite->setOrigin({BOSS_FRAME_W * 0.5f, BOSS_FRAME_H * 0.5f});
  sprite->setScale({BOSS_SPRITE_SCALE, BOSS_SPRITE_SCALE});
  sprite->setPosition({pos.x, pos.y});
  aplicarTexturaEstado("boss_aegis_idle_K");
}

void BossAegis::activarCombate() {
  if (estado == EstadoBoss::INTRO) {
    estado = EstadoBoss::PATRULLA;
    timer_estado = 0.f;
    duracion_estado = 0.75f;
    destino_patrulla = centro_arena;
  }
}

void BossAegis::aplicarTexturaEstado(const std::string &clave_tex) {
  if (!sprite || textura_actual == clave_tex)
    return;
  textura_actual = clave_tex;
  sprite->setTexture(CE::GestorAssets::Get().getTextura(clave_tex), false);
  frame_actual = 0;
  timer_anim = 0.f;
  sprite->setTextureRect(
      sf::IntRect{{0, 0}, {BOSS_FRAME_W, BOSS_FRAME_H}});
}

AtaqueBossId BossAegis::elegirSiguienteAtaque() {
  if (bolsa_ataques.empty()) {
    for (int i = 0; i < static_cast<int>(AtaqueBossId::COUNT_DISTANCIA); ++i) {
      bolsa_ataques.push_back(static_cast<AtaqueBossId>(i));
    }
    // Fisher-Yates shuffle
    for (int i = static_cast<int>(bolsa_ataques.size()) - 1; i > 0; --i) {
      int j = std::rand() % (i + 1);
      std::swap(bolsa_ataques[i], bolsa_ataques[j]);
    }
    // Evitar repetir inmediatamente el último ataque al recargar la bolsa
    if (bolsa_ataques.back() == ataque_actual && bolsa_ataques.size() > 1) {
      std::swap(bolsa_ataques.back(), bolsa_ataques.front());
    }
  }
  AtaqueBossId elegido = bolsa_ataques.back();
  bolsa_ataques.pop_back();
  return elegido;
}

void BossAegis::iniciarAtaque(AtaqueBossId atk, const CE::Vector2D &pos_jugador) {
  ataque_actual = atk;
  estado = EstadoBoss::TELEGRAPH;
  timer_estado = 0.f;
  timer_disparo = 0.f;
  timer_secundario = 0.f;
  paso_ataque = 0;

  float dx = pos_jugador.x - pos.x;
  float dy = pos_jugador.y - pos.y;
  angulo_fijado = std::atan2(dy, dx);
  angulo_patron = angulo_fijado;

  float factor_f2 = fase2 ? 0.85f : 1.0f;
  switch (atk) {
  case AtaqueBossId::ESPIRAL_REACTOR:
    duracion_estado = 0.65f * factor_f2;
    break;
  case AtaqueBossId::ESCOPETA_TACTICA:
    duracion_estado = 0.60f * factor_f2;
    break;
  case AtaqueBossId::ANILLOS_CHOQUE:
    duracion_estado = 0.72f * factor_f2;
    break;
  case AtaqueBossId::CRUZ_Y_MORTERO:
    duracion_estado = 0.70f * factor_f2;
    break;
  case AtaqueBossId::JAULA_HEXAGONAL:
    duracion_estado = 0.65f * factor_f2;
    break;
  case AtaqueBossId::ATAQUE_GIRATORIO:
    duracion_estado = 0.55f * factor_f2;
    break;
  default:
    duracion_estado = 0.65f;
    break;
  }
}

bool BossAegis::recibirDanio(int danio) {
  if (!estaVivo())
    return false;

  hp = std::max(0, hp - danio);
  timer_flash_hit = 0.085f;

  if (hp <= 0) {
    estado = EstadoBoss::DERROTADO;
    timer_estado = 0.f;
    return true;
  }
  return false;
}

void BossAegis::spawnOrbe(std::vector<ProyectilBoss> &out,
                          const CE::Vector2D &origen, float ang, float speed,
                          float escala) const {
  ProyectilBoss b;
  b.tipo = TipoProyectilBoss::ORBE_NARANJA;
  b.pos = origen;
  float v = speed * mult_vel_balas;
  b.vel = CE::Vector2D{std::cos(ang) * v, std::sin(ang) * v};
  b.angulo = ang;
  b.radio_hitbox = 8.5f * escala;
  b.escala = escala;
  b.tiempo_vida = 7.0f;
  out.push_back(b);
}

void BossAegis::spawnLanza(std::vector<ProyectilBoss> &out,
                           const CE::Vector2D &origen, float ang, float speed,
                           float escala) const {
  ProyectilBoss b;
  b.tipo = TipoProyectilBoss::LANZA_NARANJA;
  b.pos = origen;
  float v = speed * mult_vel_balas;
  b.vel = CE::Vector2D{std::cos(ang) * v, std::sin(ang) * v};
  b.angulo = ang;
  b.radio_hitbox = 8.0f * escala;
  b.escala = escala;
  b.tiempo_vida = 6.5f;
  out.push_back(b);
}

void BossAegis::spawnMortero(std::vector<ProyectilBoss> &out,
                             const CE::Vector2D &origen, float ang, float speed,
                             float det_time, int fragmentos) const {
  ProyectilBoss b;
  b.tipo = TipoProyectilBoss::MORTERO_NARANJA;
  b.pos = origen;
  float v = speed * mult_vel_balas;
  b.vel = CE::Vector2D{std::cos(ang) * v, std::sin(ang) * v};
  b.angulo = ang;
  b.radio_hitbox = 15.f;
  b.escala = 0.92f;
  b.tiempo_vida = det_time + 0.5f;
  b.tiempo_detonacion = det_time;
  b.friccion = 1.65f;
  b.sub_cantidad = fragmentos;
  b.detonar_hacia_jugador = false;
  out.push_back(b);
}

void BossAegis::spawnCapsula(std::vector<ProyectilBoss> &out,
                             const CE::Vector2D &origen, float ang, float speed,
                             float det_time) const {
  ProyectilBoss b;
  b.tipo = TipoProyectilBoss::CAPSULA_JAULA;
  b.pos = origen;
  float v = speed * mult_vel_balas;
  b.vel = CE::Vector2D{std::cos(ang) * v, std::sin(ang) * v};
  b.angulo = ang;
  b.radio_hitbox = 14.f;
  b.escala = 0.88f;
  b.tiempo_vida = det_time + 0.5f;
  b.tiempo_detonacion = det_time;
  b.friccion = 2.8f;
  b.sub_cantidad = 3;
  b.detonar_hacia_jugador = true;
  out.push_back(b);
}

EventosFrameBoss BossAegis::actualizar(float dt,
                                       const CE::Vector2D &pos_jugador,
                                       std::vector<ProyectilBoss> &balas_out,
                                       float escala_tiempo_enemigo) {
  EventosFrameBoss ev;
  tiempo_global += dt;

  if (timer_flash_hit > 0.f) {
    timer_flash_hit = std::max(0.f, timer_flash_hit - dt);
  }
  if (cooldown_giro_cercano > 0.f) {
    cooldown_giro_cercano = std::max(0.f, cooldown_giro_cercano - dt);
  }

  // Checar transición automática a Fase 2 al bajar del 50% HP
  if (!fase2 && hp > 0 && hp <= hp_max / 2 && estado != EstadoBoss::INTRO &&
      estado != EstadoBoss::DERROTADO) {
    fase2 = true;
    estado = EstadoBoss::TRANSICION_FASE2;
    timer_estado = 0.f;
    duracion_estado = 1.15f;
    ev.sfx_fase2 = true;
    ev.limpiar_balas_por_fase2 = true;
  }

  float dt_combate = dt * std::clamp(escala_tiempo_enemigo, 0.45f, 1.0f);

  switch (estado) {
  case EstadoBoss::INTRO: {
    timer_estado += dt;
    aplicarTexturaEstado("boss_aegis_idle_K");
    break;
  }

  case EstadoBoss::TRANSICION_FASE2: {
    timer_estado += dt;
    aplicarTexturaEstado("boss_aegis_enrage_K");
    // Pequeño retroceso hacia el centro durante sobrecarga
    float lerp_t = std::clamp(dt * 3.0f, 0.f, 1.f);
    pos.x += (centro_arena.x - pos.x) * lerp_t;
    pos.y += (centro_arena.y - pos.y) * lerp_t;
    if (timer_estado >= duracion_estado) {
      // Lanza inmediatamente una salva de choque para anunciar Fase 2
      iniciarAtaque(AtaqueBossId::ANILLOS_CHOQUE, pos_jugador);
      ev.sfx_telegraph = true;
    }
    break;
  }

  case EstadoBoss::PATRULLA: {
    timer_estado += dt_combate;
    CE::Vector2D diff{destino_patrulla.x - pos.x, destino_patrulla.y - pos.y};
    float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
    float vel_move = fase2 ? 155.f : 115.f;
    if (dist > 8.f) {
      pos.x += (diff.x / dist) * vel_move * dt_combate;
      pos.y += (diff.y / dist) * vel_move * dt_combate;
      aplicarTexturaEstado(fase2 ? "boss_aegis_enrage_K" : "boss_aegis_walk_K");
    } else {
      aplicarTexturaEstado(fase2 ? "boss_aegis_enrage_K" : "boss_aegis_idle_K");
    }

    // Reacción inmediata a corta distancia: si el jugador está pegado a Aegis, lanza Ataque Giratorio!
    float dx_j = pos_jugador.x - pos.x;
    float dy_j = pos_jugador.y - pos.y;
    float dist_j = std::sqrt(dx_j * dx_j + dy_j * dy_j);
    if (dist_j <= 165.f && cooldown_giro_cercano <= 0.f) {
      iniciarAtaque(AtaqueBossId::ATAQUE_GIRATORIO, pos_jugador);
      ev.sfx_telegraph = true;
      break;
    }

    if (timer_estado >= duracion_estado) {
      AtaqueBossId sig = elegirSiguienteAtaque();
      iniciarAtaque(sig, pos_jugador);
      ev.sfx_telegraph = true;
    }
    break;
  }

  case EstadoBoss::TELEGRAPH: {
    timer_estado += dt_combate;
    aplicarTexturaEstado("boss_aegis_attack_K");

    // En Escopeta Táctica, seguir al jugador con la mira láser hasta el último instante
    if (ataque_actual == AtaqueBossId::ESCOPETA_TACTICA) {
      float dx = pos_jugador.x - pos.x;
      float dy = pos_jugador.y - pos.y;
      angulo_fijado = std::atan2(dy, dx);
    }

    if (timer_estado >= duracion_estado) {
      estado = EstadoBoss::ATACANDO;
      timer_estado = 0.f;
      timer_disparo = 0.f;
      timer_secundario = 0.f;
      paso_ataque = 0;

      // Definir duración de cada ataque
      switch (ataque_actual) {
      case AtaqueBossId::ESPIRAL_REACTOR:
        duracion_estado = fase2 ? 3.8f : 3.4f;
        break;
      case AtaqueBossId::ESCOPETA_TACTICA:
        duracion_estado = fase2 ? 2.5f : 2.15f;
        break;
      case AtaqueBossId::ANILLOS_CHOQUE:
        duracion_estado = fase2 ? 2.8f : 2.35f;
        break;
      case AtaqueBossId::CRUZ_Y_MORTERO:
        duracion_estado = fase2 ? 4.0f : 3.55f;
        break;
      case AtaqueBossId::JAULA_HEXAGONAL:
        duracion_estado = fase2 ? 2.5f : 1.55f;
        break;
      case AtaqueBossId::ATAQUE_GIRATORIO:
        duracion_estado = fase2 ? 1.05f : 0.92f;
        break;
      default:
        duracion_estado = 2.5f;
        break;
      }
    }
    break;
  }

  case EstadoBoss::ATACANDO: {
    timer_estado += dt_combate;
    aplicarTexturaEstado(fase2 ? "boss_aegis_enrage_K" : "boss_aegis_attack_K");
    ejecutarAtaque(dt_combate, pos_jugador, balas_out, ev);

    if (timer_estado >= duracion_estado) {
      if (ataque_actual == AtaqueBossId::ATAQUE_GIRATORIO) {
        cooldown_giro_cercano = 3.5f;
      }
      estado = EstadoBoss::PATRULLA;
      timer_estado = 0.f;
      duracion_estado = fase2 ? 0.90f : 1.25f;

      // Si el jugador sigue pegado al terminar un ataque a distancia, castigar rápido con el giro
      float dx_j = pos_jugador.x - pos.x;
      float dy_j = pos_jugador.y - pos.y;
      float dist_j = std::sqrt(dx_j * dx_j + dy_j * dy_j);
      if (ataque_actual != AtaqueBossId::ATAQUE_GIRATORIO && dist_j <= 165.f &&
          cooldown_giro_cercano <= 0.f) {
        iniciarAtaque(AtaqueBossId::ATAQUE_GIRATORIO, pos_jugador);
        ev.sfx_telegraph = true;
        break;
      }

      // Elegir nuevo punto táctico cerca del centro de la arena
      float ang_rand = static_cast<float>(std::rand() % 360) * (PI / 180.f);
      float rad_rand = 75.f + static_cast<float>(std::rand() % 115);
      destino_patrulla = CE::Vector2D{
          centro_arena.x + std::cos(ang_rand) * rad_rand,
          centro_arena.y + std::sin(ang_rand) * rad_rand};
    }
    break;
  }

  case EstadoBoss::DERROTADO: {
    timer_estado += dt;
    aplicarTexturaEstado("boss_aegis_enrage_K");
    if (paso_ataque == 0) {
      paso_ataque = 1;
      ev.explosion_derrota = true;
    }
    alpha_derrota = std::max(0.f, 255.f * (1.f - timer_estado / 1.4f));
    break;
  }
  }

  actualizarAnimacion(dt);
  return ev;
}

void BossAegis::ejecutarAtaque(float dt, const CE::Vector2D &pos_jugador,
                               std::vector<ProyectilBoss> &balas_out,
                               EventosFrameBoss &ev) {
  timer_disparo -= dt;
  timer_secundario -= dt;

  // Origen visual de los cañones / reactor
  CE::Vector2D core_pos{pos.x, pos.y - 6.f};
  CE::Vector2D canon_izq{pos.x - 56.f, pos.y + 18.f};
  CE::Vector2D canon_der{pos.x + 56.f, pos.y + 18.f};

  switch (ataque_actual) {
  case AtaqueBossId::ESPIRAL_REACTOR: {
    // 1) Espiral Doble del Reactor:
    // 4 brazos (5 en F2) rotando en espiral; cambia dirección a la mitad para sorprender
    float dir_rot = (timer_estado < duracion_estado * 0.5f) ? 1.0f : -1.0f;
    float vel_rot = (fase2 ? 1.95f : 1.55f) * dir_rot;
    angulo_patron += vel_rot * dt;

    float intervalo = fase2 ? 0.125f : 0.155f;
    if (timer_disparo <= 0.f) {
      timer_disparo = intervalo;
      int brazos = fase2 ? 5 : 4;
      float speed = fase2 ? 275.f : 235.f;
      for (int i = 0; i < brazos; ++i) {
        float ang = angulo_patron + (TWO_PI * i) / static_cast<float>(brazos);
        spawnOrbe(balas_out, core_pos, ang, speed, 0.85f);
      }
      // En Fase 2 agregar pulso contra-rotatorio más lento cada 3 disparos
      if (fase2 && (paso_ataque % 3 == 0)) {
        for (int i = 0; i < 4; ++i) {
          float ang = -angulo_patron * 0.7f + (TWO_PI * i) / 4.f;
          spawnOrbe(balas_out, core_pos, ang, 175.f, 0.75f);
        }
      }
      paso_ataque++;
      if (paso_ataque % 2 == 1)
        ev.sfx_disparo = true;
    }
    break;
  }

  case AtaqueBossId::ESCOPETA_TACTICA: {
    // 2) Ráfaga de Escopeta Táctica:
    // 3 oleadas (4 en F2) apuntadas al jugador formando flechas en V de lanzas naranjas
    int max_rafagas = fase2 ? 4 : 3;
    float intervalo = fase2 ? 0.55f : 0.65f;
    if (timer_disparo <= 0.f && paso_ataque < max_rafagas) {
      timer_disparo = intervalo;
      float dx = pos_jugador.x - pos.x;
      float dy = pos_jugador.y - pos.y;
      float ang_base = std::atan2(dy, dx);
      angulo_fijado = ang_base;

      int cant = 7 + (paso_ataque % 3); // 7 -> 8 -> 9 -> 7
      if (fase2)
        cant += 2;
      float spread_total = 1.18f; // ~67 grados
      CE::Vector2D origen = (paso_ataque % 2 == 0) ? canon_izq : canon_der;
      // Combinar ambos cañones para disparo simétrico + núcleo
      for (int i = 0; i < cant; ++i) {
        float t = (cant > 1) ? (static_cast<float>(i) / (cant - 1) - 0.5f) : 0.f;
        float ang = ang_base + t * spread_total;
        // Perfil de velocidad en cuña/flecha estilo Enter the Gungeon
        float factor_centro = 1.0f - std::abs(t) * 0.42f;
        float speed = (fase2 ? 410.f : 355.f) * factor_centro;
        CE::Vector2D spawn_p = (i < cant / 2) ? canon_izq : canon_der;
        if (cant % 2 == 1 && i == cant / 2)
          spawn_p = core_pos;
        (void)origen;
        spawnLanza(balas_out, spawn_p, ang, speed, 0.86f);
      }
      paso_ataque++;
      ev.sfx_disparo = true;
    }
    break;
  }

  case AtaqueBossId::ANILLOS_CHOQUE: {
    // 3) Anillos de Choque Concéntricos:
    // Oleadas densas de 26-30 orbes con 2 brechas rotatorias + velocidades alternas
    // Diseñados para esquivar con Dash (i-frames) o leer las brechas!
    int max_anillos = fase2 ? 4 : 3;
    float intervalo = fase2 ? 0.62f : 0.72f;
    if (timer_disparo <= 0.f && paso_ataque < max_anillos) {
      timer_disparo = intervalo;
      int total_ring = fase2 ? 30 : 26;
      float offset_ang = angulo_patron + paso_ataque * 0.55f;
      float speed = (paso_ataque % 2 == 0) ? (fase2 ? 275.f : 235.f)
                                           : (fase2 ? 335.f : 290.f);
      // 2 brechas simétricas de 2 proyectiles cada una
      int gap1 = (paso_ataque * 5) % total_ring;
      int gap2 = (gap1 + total_ring / 2) % total_ring;

      for (int i = 0; i < total_ring; ++i) {
        bool en_brecha = (i == gap1 || i == (gap1 + 1) % total_ring ||
                          i == gap2 || i == (gap2 + 1) % total_ring);
        if (en_brecha)
          continue;
        float ang = offset_ang + (TWO_PI * i) / static_cast<float>(total_ring);
        spawnOrbe(balas_out, core_pos, ang, speed, 0.88f);
      }

      // En el último anillo añadir un mini-anillo interior rápido de lanzas
      if (paso_ataque == max_anillos - 1) {
        int lanzas = fase2 ? 12 : 8;
        for (int i = 0; i < lanzas; ++i) {
          float ang = angulo_fijado + (TWO_PI * i) / static_cast<float>(lanzas);
          spawnLanza(balas_out, core_pos, ang, fase2 ? 360.f : 310.f, 0.80f);
        }
      }

      paso_ataque++;
      ev.sfx_disparo = true;
    }
    break;
  }

  case AtaqueBossId::CRUZ_Y_MORTERO: {
    // 4) Cruz Barrida + Morteros de Fragmentación:
    // 4 aspas giratorias de lanzas naranjas + 3 morteros pesados que explotan en estrellas de 8 orbes!
    float vel_cruz = fase2 ? 0.62f : 0.48f;
    angulo_patron += vel_cruz * dt;

    float intervalo_cruz = fase2 ? 0.145f : 0.175f;
    if (timer_disparo <= 0.f) {
      timer_disparo = intervalo_cruz;
      for (int i = 0; i < 4; ++i) {
        float ang = angulo_patron + (PI * 0.5f) * static_cast<float>(i);
        spawnLanza(balas_out, core_pos, ang, fase2 ? 300.f : 255.f, 0.80f);
      }
    }

    float intervalo_mortero = fase2 ? 0.95f : 1.15f;
    if (timer_secundario <= 0.f) {
      timer_secundario = intervalo_mortero;
      float dx = pos_jugador.x - pos.x;
      float dy = pos_jugador.y - pos.y;
      float ang_j = std::atan2(dy, dx);
      float dist_j = std::sqrt(dx * dx + dy * dy);
      // Velocidad calibrada para frenar cerca de la distancia del jugador
      float speed_m = std::clamp(dist_j * 1.15f, 240.f, 440.f);
      CE::Vector2D origen = (paso_ataque % 2 == 0) ? canon_izq : canon_der;
      spawnMortero(balas_out, origen, ang_j, speed_m, 0.92f, fase2 ? 10 : 8);
      paso_ataque++;
      ev.sfx_disparo = true;
    }
    break;
  }

  case AtaqueBossId::JAULA_HEXAGONAL: {
    // 5) Jaula Hexagonal / Fuego Cruzado:
    // Despliega 6 cápsulas en hexágono que se detienen y abren fuego cruzado hacia el jugador
    int max_rondas = fase2 ? 2 : 1;
    float intervalo = 1.15f;
    if (timer_disparo <= 0.f && paso_ataque < max_rondas) {
      timer_disparo = intervalo;
      float offset = angulo_fijado + (paso_ataque * PI / 6.f);
      int nodos = 6;
      for (int i = 0; i < nodos; ++i) {
        float ang = offset + (TWO_PI * i) / static_cast<float>(nodos);
        spawnCapsula(balas_out, core_pos, ang, 540.f, 0.88f);
      }
      // Acompañar con un abanico central ligero para evitar que el jugador se quede quieto
      float dx = pos_jugador.x - pos.x;
      float dy = pos_jugador.y - pos.y;
      float ang_j = std::atan2(dy, dx);
      for (int k = -2; k <= 2; ++k) {
        spawnOrbe(balas_out, core_pos, ang_j + k * 0.20f, 240.f, 0.80f);
      }
      paso_ataque++;
      ev.sfx_disparo = true;
    }
    break;
  }

  case AtaqueBossId::ATAQUE_GIRATORIO: {
    // 6) Ataque Giratorio a corta distancia (-2 HP al impacto):
    // Gira 360° rápidamente con aspas de plasma naranja y suelta un anillo tangencial de chispas
    float vel_giro = fase2 ? 21.5f : 18.5f;
    rotacion_sprite += vel_giro * dt;
    angulo_patron += vel_giro * dt;

    // Ligero desplazamiento hacia el jugador (manteniéndose dentro del radio táctico)
    float dx = pos_jugador.x - pos.x;
    float dy = pos_jugador.y - pos.y;
    float dist = std::sqrt(dx * dx + dy * dy);
    if (dist > 30.f) {
      float paso_vel = fase2 ? 85.f : 65.f;
      pos.x += (dx / dist) * paso_vel * dt;
      pos.y += (dy / dist) * paso_vel * dt;
      // Limitar para no salirse de la zona central-media
      float cx = pos.x - centro_arena.x;
      float cy = pos.y - centro_arena.y;
      float cdist = std::sqrt(cx * cx + cy * cy);
      if (cdist > 360.f && cdist > 0.001f) {
        pos.x = centro_arena.x + (cx / cdist) * 360.f;
        pos.y = centro_arena.y + (cy / cdist) * 360.f;
      }
    }

    // Suelta proyectiles tangenciales breves durante el giro
    if (timer_disparo <= 0.f) {
      timer_disparo = 0.22f;
      for (int i = 0; i < 4; ++i) {
        float ang = angulo_patron + i * (PI * 0.5f);
        CE::Vector2D borde{pos.x + std::cos(ang) * 95.f,
                           pos.y + std::sin(ang) * 95.f};
        spawnOrbe(balas_out, borde, ang + 0.35f, fase2 ? 260.f : 220.f, 0.72f);
      }
      ev.sfx_disparo = true;
    }
    break;
  }

  default:
    break;
  }
}

void BossAegis::actualizarAnimacion(float dt) {
  if (!sprite)
    return;

  duracion_frame = (estado == EstadoBoss::ATACANDO || estado == EstadoBoss::TRANSICION_FASE2)
                       ? 0.095f
                       : (estado == EstadoBoss::PATRULLA ? 0.135f : 0.16f);

  timer_anim += dt;
  if (timer_anim >= duracion_frame) {
    timer_anim -= duracion_frame;
    if (estado == EstadoBoss::TELEGRAPH) {
      // Alternar entre frames de carga 0 y 1
      frame_actual = (frame_actual == 0) ? 1 : 0;
    } else if (estado == EstadoBoss::ATACANDO && !fase2) {
      // Alternar entre frames de disparo 2 y 3
      frame_actual = (frame_actual == 2) ? 3 : 2;
    } else {
      frame_actual = (frame_actual + 1) % BOSS_FRAMES;
    }
  }

  sprite->setTextureRect(
      sf::IntRect{{frame_actual * BOSS_FRAME_W, 0}, {BOSS_FRAME_W, BOSS_FRAME_H}});

  // Leve vibración durante carga/disparo/transición para dar peso mecánico
  float jitterX = 0.f;
  float jitterY = 0.f;
  if (estado == EstadoBoss::TRANSICION_FASE2 || estado == EstadoBoss::DERROTADO) {
    jitterX = std::sin(tiempo_global * 68.f) * 4.5f;
    jitterY = std::cos(tiempo_global * 54.f) * 3.5f;
  } else if (estado == EstadoBoss::TELEGRAPH) {
    jitterX = std::sin(tiempo_global * 45.f) * 1.8f;
  }

  sprite->setPosition({pos.x + jitterX, pos.y + jitterY});

  // Rotar sprite durante el Ataque Giratorio y regresar suave a 0° al terminar
  if (!(estado == EstadoBoss::ATACANDO &&
        ataque_actual == AtaqueBossId::ATAQUE_GIRATORIO)) {
    if (std::abs(rotacion_sprite) > 0.001f) {
      // Normalizar ángulo a [-PI, PI] antes de volver suave a 0
      rotacion_sprite = std::fmod(rotacion_sprite, TWO_PI);
      if (rotacion_sprite > PI)
        rotacion_sprite -= TWO_PI;
      if (rotacion_sprite < -PI)
        rotacion_sprite += TWO_PI;
      rotacion_sprite *= std::exp(-14.f * dt);
      if (std::abs(rotacion_sprite) < 0.01f)
        rotacion_sprite = 0.f;
    }
  }
  sprite->setRotation(sf::radians(rotacion_sprite));

  // Tinte por daño (flash blanco/ámbar) o fade de derrota
  if (estado == EstadoBoss::DERROTADO) {
    std::uint8_t a = static_cast<std::uint8_t>(std::clamp(alpha_derrota, 0.f, 255.f));
    sprite->setColor(sf::Color(255, 165, 110, a));
  } else if (timer_flash_hit > 0.f) {
    sprite->setColor(sf::Color(255, 215, 185, 255));
  } else {
    sprite->setColor(sf::Color::White);
  }
}

void BossAegis::dibujarTelegraph(sf::RenderTarget &target,
                                 const CE::Vector2D &pos_jugador) const {
  if (!estaVivo())
    return;

  // Sombra táctil debajo del mech para anclarlo visualmente al suelo de la arena
  sf::CircleShape sombra{62.f};
  sombra.setOrigin({62.f, 62.f});
  sombra.setScale({1.0f, 0.45f});
  sombra.setPosition({pos.x, pos.y + 62.f});
  sombra.setFillColor(sf::Color(5, 8, 14, 135));
  target.draw(sombra);

  // Anillo base de energía en el suelo (más intenso en Fase 2)
  float pulso = 0.5f + 0.5f * std::sin(tiempo_global * (fase2 ? 9.f : 4.f));
  sf::CircleShape aura_suelo{74.f + pulso * 6.f};
  aura_suelo.setOrigin({aura_suelo.getRadius(), aura_suelo.getRadius()});
  aura_suelo.setScale({1.0f, 0.58f});
  aura_suelo.setPosition({pos.x, pos.y + 42.f});
  aura_suelo.setFillColor(sf::Color::Transparent);
  aura_suelo.setOutlineThickness(fase2 ? 2.5f : 1.5f);
  aura_suelo.setOutlineColor(
      fase2 ? sf::Color(255, 105, 20, static_cast<std::uint8_t>(110 + pulso * 95))
            : sf::Color(255, 135, 35, static_cast<std::uint8_t>(45 + pulso * 55)));
  target.draw(aura_suelo);

  // Avisos visuales específicos durante TELEGRAPH y TRANSICION_FASE2
  if (estado == EstadoBoss::TRANSICION_FASE2) {
    float prog = std::clamp(timer_estado / std::max(0.01f, duracion_estado), 0.f, 1.f);
    sf::CircleShape onda{60.f + prog * 340.f};
    onda.setOrigin({onda.getRadius(), onda.getRadius()});
    onda.setPosition({pos.x, pos.y});
    onda.setFillColor(sf::Color(255, 95, 15, static_cast<std::uint8_t>((1.f - prog) * 70.f)));
    onda.setOutlineThickness(4.f);
    onda.setOutlineColor(sf::Color(255, 145, 30, static_cast<std::uint8_t>((1.f - prog) * 240.f)));
    target.draw(onda);
    return;
  }

  if (estado == EstadoBoss::ATACANDO &&
      ataque_actual == AtaqueBossId::ATAQUE_GIRATORIO) {
    // Efecto visual activo del Ataque Giratorio (-2 HP): zona roja-naranja + 4 cuchillas de plasma
    float rad = getRadioGiroCercano();
    sf::CircleShape zona{rad};
    zona.setOrigin({rad, rad});
    zona.setPosition({pos.x, pos.y});
    zona.setFillColor(sf::Color(255, 65, 20, 48));
    zona.setOutlineThickness(4.f);
    zona.setOutlineColor(sf::Color(255, 130, 30, 235));
    target.draw(zona);

    sf::CircleShape anillo_int{rad * 0.72f};
    anillo_int.setOrigin({anillo_int.getRadius(), anillo_int.getRadius()});
    anillo_int.setPosition({pos.x, pos.y});
    anillo_int.setFillColor(sf::Color::Transparent);
    anillo_int.setOutlineThickness(2.5f);
    anillo_int.setOutlineColor(sf::Color(255, 205, 85, 195));
    target.draw(anillo_int);

    // 4 hojas/aspas giratorias de energía naranja-blanca
    for (int i = 0; i < 4; ++i) {
      float a = angulo_patron + i * (PI * 0.5f);
      sf::ConvexShape aspa{4};
      aspa.setPoint(0, sf::Vector2f{42.f, -10.f});
      aspa.setPoint(1, sf::Vector2f{rad, -4.f});
      aspa.setPoint(2, sf::Vector2f{rad - 18.f, 16.f});
      aspa.setPoint(3, sf::Vector2f{42.f, 8.f});
      aspa.setPosition({pos.x, pos.y});
      aspa.setRotation(sf::radians(a));
      aspa.setFillColor(sf::Color(255, 125, 25, 215));
      aspa.setOutlineThickness(2.f);
      aspa.setOutlineColor(sf::Color(255, 235, 160, 240));
      target.draw(aspa);
    }
    return;
  }

  if (estado != EstadoBoss::TELEGRAPH)
    return;

  float prog = std::clamp(timer_estado / std::max(0.01f, duracion_estado), 0.f, 1.f);
  std::uint8_t alpha = static_cast<std::uint8_t>(85.f + prog * 155.f);

  // Anillo de carga contrayéndose hacia el reactor
  float r_carga = 115.f * (1.0f - prog * 0.65f);
  sf::CircleShape anillo{r_carga};
  anillo.setOrigin({r_carga, r_carga});
  anillo.setPosition({pos.x, pos.y - 4.f});
  anillo.setFillColor(sf::Color::Transparent);
  anillo.setOutlineThickness(3.f);
  anillo.setOutlineColor(sf::Color(255, 130, 25, alpha));
  target.draw(anillo);

  // Guías específicas según el patrón que viene
  if (ataque_actual == AtaqueBossId::ESCOPETA_TACTICA) {
    float dx = pos_jugador.x - pos.x;
    float dy = pos_jugador.y - pos.y;
    float ang = std::atan2(dy, dx);
    for (int k = -1; k <= 1; ++k) {
      float a = ang + k * 0.52f;
      sf::RectangleShape linea{{460.f, k == 0 ? 2.5f : 1.5f}};
      linea.setOrigin({0.f, linea.getSize().y * 0.5f});
      linea.setPosition({pos.x, pos.y - 6.f});
      linea.setRotation(sf::radians(a));
      linea.setFillColor(sf::Color(255, 120, 20, static_cast<std::uint8_t>(alpha * 0.65f)));
      target.draw(linea);
    }
  } else if (ataque_actual == AtaqueBossId::CRUZ_Y_MORTERO) {
    for (int i = 0; i < 4; ++i) {
      float a = angulo_patron + i * (PI * 0.5f);
      sf::RectangleShape haz{{520.f, 3.0f}};
      haz.setOrigin({0.f, 1.5f});
      haz.setPosition({pos.x, pos.y - 6.f});
      haz.setRotation(sf::radians(a));
      haz.setFillColor(sf::Color(255, 110, 20, static_cast<std::uint8_t>(alpha * 0.60f)));
      target.draw(haz);
    }
  } else if (ataque_actual == AtaqueBossId::JAULA_HEXAGONAL) {
    sf::CircleShape hex{105.f + prog * 25.f, 6};
    hex.setOrigin({hex.getRadius(), hex.getRadius()});
    hex.setPosition({pos.x, pos.y});
    hex.setRotation(sf::radians(angulo_fijado));
    hex.setFillColor(sf::Color::Transparent);
    hex.setOutlineThickness(2.5f);
    hex.setOutlineColor(sf::Color(255, 140, 30, alpha));
    target.draw(hex);
  } else if (ataque_actual == AtaqueBossId::ANILLOS_CHOQUE) {
    sf::CircleShape ring2{65.f + prog * 110.f};
    ring2.setOrigin({ring2.getRadius(), ring2.getRadius()});
    ring2.setPosition({pos.x, pos.y});
    ring2.setFillColor(sf::Color::Transparent);
    ring2.setOutlineThickness(2.2f);
    ring2.setOutlineColor(sf::Color(255, 155, 45, static_cast<std::uint8_t>((1.f - prog) * 220.f)));
    target.draw(ring2);
  } else if (ataque_actual == AtaqueBossId::ATAQUE_GIRATORIO) {
    // Advertencia clara de alcance del Ataque Giratorio (-2 HP) antes de girar
    float rad = getRadioGiroCercano();
    sf::CircleShape zona_aviso{rad};
    zona_aviso.setOrigin({rad, rad});
    zona_aviso.setPosition({pos.x, pos.y});
    zona_aviso.setFillColor(
        sf::Color(255, 55, 25, static_cast<std::uint8_t>(22.f + prog * 58.f)));
    zona_aviso.setOutlineThickness(3.f);
    zona_aviso.setOutlineColor(sf::Color(255, 95, 25, alpha));
    target.draw(zona_aviso);

    // Onda interna expandiéndose hasta el borde del área de peligro
    sf::CircleShape pulso_rad{35.f + prog * (rad - 35.f)};
    pulso_rad.setOrigin({pulso_rad.getRadius(), pulso_rad.getRadius()});
    pulso_rad.setPosition({pos.x, pos.y});
    pulso_rad.setFillColor(sf::Color::Transparent);
    pulso_rad.setOutlineThickness(2.5f);
    pulso_rad.setOutlineColor(sf::Color(255, 195, 75, alpha));
    target.draw(pulso_rad);
  }
}

void BossAegis::dibujar(sf::RenderTarget &target) const {
  if (!sprite)
    return;
  if (estado == EstadoBoss::DERROTADO && alpha_derrota <= 1.f)
    return;
  target.draw(*sprite);
}

std::string BossAegis::getNombreAtaqueActual() const {
  if (estado == EstadoBoss::INTRO)
    return "SISTEMAS ACTIVANDOSE...";
  if (estado == EstadoBoss::TRANSICION_FASE2)
    return "COMBATE ACTIVO";
  if (estado == EstadoBoss::DERROTADO)
    return "UNIDAD NEUTRALIZADA";
  if (estado == EstadoBoss::PATRULLA)
    return "REPOSICIONANDO";

  switch (ataque_actual) {
  case AtaqueBossId::ESPIRAL_REACTOR:
    return "PATRON 01 // ESPIRAL DEL REACTOR";
  case AtaqueBossId::ESCOPETA_TACTICA:
    return "PATRON 02 // RAFAGA DE ESCOPETA";
  case AtaqueBossId::ANILLOS_CHOQUE:
    return "PATRON 03 // ANILLOS DE CHOQUE";
  case AtaqueBossId::CRUZ_Y_MORTERO:
    return "PATRON 04 // CRUZ BARRIDA Y MORTEROS";
  case AtaqueBossId::JAULA_HEXAGONAL:
    return "PATRON 05 // JAULA HEXAGONAL";
  case AtaqueBossId::ATAQUE_GIRATORIO:
    return "CONTRAATAQUE // TORBELLINO CERCANO";
  default:
    return "COMBATE ACTIVO";
  }
}

std::string BossAegis::getSubtituloEstado() const {
  return fase2 ? "FASE II - SOBRECARGA" : "FASE I - PROTOCOLO CENTINELA";
}

} // namespace IVJ
