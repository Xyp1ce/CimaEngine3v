#pragma once
#include "../Utils/Vector2D.hpp"
#include <map>
#include <memory>
#include <string>

namespace IVJ
{
class Entidad;
}

namespace CE
{
/**
 * @class IComponentes
 * @brief Clase base abstracta para todos los componentes de entidades.
 *
 * Define la interfaz principal para el sistema de componentes. Todos los componentes
 * derivados heredan de esta clase y se pueden adjuntar a objetos (Objeto) para
 * proporcionar funcionalidad específica (transformación, renderizado, estadísticas, etc).
 *
 * El motor utiliza un patrón de arquitectura basada en componentes (ECS - Entity Component System)
 * para máxima flexibilidad y composición.
 */
class IComponentes
{
  public:
    /** @brief Destructor virtual por defecto */
    virtual ~IComponentes() {};
    virtual std::shared_ptr<IComponentes> clonar() const = 0;
};

/**
 * @class INombre
 * @brief Componente que almacena el nombre/identificador de una entidad.
 *
 * Proporciona un identificador textual único para cada entidad del juego.
 */
class INombre : public IComponentes
{
  public:
    /**
     * @brief Constructor del componente nombre.
     * @param nom Nombre asignado a la entidad
     */
    explicit INombre(const std::string &nom);

    /** @brief Destructor virtual */
    ~INombre() override {};
    std::shared_ptr<IComponentes> clonar() const override
    {
        return std::make_shared<INombre>(*this);
    };

  public:
    /** @brief Nombre/identificador de la entidad */
    std::string nombre;
};

/**
 * @class ITransform
 * @brief Componente de transformación que almacena posición, velocidad y rotación.
 *
 * Gestiona la transformación espacial de la entidad incluyendo posición,
 * velocidad para física y ángulo de rotación.
 */
class ITransform : public IComponentes
{
  public:
    /**
     * @brief Constructor por defecto (posición 0,0, velocidad 0,0, ángulo 0).
     */
    ITransform();

    /**
     * @brief Constructor con parámetros iniciales.
     * @param pos Posición inicial de la entidad
     * @param vel Velocidad inicial de la entidad
     * @param ang Ángulo de rotación inicial en grados
     */
    ITransform(const Vector2D &pos, const Vector2D &vel, float ang);

    /** @brief Destructor virtual */
    ~ITransform() override {};
    std::shared_ptr<IComponentes> clonar() const override
    {
        return std::make_shared<ITransform>(*this);
    };

  public:
    /** @brief Posición actual de la entidad */
    Vector2D posicion;

    /** @brief Posición anterior (frame anterior) para detección de cambios */
    Vector2D pos_prev;

    /** @brief Velocidad de la entidad (píxeles/segundo) */
    Vector2D velocidad;

    /** @brief Ángulo de rotación en grados */
    float angulo;
};

/**
 * @class ITimer
 * @brief Componente de temporizador para animaciones y timing de eventos.
 *
 * Rastrea el frame actual y la fracción de frame para animaciones
 * sincronizadas y efectos basados en tiempo.
 */
class ITimer : public IComponentes
{
  public:
    /** @brief Constructor inicializa timer en 0 */
    ITimer(int max_frame);

    /** @brief Destructor virtual */
    ~ITimer() override {};
    std::shared_ptr<IComponentes> clonar() const override
    {
        return std::make_shared<ITimer>(*this);
    };

    /** @brief Frame actual de la animación */
    int frame_actual;

    int frame_maximo;
};

/**
 * @class IStats
 * @brief Componente de estadísticas RPG de la entidad.
 *
 * Almacena valores de 8 bits (0-255) para típicas estadísticas
 * de juegos RPG: salud, fuerza, defensa, agilidad, etc.
 */
class IStats : public IComponentes
{
  public:
    /** @brief Destructor virtual */
    ~IStats() override {};

    /** @brief Puntos de vida actuales (8 bits = 0-255 máximo) */
    int hp;

    /** @brief Puntos de vida máximos */
    int hp_max;

