#pragma once
#include <SFML/Graphics.hpp>
#include <optional>

namespace CE
{
    /**
     * @class InputBuffer
     * @brief Buffer circular para almacenar eventos de entrada de SFML.
     * 
     * Implementa una cola FIFO (First-In-First-Out) para procesar eventos
     * de entrada de manera asincrónica. Los eventos se insertan desde el
     * procesador de eventos del sistema y se consumen por la lógica de juego.
     * 
     * @note Capacidad: especificada en constructor
     * @note Soporta std::optional<sf::Event> para manejar ausencia de eventos
     */
    class InputBuffer
    {
        public:
            /**
             * @brief Constructor del buffer circular.
             * @param size Capacidad máxima del buffer (número de eventos)
             */
            explicit InputBuffer(int size);
            
            /**
             * @brief Agrega un evento al final de la cola.
             * @param evento Referencia al evento opcional de SFML a almacenar
             */
            void pushData(std::optional<sf::Event>& evento);
            
            /**
             * @brief Extrae y retorna el evento del inicio de la cola.
             * @return Referencia a evento opcional (frente de la cola)
             */
            std::optional<sf::Event>& popData();
            
            /**
             * @brief Limpia todos los eventos del buffer.
             */
            void clearData();
            
        private:
            /** @brief Vector que almacena los eventos */
            std::vector<std::optional<sf::Event>> buffer;
            
            /** @brief Índice del tope de escritura (siguiente posición para insertar) */
            int tope;
            
            /** @brief Índice de salida de lectura (siguiente posición para extraer) */
            int salir;
            
            /** @brief Contador de elementos actualmente en el buffer */
            int ele{0};
            
        public:
            /** @brief Tamaño/capacidad máxima del buffer (constante) */
            const int size;
            
            /**
             * @brief Verifica si el buffer está vacío.
             * @return true si no hay eventos en el buffer, false en caso contrario
             */
            bool estaVacio() const {return ele==0;}
    };
}

