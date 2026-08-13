#pragma once
#include <SFML/Graphics.hpp>
#include "../Inputs/InputBuffer.hpp"

namespace CE
{
    /**
     * @class GameLayer
     * @brief Clase base abstracta para la capa de lógica principal de juego.
     * 
     * Define la interfaz que debe implementar la capa de juego, permitiendo que
     * el motor gestione el ciclo de vida de la aplicación. Proporciona puntos de
     * enganche (hooks) para inicialización, procesamiento de entrada, actualización
     * y renderizado.
     * 
     * Las clases derivadas deben implementar:
     * - OnInputs() en sus tres variantes para procesamiento de entrada flexible
     * - OnUpdate(float dt) para lógica de actualización
     * - OnRender() para renderizado (opcional)
     */
    class GameLayer
    {
        public:
            /** @brief Destructor virtual de la capa de juego */
            virtual ~GameLayer(){};

            /**
             * @brief Hook de inicialización llamado al inicio del motor.
             * 
             * Las subclases pueden sobrescribir este método para realizar
             * inicializaciones específicas de su lógica de juego.
             */
            virtual void OnInit(void){};
            
            /**
             * @brief Verifica si la capa de juego sigue activa.
             * @return true si el juego está corriendo, false si debe terminar
             */
            virtual bool EstaCorriendo(void){return !termino;};
            
            /**
             * @brief Procesa eventos de SFML individuales.
             * @param eventos Referencia a evento opcional de SFML
             * @note Método virtual puro - debe ser implementado por subclases
             */
            virtual void OnInputs(std::optional<sf::Event>& eventos)=0;
            
            /**
             * @brief Procesa el buffer completo de eventos acumulados.
             * @param in_buffer Referencia al buffer de entrada circulante
             * @note Método virtual puro - debe ser implementado por subclases
             */
            virtual void OnInputs(InputBuffer& in_buffer)=0;
            
            /**
             * @brief Procesa entrada sin parámetros (entrada continua como teclas sostenidas).
             * @note Método virtual puro - debe ser implementado por subclases
             */
            virtual void OnInputs()=0;
            
            /**
             * @brief Actualiza la lógica de la capa de juego.
             * @param dt Delta time (tiempo transcurrido en segundos)
             * @note Método virtual puro - debe ser implementado por subclases
             */
            virtual void OnUpdate(float dt)=0;
            
            /**
             * @brief Hook de renderizado llamado cada frame.
             * 
             * Las subclases pueden sobrescribir este método para realizar
             * renderizado específico. Por defecto no hace nada.
             */
            virtual void OnRender(){};

        protected:
            /** @brief Bandera que indica si la capa debe terminar (false = continuar ejecutando) */
            bool termino;
            //sf::Event trigger_evento;
    };
}