    /** @brief Estadística de fuerza (daño base) */
    int str;

    /** @brief Estadística de defensa (reducción de daño) */
    int def;

    /** @brief Estadística de agilidad (velocidad y evasión) */
    int agi;
    bool congelar{false};
    std::shared_ptr<IComponentes> clonar() const override
    {
        return std::make_shared<IStats>(*this);
    };
};

/**
 * @class ITexto
 * @brief Componente para renderizar texto en la entidad.
 *
 * Contiene un objeto sf::Text de SFML para mostrar texto renderizado
 * en la posición de la entidad con la fuente especificada.
 */
class ITexto : public IComponentes
{
  public:
    /**
     * @brief Constructor del componente texto.
     * @param font Referencia a la fuente de SFML a usar
     * @param texto Contenido textual inicial
     */
    ITexto(const sf::Font &font, const std::string &texto);

    /** @brief Destructor virtual */
    ~ITexto() override {};
    std::shared_ptr<IComponentes> clonar() const override
    {
        return std::make_shared<ITexto>(*this);
    };

    /** @brief Objeto de texto SFML que se renderiza */
    sf::Text m_texto;
};

/**
 * @class ISprite
 * @brief Componente de sprite para renderizar gráficos 2D.
 *
 * Almacena un sprite SFML con textura, dimensiones y escala.
 * Proporciona dos constructores: uno con escala simple y otro con ancho/alto.
 */
class ISprite : public IComponentes
{
  public:
    /**
     * @brief Constructor simple con escala uniforme.
     * @param textura Referencia a la textura SFML
     * @param escala Factor de escala (1.0 = tamaño original)
     */
    ISprite(const sf::Texture &textura, float escala);

    /**
     * @brief Constructor con dimensiones específicas.
     * @param textura Referencia a la textura SFML
     * @param w Ancho deseado del sprite en píxeles
     * @param h Alto deseado del sprite en píxeles
     * @param escala Factor de escala adicional
     */
    ISprite(const sf::Texture &textura, int w, int h, float escala);

    /** @brief Destructor virtual */
    ~ISprite() override {};
    std::shared_ptr<IComponentes> clonar() const override
    {
        return std::make_shared<ISprite>(*this);
    };

  public:
    /** @brief Objeto sprite SFML para renderizado */
    sf::Sprite m_sprite;

    /** @brief Ancho del sprite en píxeles */
    int width;

    /** @brief Alto del sprite en píxeles */
    int height;

    /** @brief Factor de escala aplicado al sprite */
    float escala;

  private:
    /** @brief Textura SFML asociada al sprite */
    sf::Texture m_textura;
};

/**
 * @class IShader
 * @brief Componente para aplicar efectos de shader (GLSL) a una entidad.
 *
 * Gestiona un shader SFML con uniforms variables. Permite establecer
 * parámetros del shader como colores, tiempos, vectores y matrices.
 *
 * @note Los uniforms se almacenan como void* con tipo erasure para flexibilidad
 */
class IShader : public IComponentes
{
  public:
    /**
     * @enum ShaderVars
     * @brief Tipos de variables/uniforms soportados por el shader.
     */
    enum class ShaderVars
    {
        FLOAT, ///< Variable de punto flotante (float)
        VEC2F, ///< Vector de 2D (sf::Vector2f)
        VEC3F, ///< Vector de 3D (sf::Vector3f)
        VEC4F, ///< Vector de 4D (sf::Glsl::Vec4)
        MAT3,  ///< Matriz 3x3 (sf::Glsl::Mat3)
        MAT4,  ///< Matriz 4x4 (sf::Glsl::Mat4)
        TEX    ///< Textura (sf::Texture)
    };

  public:
    /**
     * @brief Constructor del componente shader.
     * @param vert Ruta al archivo de vertex shader
     * @param frag Ruta al archivo de fragment shader
     */
    explicit IShader(const std::string &vert, const std::string &frag);

    /** @brief Destructor virtual */
    ~IShader() override {};
    std::shared_ptr<IComponentes> clonar() const override
    {
        return nullptr;
    };

