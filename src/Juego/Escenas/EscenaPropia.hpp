#pragma once
#include <Juego/Sistemas/ConfigExpendable.hpp>
#include <Juego/Sistemas/SistemaOrbes.hpp>
#include <Juego/objetos/BossAegis.hpp>
#include <Juego/objetos/Entidad.hpp>
#include <Juego/objetos/TileMap.hpp>
#include <Motor/Primitivos/Escena.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace IVJ {

enum class FasePartida {
  INTRO,
  COMBATE,
  VICTORIA,
  DERROTA
};

struct ProyectilBala {
  CE::Vector2D pos{0.f, 0.f};
  CE::Vector2D direccion{0.f, -1.f};
  float velocidad{900.f};
  float angulo{0.f};
  float tiempo_vida{2.2f};
  float tiempo_transcurrido{0.f};
  int danio{10};
  bool es_critico{false};
  bool es_homing{false};
  bool es_railgun{false};
  float escala{0.72f};
  int id_salva{0};
};

struct OrbeMejora {
  CE::Vector2D pos{0.f, 0.f};
  TipoOrbe tipo{TipoOrbe::TEMPORAL_MORADO};
  int disparos_restantes{4};      // Resistencia fija de 4 disparos del jugador (GDD)
  float tiempo_vida_max{15.0f};   // 15 segundos antes de expirar (GDD)
  float tiempo_transcurrido{0.f};
  float rotacion{0.f};
  float timer_flash{0.f};
};

struct ParticulaVFX {
  CE::Vector2D pos{0.f, 0.f};
  CE::Vector2D vel{0.f, 0.f};
  float radio{4.f};
  float vida{0.35f};
  float vida_max{0.35f};
  sf::Color color{sf::Color::White};
};

struct OndaExpansivaVFX {
  CE::Vector2D pos{0.f, 0.f};
  float radio_inicial{14.f};
  float radio_final{120.f};
  float vida{0.35f};
  float vida_max{0.35f};
  sf::Color color{sf::Color::White};
};

struct TextoFlotanteVFX {
  CE::Vector2D pos{0.f, 0.f};
  std::string texto;
  sf::Color color{sf::Color::White};
  unsigned int tam{16};
  float vida{0.85f};
  float vida_max{0.85f};
};

struct AfterImageDash {
  sf::Vector2f pos{0.f, 0.f};
  const sf::Texture *textura{nullptr};
  sf::IntRect rect{};
  float vida{0.22f};
  float vida_max{0.22f};
};

struct MuzzleFlashVFX {
  CE::Vector2D pos{0.f, 0.f};
  float angulo{0.f};
  float vida{0.f};
};

class Escena_Propia : public CE::Escena {
public:
  explicit Escena_Propia(std::shared_ptr<Entidad> &pref);
  virtual ~Escena_Propia() {};
  void onInit() override;
  void onFinal() override;
  void onUpdate(float dt) override;
  void onInputs(const CE::Botones &accion) override;
  void onRender() override;
  std::shared_ptr<Entidad> getJugador() override { return jugador_soldier; }

private:
  // Ciclo de partida
  void cargarAudio();
  void reproducirSFX(const std::string &clave, float volumen = 55.f,
                     float pitch = 1.0f);
  void reiniciarPartida();

  // Movimiento, colisiones y jugador
  void moverJugadorConSliding(float dt);
  void intentarDash();
  void daniarJugador(int cantidad = 1, const std::string &etiqueta = "");
  void dispararBala();
  bool iniciarRecarga();
  void recargarArma();
  bool estaRodando() const;
  void limpiarControles();
  void resincronizarControles();

  // Sistema de Orbes (GDD)
  void registrarImpactoEnJefe(int danio_aplicado, int id_salva);
  void evaluarDropOrbe();
  void generarOrbe(TipoOrbe tipo);
  void reclamarOrbe(const OrbeMejora &orbe);

  // Proyectiles del jefe y detonaciones
  void actualizarProyectilesBoss(float dt);

  // VFX y helpers visuales
  void spawnParticulas(const CE::Vector2D &pos, int cant,
                       const sf::Color &color, float vel_min, float vel_max,
                       float radio = 4.f, float vida = 0.4f);
  void spawnOnda(const CE::Vector2D &pos, float r0, float r1,
                 const sf::Color &color, float vida = 0.35f);
  void spawnTextoFlotante(const CE::Vector2D &pos, const std::string &txt,
                          const sf::Color &color, unsigned int tam = 16,
                          float vida = 0.85f);
  void renderHUD(sf::RenderTexture &tex);
  void renderOverlayPausa(sf::RenderTexture &tex);
  void renderOverlayFinPartida(sf::RenderTexture &tex);

private:
  int inicializar{1};
  std::shared_ptr<Entidad> &jugador_soldier;
  std::vector<TileMap> tiles_industrial;

  // Geometría de la arena circular (31x31 tiles * 48px = 1488x1488)
  const CE::Vector2D centro_arena{744.f, 744.f};
  const CE::Vector2D pos_spawn_jugador{744.f, 1224.f};
  const float radio_arena_libre{564.f};

  // Estado general de la partida y dificultad
  ReglasDificultad reglas;
  FasePartida fase_partida{FasePartida::INTRO};
  float timer_intro{0.f};
  const float duracion_intro{1.35f};
  float tiempo_combate{0.f};
  float tiempo_global{0.f};
  int opcion_fin{0}; // 0: Reintentar, 1: Menu principal

  // Estadísticas y estado del Jugador
  BuildJugador build;
  int hp_jugador{6};
  int hp_max_jugador{6};
  float timer_invuln_golpe{0.f};
  float timer_flash_danio_hud{0.f};
  float cooldown_dash_restante{0.f};
  bool estaba_rodando{false};
  float timer_afterimage{0.f};
  int frame_muerte_jugador{0};
  float timer_muerte_jugador{0.f};
  int disparos_totales{0};
  int disparos_acertados{0};
  int danio_total_infligido{0};
  int contador_salva{0};
  int ultima_salva_con_impacto{-1};

  // Disparo, munición y recarga
  std::vector<ProyectilBala> proyectiles;
  float tiempo_ultimo_disparo{0.5f};
  int balas_cargador{24};
  int balas_reserva{360};
  const int balas_reserva_inicial{360};
  bool recargando{false};
  float tiempo_recarga_actual{0.0f};
  std::optional<sf::Sprite> sprite_arma_hud;
  MuzzleFlashVFX muzzle_flash;

  // Jefe AEGIS-01 y sus proyectiles naranjas
  BossAegis boss;
  std::vector<ProyectilBoss> proyectiles_boss;

  // Sistema de Orbes de Mejora (5 disparos consecutivos -> 30% permanente / 10% temporal)
  std::vector<OrbeMejora> orbes;
  int impactos_consecutivos_jefe{0};
  int impactos_requeridos_drop{5};

  // VFX
  std::vector<ParticulaVFX> particulas;
  std::vector<OndaExpansivaVFX> ondas;
  std::vector<TextoFlotanteVFX> textos_flotantes;
  std::vector<AfterImageDash> afterimages;

  // Audio local (buffers independientes para evitar sobreescritura en GestorAssets)
  std::map<std::string, sf::SoundBuffer> sfx_buffers;
  std::vector<sf::Sound> sfx_pool;
  bool audio_cargado{false};

  // Sistema de Pausa
  bool en_pausa{false};
  int opcion_pausa{0}; // 0: Reanudar, 1: Regresar al menu principal, 2: Escritorio
};

} // namespace IVJ
