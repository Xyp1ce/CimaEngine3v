#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window/Cursor.hpp>
#include "../Utils/Utils.hpp"
#include<memory>
#include<optional>

namespace CE
{
    /**
     * @class Render
     * @brief Gestor singleton del sistema de renderizado SFML.
     * 
     * Gestiona la creación y mantenimiento de ventana y textura de renderizado.
     * Mantiene colas de objetos dibujables y coordina operaciones de renderizado.
     * 
     * Soporta doble target: RenderWindow (ventana del sistema) y RenderTexture
     * (textura para post-procesamiento o captura).
     * 
     * @note Patrón Singleton - instancia única a través de Get()
     */
    class Render
    {
        public:
            /**
             * @brief Obtiene la instancia singleton del gestor de renderizado.
             * @return Referencia mutable al renderizador único
             */
            static Render& Get(void);
            
            /**
             * @brief Limpia/termina el gestor y libera recursos.
             */
            static void Terminar(void);
            
            /**
             * @brief Crea la ventana de renderizado.
             * @param motor_info Configuración con dimensiones y título de ventana
             */
            void CrearVentana(const MotorConfig& motor_info);
            
            /**
             * @brief Redimensiona la textura de renderizado.
             * @param w Nuevo ancho
             * @param h Nuevo alto
             */
            void AutoResizeTextura(unsigned int  w, unsigned int h);
            
            /**
             * @brief Agrega un objeto drawable a la cola de renderizado.
             * @param obj Referencia constante al objeto drawable
             */
            void AddToDraw(const sf::Drawable &obj);
            
            /**
             * @brief Agrega un objeto drawable con estados de renderizado personalizados.
             * @param obj Referencia constante al objeto drawable
             * @param shader Estados de renderizado (shader, blend, transform)
             */
            void AddToDraw(const sf::Drawable &obj, sf::RenderStates shader);
            
            /**
             * @brief Establece el color de limpieza de la ventana.
             * @param clear Color SFML para limpiar fondo
             */
            void OnClearColor(const sf::Color& clear);
            
            /**
             * @brief Muestra/renderiza el contenido de la textura.
             */
            void OnDisplayTextura(void);
            
            /**
             * @brief Intercambia buffers y muestra la ventana.
             */
            void OnDisplayVentana(void);
            
            /**
             * @brief Obtiene la ventana de renderizado SFML.
             * @return Referencia mutable a RenderWindow
             */
            sf::RenderWindow& GetVentana(void);
            
            /**
             * @brief Obtiene la textura de renderizado SFML.
             * @return Referencia mutable a RenderTexture
             */
            sf::RenderTexture& GetTextura(void);

            /**
             * @brief Configura la visibilidad del cursor del ratón.
             * @param visible true para mostrar el cursor, false para ocultarlo
             */
            void setMouseCursorVisible(bool visible);

            /**
             * @brief Actualiza las dimensiones y posición del viewport en pantalla.
             */
            void setViewportRect(float x, float y, float w, float h);

            /**
             * @brief Obtiene las coordenadas en el mundo del juego del cursor del ratón.
             */
            sf::Vector2f getMouseWorldPos() const;

            /**
             * @brief Indica si el cursor está dentro del área visible del juego en el viewport.
             */
            bool isMouseInsideViewport() const;
            
        private:
            /**
             * @brief Constructor privado (singleton).
             */
            explicit Render()=default;
            
            /** @brief Ventana de renderizado SFML (gestión automática) */
            std::unique_ptr<sf::RenderWindow> r_ventana;
            
            /** @brief Textura de renderizado SFML (gestión automática) */
            std::unique_ptr<sf::RenderTexture> r_textura;

            /** @brief Cursor del sistema preservado en memoria */
            std::optional<sf::Cursor> cursor_flecha;

            /** @brief Posición y tamaño del viewport de ImGui */
            float viewport_x{0.f};
            float viewport_y{0.f};
            float viewport_w{0.f};
            float viewport_h{0.f};
            
            /** @brief Instancia singleton del renderizador */
            static inline Render* instancia =nullptr;
    };
}