    /**
     * @brief Establece un uniform escalar (float).
     * @param key Nombre del uniform en el shader
     * @param valor Puntero al valor float
     */
    void setEscalar(const std::string &key, float *valor);

    /**
     * @brief Establece un uniform de vector 2D.
     * @param key Nombre del uniform en el shader
     * @param valor Puntero a sf::Vector2f
     */
    void setVector2(const std::string &key, sf::Vector2f *valor);

    /**
     * @brief Establece un uniform de vector 3D.
     * @param key Nombre del uniform en el shader
     * @param valor Puntero a sf::Vector3f
     */
    void setVector3(const std::string &key, sf::Vector3f *valor);

    /**
     * @brief Establece un uniform de vector 4D.
     * @param key Nombre del uniform en el shader
     * @param valor Puntero a sf::Glsl::Vec4
     */
    void setVector4(const std::string &key, sf::Glsl::Vec4 *valor);

    /**
     * @brief Establece un uniform de matriz 3x3.
     * @param key Nombre del uniform en el shader
     * @param valor Puntero a sf::Glsl::Mat3
     */
    void setMat3(const std::string &key, sf::Glsl::Mat3 *valor);

    /**
     * @brief Establece un uniform de matriz 4x4.
     * @param key Nombre del uniform en el shader
     * @param valor Puntero a sf::Glsl::Mat4
     */
    void setMat4(const std::string &key, sf::Glsl::Mat4 *valor);

    /**
     * @brief Establece un uniform de textura.
     * @param key Nombre del uniform en el shader
     * @param valor Puntero a sf::Texture
     */
    void setTextura(const std::string &key, sf::Texture *valor);

    /**
     * @brief Cambia el programa de shader por otro.
     * @param vert Ruta al nuevo vertex shader
     * @param frag Ruta al nuevo fragment shader
     */
    void cambiarShader(const std::string &vert, const std::string &frag);

  public:
    /** @brief Objeto shader SFML compilado */
    sf::Shader m_shader;

    /** @brief Ruta del archivo fragment shader */
    std::string m_fragshaderFile;

    /** @brief Ruta del archivo vertex shader */
    std::string m_vertshaderFile;

    /** @brief Mapa de uniforms con tipo y puntero a valor (type erasure) */
    std::map<std::string, std::pair<IShader::ShaderVars, void *>> m_vars;
};

/**
 * @class IControl
 * @brief Componente que rastrea el estado de entrada/controles de una entidad.
 *
 * Almacena booleans para 8 direcciones y acciones de entrada,
 * típicamente mapeadas a controles del jugador o IA.
 *
 * @note Futuro: considerar usar bitfield (entero de 32 bits) para 32 botones
 */
class IControl : public IComponentes
{
  public:
    /** @brief Constructor inicializa todos los controles en false */
    explicit IControl();

    /** @brief Destructor virtual */
    ~IControl() override {};
    std::shared_ptr<IComponentes> clonar() const override
    {
        return std::make_shared<IControl>(*this);
    };

  public:
    /** @brief Entrada: dirección arriba */
    bool arr;

    /** @brief Entrada: dirección abajo */
    bool abj;

    /** @brief Entrada: dirección izquierda */
    bool izq;

    /** @brief Entrada: dirección derecha */
    bool der;

    /** @brief Entrada: acción correr */
    bool run;

    /** @brief Entrada: acción saltar */
    bool jmp;

    /** @brief Entrada: acción atacar/usar */
    bool acc;

    /** @brief Entrada: acción secundaria */
    bool sacc;

  public:
    /**
     * @brief Activa o desactiva el componente de control.
     * @param a true para activar, false para desactivar
     */
    void setActivo(bool a)
    {
        activo = a;
    }

    /**
     * @brief Verifica si el componente de control está activo.
     * @return true si está activo, false si está desactivado
     */
    bool isActivo() const
    {
        return activo;
    }

