#pragma once
#include "../Componentes/IComponentes.hpp"
#include <SFML/Graphics.hpp>

namespace CE
{
/**
 * @class Objeto
 * @brief Clase base para objetos del juego implementando patrón de arquitectura basada en componentes.
 *
 * Hereda de sf::Drawable y sf::Transformable de SFML para integración con el sistema de renderizado.
 * Utiliza composición de componentes (IComponentes) para definir comportamiento y propiedades.
 *
 * Características principales:
 * - Sistema de componentes genéricos adjuntables en runtime
 * - Detección dinámica de componentes por tipo (RTTI)
 * - Contador estático de objetos activos
 * - Componentes principales preestablecidos: Nombre, Transformación, Estadísticas
 *
 * @note Las subclases deben implementar onUpdate(float dt) para lógica de actualización
 */
class Objeto : public sf::Drawable, public sf::Transformable
{
  public:
    /**
     * @brief Constructor del objeto.
     *
     * Inicializa el objeto con componentes básicos (nombre, transformación, estadísticas)
     * e incrementa el contador global de objetos.
     */
    explicit Objeto();
    Objeto(const Objeto &cpy);

    /** @brief Destructor virtual por defecto */
    virtual ~Objeto() = default;

    /**
     * @brief Implementa la interfaz Drawable de SFML.
     * @param target Target de renderizado de SFML (ventana o textura)
     * @param state Estados de renderizado de SFML (shader, blend, transform)
     */
    void draw(sf::RenderTarget &target, sf::RenderStates state) const override;

    /**
     * @brief Adjunta un componente compartido al objeto.
     * @param componnentes Puntero compartido al componente a agregar
     * @return Referencia al objeto para encadenamiento (fluent interface)
     *
     * Los componentes se almacenan en un contenedor y se pueden
     * recuperar por tipo usando getComponente<T>().
     */
    Objeto &addComponente(const std::shared_ptr<IComponentes> &componnentes);

    /**
     * @brief Copia un componente existente y lo adjunta al objeto.
     * @tparam T Tipo de componente (debe derivar de IComponentes)
     * @param componnentes Puntero al componente a copiar
     * @return Referencia al objeto para encadenamiento (fluent interface)
     *
     * Realiza una copia profunda del componente y lo agrega a la lista.
     */
    template <typename T> Objeto &copyComponente(const T *componnentes)
    {
        auto copia = std::make_shared<T>(*componnentes);
        componentes.push_back(copia);
        return *this;
    }

    /**
     * @brief Obtiene un componente del objeto por tipo.
     * @tparam T Tipo de componente a buscar (debe derivar de IComponentes)
     * @return Puntero al componente si existe, nullptr en caso contrario
     *
     * Utiliza RTTI (dynamic_cast) para encontrar el primer componente
     * del tipo especificado. Es seguro en tiempo de compilación con static_assert.
     */
    template <typename T> T *getComponente() const
    {
        static_assert(std::is_base_of<IComponentes, T>::value, "Solo derivados de IComponentes");
        for (auto &c : componentes)
        {
            T *comp = dynamic_cast<T *>(c.get());
            if (comp)
                return comp;
        }
        return nullptr;
    };

    /**
     * @brief Verifica si el objeto tiene un componente de tipo específico.
     * @tparam T Tipo de componente a buscar (debe derivar de IComponentes)
     * @return true si el componente existe, false en caso contrario
     *
     * Método auxiliar para verificación sin obtener el componente.
     */
    template <typename T> bool tieneComponente() const
    {
        static_assert(std::is_base_of<IComponentes, T>::value, "Solo derivados de IComponentes");
        for (auto &c : componentes)
        {
            T *comp = dynamic_cast<T *>(c.get());
            if (comp)
                return true;
        }
        return false;
    };

    /**
     * @brief Obtiene el número total de objetos creados.
     * @return Contador de objetos activos globales
     */
    static int getNumObjetos()
    {
        return num_objetos;
    }

    /**
     * @brief Obtiene el componente de nombre del objeto.
     * @return Puntero compartido al componente INombre
     */
    std::shared_ptr<INombre> getNombre()
    {
        return nombre;
    }

    /**
     * @brief Obtiene el componente de transformación del objeto.
     * @return Referencia al puntero compartido del componente ITransform
     */
    std::shared_ptr<ITransform> &getTransformada()
    {
        return transform;
    }

    /**
     * @brief Obtiene el componente de estadísticas del objeto.
     * @return Referencia al puntero compartido del componente IStats
     */
    std::shared_ptr<IStats> &getStats()
    {
        return stats;
    }

    /**
     * @brief Verifica si el objeto está vivo.
     * @return true si hp > 0, false en caso contrario
     */
    bool estaVivo() const
    {
        return stats->hp > 0;
    }

    /**
     * @brief Obtiene la lista completa de componentes del objeto.
     * @return Referencia al vector de componentes compartidos
     */
    std::vector<std::shared_ptr<IComponentes>> &getListaComponentes()
    {
        return componentes;
    }

    /**
     * @brief Establece la posición del objeto.
     * @param x Coordenada X en píxeles
     * @param y Coordenada Y en píxeles
     */
    void setPosicion(float x, float y);

    /**
     * @brief Actualiza la lógica del objeto cada frame.
     * @param dt Delta time (tiempo transcurrido en segundos)
     * @note Método virtual puro - debe ser implementado por subclases
     */
    virtual void onUpdate(float dt) = 0;

    /**
     * @brief Obtiene representación textual del objeto.
     * @return Cadena con el nombre del objeto
     */
    virtual std::string toString()
    {
        return nombre->nombre;
    }

    // PATCH
    virtual void inputFSM() {};

  private:
    /** @brief Contador estático de objetos globales */
    static int num_objetos;

  protected:
    /** @brief Componente de nombre del objeto */
    std::shared_ptr<INombre> nombre;

    /** @brief Componente de transformación (posición, velocidad, rotación) */
    std::shared_ptr<ITransform> transform;

    /** @brief Componente de estadísticas (salud, fuerza, etc) */
    std::shared_ptr<IStats> stats;

    /** @brief Contenedor de componentes adjuntos al objeto */
    std::vector<std::shared_ptr<IComponentes>> componentes;
};
} // namespace CE
