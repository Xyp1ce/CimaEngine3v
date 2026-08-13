#pragma once
#include <SFML/Graphics.hpp>
#include "../Utils/Utils.hpp"
#include<memory>

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
            
        private:
            /**
             * @brief Constructor privado (singleton).
             */
            explicit Render()=default;
            
            /** @brief Ventana de renderizado SFML (gestión automática) */
            std::unique_ptr<sf::RenderWindow> r_ventana;
            
            /** @brief Textura de renderizado SFML (gestión automática) */
            std::unique_ptr<sf::RenderTexture> r_textura;
            
            /** @brief Instancia singleton del renderizador */
            static inline Render* instancia =nullptr;
    };
}