  private:
    /** @brief Bandera de activación del componente */
    bool activo{true};
};

/**
 * @class IBoundingBox
 * @brief Componente de colisión que define un área de límites rectangular.
 *
 * Almacena las dimensiones de la caja colisora y su punto medio
 * para facilitar cálculos de colisión y detección espacial.
 */
class IBoundingBox : public IComponentes
{
  public:
    /**
     * @brief Constructor de la caja colisora.
     * @param dim Dimensiones de la caja (ancho y alto)
     */
    explicit IBoundingBox(const Vector2D &dim);
    ~IBoundingBox() override {};
    std::shared_ptr<IComponentes> clonar() const override
    {
        return std::make_shared<IBoundingBox>(*this);
    };

    /** @brief Dimensiones totales de la caja colisora (ancho, alto) */
    Vector2D tam;

    /** @brief Mitad de las dimensiones (usado para cálculos rápidos) */
    Vector2D mitad;
};

/**
 * @class IRespawn
 * @brief Componente para gestionar generación de entidades (spawning).
 *
 * Maneja la aparición periódica de nuevas entidades (enemigos, items) en un área.
 * Proporciona dos modos: por lista de componentes o prefab de entidad.
 */
class IRespawn : public IComponentes
{
  public:
    /**
     * @brief Constructor del respawn con lista de componentes.
     * @param lista_comp Lista de componentes para cada entidad generada
     * @param max Número máximo de entidades a generar
     */
    IRespawn(std::vector<std::shared_ptr<IComponentes>> &lista_comp, int max);

    /**
     * @brief Constructor del respawn con prefab de entidad.
     * @param max Número máximo de entidades a generar
     * @param w Ancho del área de spawn en píxeles
     * @param h Alto del área de spawn en píxeles
     * @param pre Puntero a entidad prefab a duplicar
     */
    IRespawn(int max, int w, int h, IVJ::Entidad *pre);
    ~IRespawn() override {};
    std::shared_ptr<IComponentes> clonar() const override
    {
        return std::make_shared<IRespawn>(*this);
    };

    /** @brief Contador de frames actual en el temporizador de spawn */
    int timer_actual;

    /** @brief Intervalo de frames entre spawns */
    int timer_maximo;

    /** @brief Número de entidades generadas actualmente */
    int num_objetos;

    /** @brief Puntero a entidad prefab para duplicar */
    IVJ::Entidad *prefab;

    /** @brief Ancho del área de spawn en píxeles */
    int width;

    /** @brief Alto del área de spawn en píxeles */
    int height;

    /** @brief Contador de spawns actuales */
    int curr_spawn{0};

    /** @brief Máximo número de spawns permitidos */
    int max_spawn;

    /** @brief Lista de componentes para cada entidad generada */
    std::vector<std::shared_ptr<IComponentes>> componentes;
};

/**
 * @class IPaths
 * @brief Componente para seguimiento de trayectorias usando curvas Bézier.
 *
 * Permite que una entidad siga una ruta predefinida usando curvas Bézier cuadráticas.
 * Almacena puntos de control y rastrea la posición actual en la trayectoria.
 */
class IPaths : public IComponentes
{
  public:
    /**
     * @brief Constructor del componente de trayectorias.
     * @param total_frames Número total de frames de la trayectoria completa
     */
    explicit IPaths(int total_frames);
    ~IPaths() override {};

    std::shared_ptr<IComponentes> clonar() const override
    {
        return std::make_shared<IPaths>(*this);
    };
    /**
     * @brief Agrega una curva Bézier cuadrática a la trayectoria.
     * @param p1 Primer punto de control
     * @param p2 Punto de control central
     * @param p3 Tercer punto de control
     */
    void addCurva(Vector2D p1, Vector2D p2, Vector2D p3);

  public:
    /** @brief Lista de puntos que forman la trayectoria */
    std::vector<Vector2D> puntos;

    /** @brief Desplazamiento en la lista de puntos */
    int offset;

    /** @brief ID de la curva actual siendo seguida */
    int id_curva;

    /** @brief Total de frames de la curva actual */
    int frame_total_curva;

    /** @brief Frame actual dentro de la curva */
    int frame_actual_curva;
};
} // namespace CE
