#pragma once
#include "../Inputs/Botones.hpp"
#include "CEPool.hpp"
#include <SFML/Graphics.hpp>
//shaders y tilemap mover el source a motor/mapas
#include "../../Juego/objetos/TileMap.hpp"
#include<vector>
#include<map>

namespace CE
{
    /**
     * @class Escena
     * @brief Clase base abstracta para las escenas del juego.
     * 
     * Define la interfaz que deben implementar todas las escenas del juego.
     * Una escena contiene lógica específica, objetos, tilemaps y mapeos de entrada.
     * 
     * Características:
     * - Pool de objetos para gestión de entidades
     * - Sistema de mapeo de botones a acciones
     * - Soporte para tilemaps con múltiples capas
     * - Hooks del ciclo de vida (init, update, render, cleanup)
     * 
     * @note Las subclases deben implementar todos los métodos virtuales puros
     */
    class Escena
    {
        /** @brief Tipo alias para mapeo de botones: scancode → nombre de acción */
        typedef std::map<sf::Keyboard::Scancode,std::string> HashBotones;
        
        public:
            /** @brief Destructor virtual */
            virtual ~Escena(){};

            /**
             * @brief Inicializa la escena (se llama al cargarla).
             * @note Método virtual puro - debe ser implementado por subclases
             */
            virtual void onInit()=0;
            
            /**
             * @brief Limpia la escena (se llama al descargarla).
             * @note Método virtual puro - debe ser implementado por subclases
             */
            virtual void onFinal()=0;
            
            /**
             * @brief Actualiza la lógica de la escena.
             * @param dt Delta time en segundos
             * @note Método virtual puro - debe ser implementado por subclases
             */
            virtual void onUpdate(float dt)=0;
            
            /**
             * @brief Procesa entrada de botones mapeados.
             * @param accion Botón presionado con sus propiedades
             * @note Método virtual puro - debe ser implementado por subclases
             */
            virtual void onInputs(const CE::Botones& accion)=0;
            
            /**
             * @brief Renderiza la escena.
             * @note Método virtual puro - debe ser implementado por subclases
             */
            virtual void onRender()=0;
            
            /**
             * @brief Registra un mapeo de tecla a acción.
             * @param boton Escancode de SFML de la tecla
             * @param accion Nombre textual de la acción asignada
             */
            void registrarBotones(sf::Keyboard::Scancode boton, const std::string &accion)
            {
                botones[boton]=accion;
            }
            
            /**
             * @brief Obtiene el mapeo de botones a acciones.
             * @return Referencia mutable al mapa de botones
             */
            HashBotones &getBotones()
            {
                return botones;
            }
            
            /**
             * @brief Obtiene el pool de objetos de la escena.
             * @return Referencia mutable al pool de objetos
             */
            Pool& getPool()
            {
                return objetos;
            }
            
            /**
             * @brief Obtiene todas las teclas registradas como vector.
             * @return Vector con los escancodes de todas las teclas registradas
             */
            std::vector<sf::Keyboard::Scancode> 
            getBotonesRegistrados()
            {
                std::vector<sf::Keyboard::Scancode> b;
                b.reserve(botones.size());
                for(auto k = botones.begin();k!=botones.end();k++)
                {
                    b.emplace_back(k->first);
                }
                return b;
            }
            
            /**
             * @brief Obtiene la entidad del jugador de la escena.
             * @return Puntero compartido a la entidad del jugador (nullptr por defecto)
             * @note Puede ser sobrescrito por subclases
             */
            virtual std::shared_ptr<IVJ::Entidad> getJugador(){return nullptr;}
            
            /**
             * @brief Obtiene todas las capas de tilemaps.
             * @return Referencia mutable al vector de tilemaps
             */
            std::vector<IVJ::TileMap>& getTileMapsLayers()  {return tiles_layers;};
            
        public:
            /** @brief Mapeo de botones a nombres de acciones (público para acceso directo) */
            HashBotones botones;
            
            /** @brief Mapeo de botones GUI (interfaz gráfica) */
            HashBotones gui;
            
            /** @brief Nombre/identificador de la escena */
            std::string nombre;
            
        protected:
            /** @brief Pool de objetos de la escena (capacidad: 100 objetos) */
            Pool objetos{100};
            
            /** @brief Vector de tilemaps con múltiples capas */
            std::vector<IVJ::TileMap> tiles_layers;
    };
}
