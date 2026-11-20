#pragma once
#include <Juego/objetos/Entidad.hpp>
#include <Motor/Utils/Vector2D.hpp>
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>

namespace IVJ {

enum class TipoProyectilBoss {
  ORBE_NARANJA,    // Esfera plasma naranja (espirales, anillos, fragmentos)
  LANZA_NARANJA,   // Lanza direccional naranja (escopeta, cruz, fuego cruzado)
  MORTERO_NARANJA, // Cápsula pesada que detona en estrella de 8 orbes
  CAPSULA_JAULA    // Cápsula hexagonal que se detiene y dispara fuego cruzado
};

struct ProyectilBoss {
  TipoProyectilBoss tipo{TipoProyectilBoss::ORBE_NARANJA};
  CE::Vector2D pos{0.f, 0.f};
  CE::Vector2D vel{0.f, 0.f};
  float angulo{0.f};
  float radio_hitbox{9.f};
  float escala{1.f};
  float tiempo_vida{6.0f};
  float edad{0.f};

  // Comportamiento especial para MORTERO_NARANJA y CAPSULA_JAULA
  float tiempo_detonacion{0.f}; // >0 indica que se fragmenta al llegar a esta edad
  float friccion{0.f};          // Desaceleración exponencial para cápsulas
  int sub_cantidad{8};          // Proyectiles al detonar
  bool detonar_hacia_jugador{false};
};

enum class AtaqueBossId {
  ESPIRAL_REACTOR = 0, // 1. Doble espiral contra-rotatoria
  ESCOPETA_TACTICA = 1,// 2. 3 ráfagas apuntadas en cuña
  ANILLOS_CHOQUE = 2,  // 3. Anillos concéntricos con brechas (dodge-roll)
  CRUZ_Y_MORTERO = 3,  // 4. Cruz giratoria + morteros fragmentarios
  JAULA_HEXAGONAL = 4, // 5. 6 cápsulas hexagonales en fuego cruzado
  ATAQUE_GIRATORIO = 5,// 6. Ataque giratorio a corta distancia (-2 HP)
  COUNT_DISTANCIA = 5
};

enum class EstadoBoss {
  INTRO,
  PATRULLA,
  TELEGRAPH,
  ATACANDO,
  TRANSICION_FASE2,
  DERROTADO
};

struct EventosFrameBoss {
  bool sfx_telegraph{false};
  bool sfx_disparo{false};
  bool sfx_fase2{false};
  bool limpiar_balas_por_fase2{false};
  bool explosion_derrota{false};
};

class BossAegis {
public:
  BossAegis() = default;

  void cargarTexturas();
  void inicializar(const CE::Vector2D &centro_arena, int hp_max_inicial,
                   float mult_vel_balas);
  void activarCombate();

  EventosFrameBoss actualizar(float dt, const CE::Vector2D &pos_jugador,
                              std::vector<ProyectilBoss> &balas_out,
                              float escala_tiempo_enemigo);

  bool recibirDanio(int danio);
  void dibujar(sf::RenderTarget &target) const;
  void dibujarTelegraph(sf::RenderTarget &target,
                        const CE::Vector2D &pos_jugador) const;

  // Consultas de estado
  CE::Vector2D getPosicion() const { return pos; }
  float getRadioColisionJugador() const { return 62.f; }
  float getRadioHitboxBala() const { return 62.f; }
  float getRadioGiroCercano() const { return 162.f; }
  bool estaEnGiroCercanoActivo() const {
    return estaVivo() && estado == EstadoBoss::ATACANDO &&
           ataque_actual == AtaqueBossId::ATAQUE_GIRATORIO;
  }
  int getHP() const { return hp; }
  int getHPMax() const { return hp_max; }
  bool estaVivo() const { return hp > 0 && estado != EstadoBoss::DERROTADO; }
  bool esFase2() const { return fase2; }
  EstadoBoss getEstado() const { return estado; }
  std::string getNombreAtaqueActual() const;
  std::string getSubtituloEstado() const;

private:
  AtaqueBossId elegirSiguienteAtaque();
  void iniciarAtaque(AtaqueBossId atk, const CE::Vector2D &pos_jugador);
  void ejecutarAtaque(float dt, const CE::Vector2D &pos_jugador,
                      std::vector<ProyectilBoss> &balas_out,
                      EventosFrameBoss &ev);
  void actualizarAnimacion(float dt);
  void aplicarTexturaEstado(const std::string &clave_tex);

  // Generadores de proyectiles
  void spawnOrbe(std::vector<ProyectilBoss> &out, const CE::Vector2D &origen,
                 float ang, float speed, float escala = 0.85f) const;
  void spawnLanza(std::vector<ProyectilBoss> &out, const CE::Vector2D &origen,
                  float ang, float speed, float escala = 0.82f) const;
  void spawnMortero(std::vector<ProyectilBoss> &out, const CE::Vector2D &origen,
                    float ang, float speed, float det_time, int fragmentos) const;
  void spawnCapsula(std::vector<ProyectilBoss> &out, const CE::Vector2D &origen,
                    float ang, float speed, float det_time) const;

  // Posición y arena
  CE::Vector2D centro_arena{744.f, 744.f};
  CE::Vector2D pos{744.f, 744.f};
  CE::Vector2D destino_patrulla{744.f, 744.f};
  float radio_patrulla{200.f};

  // Estadísticas
  int hp_max{1450};
  int hp{1450};
  bool fase2{false};
  float mult_vel_balas{1.0f};

  // FSM y ataques
  EstadoBoss estado{EstadoBoss::INTRO};
  AtaqueBossId ataque_actual{AtaqueBossId::ESPIRAL_REACTOR};
  std::vector<AtaqueBossId> bolsa_ataques;
  float timer_estado{0.f};
  float duracion_estado{0.f};
  float timer_disparo{0.f};
  float timer_secundario{0.f};
  float angulo_patron{0.f};
  float angulo_fijado{0.f};
  float cooldown_giro_cercano{0.f};
  float rotacion_sprite{0.f};
  int paso_ataque{0};

  // Visual y animación (180x180 por frame, 4 frames por hoja)
  std::optional<sf::Sprite> sprite;
  std::string textura_actual;
  int frame_actual{0};
  float timer_anim{0.f};
  float duracion_frame{0.14f};
  float timer_flash_hit{0.f};
  float tiempo_global{0.f};
  float alpha_derrota{255.f};
  bool texturas_cargadas{false};
};

} // namespace IVJ
