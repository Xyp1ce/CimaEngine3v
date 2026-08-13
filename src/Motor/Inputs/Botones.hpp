#pragma once

#include <SFML/Graphics.hpp>
#include <string>

namespace CE
{
    /**
     * @class Botones
     * @brief Encapsula la configuración de un botón/tecla con su tipo de acción.
     * 
     * Representa un mapeo entre una tecla del teclado y una acción de entrada
     * específica. Cada botón está identificado por un nombre, tiene un tipo de acción
     * (Al presionar o al liberar) y está asociado a un escancode SFML.
     * 
     * @note Los escancodes permiten mapeo independiente del idioma del teclado
     */
    class Botones
    {
        public:
            /**
             * @enum TipoAccion
             * @brief Tipos de eventos de acción que puede generar un botón.
             */
            enum class TipoAccion
            {
                OnPress,
                OnRelease,
                None
            };
            
        public:
            /**
             * @brief Constructor del botón.
             * @param nombre Identificador textual del botón (ej: "saltar", "atacar")
             * @param tipo Tipo de acción (OnPress, OnRelease)
             * @param scan Escancode de SFML de la tecla asociada
             */
            Botones(const std::string& nombre, const TipoAccion& tipo,const sf::Keyboard::Scancode& scan);
            
            /**
             * @brief Obtiene el nombre/identificador del botón.
             * @return Nombre del botón como cadena constante
             */
            const std::string getNombre() const;
            
            /**
             * @brief Obtiene el tipo de acción como cadena de texto.
             * @return Representación textual del tipo de acción ("OnPress", "OnRelease", "None")
             */
            const std::string getTipoString() const;
            
            /**
             * @brief Obtiene el tipo de acción como enumeración.
             * @return Referencia constante al TipoAccion del botón
             */
            const TipoAccion& getTipo() const;
            
            /**
             * @brief Obtiene representación textual completa del botón.
             * @return Cadena con formato: "nombre: tipo"
             */
            const std::string toString() const;
            
            /**
             * @brief Obtiene el escancode de la tecla SFML.
             * @return Referencia constante al sf::Keyboard::Scancode
             */
            const sf::Keyboard::Scancode& getScancode() const;
            
        private:
            /** @brief Nombre/identificador del botón (ej: "saltar") */
            std::string nombre;
            
            /** @brief Tipo de acción asociada (OnPress, OnRelease) */
            TipoAccion accion{TipoAccion::None};
            
            /** @brief Escancode de SFML de la tecla del teclado */
            sf::Keyboard::Scancode tecla;
    };
}
