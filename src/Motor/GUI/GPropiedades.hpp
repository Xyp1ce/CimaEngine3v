#pragma once
#include "GLayer.hpp"
#include "GPaths.hpp"
#include "../Primitivos/Objetos.hpp"
#include "../Componentes/IComponentes.hpp"
#include <Juego/Componentes/IJComponentes.hpp>
#include "../../Juego/objetos/TileMap.hpp"

namespace CE
{
    /**
     * @class GPropiedades
     * @brief Inspector de propiedades para editar objetos y tilemaps en tiempo real.
     * 
     * Panel ImGui que permite editar propiedades de objetos seleccionados,
     * incluyendo transformación, sprites, shaders, controles y trayectorias.
     * También permite edición de propiedades de tilemaps.
     * 
     * Utiliza múltiples sub-paneles para diferentes tipos de componentes:
     * - Transform: posición, velocidad, rotación
     * - Sprite: textura, dimensiones, escala
     * - Shader: uniforms del shader
     * - Control: estado de botones de entrada
     * - Paths: editor de trayectorias (delegado a GPaths)
     * 
     * @note Derivada de GLayer para integración en sistema GUI del motor
     * @note Contiene instancia de GPaths para edición de trayectorias
     */
    class GPropiedades : public GLayer
    {
        public:
            /** @brief Destructor virtual - limpia GPaths */
            ~GPropiedades() override
            {
                delete guipath;    
            };
            
            /**
             * @brief Inicializa el inspector de propiedades.
             * @param des Configuración del motor
             */
            void OnInit(const MotorConfig& des) override;
            
            /**
             * @brief Actualiza el inspector cada frame.
             * @param dt Delta time en segundos
             */
            void OnUpdate(float dt) override;
            
            /**
             * @brief Renderiza el inspector en ImGui.
             */
            void OnRender(void) override;
            
            /**
             * @brief Establece qué objeto editar.
             * @param ref Puntero al objeto a editar (nullptr para deseleccionar)
             */
            void seleccionarObjeto(Objeto* ref);
            
            /**
             * @brief Establece qué tilemap editar.
             * @param ref Puntero al tilemap a editar (nullptr para deseleccionar)
             */
            void seleccionarTileMap(IVJ::TileMap *ref);
            
        private:

            //lab9 colisiones
            bool panelStats();
            /**
             * @brief Renderiza panel de edición de transformación.
             * @return true si se realizaron cambios
             */
            bool panelTransformada();
            
            /**
             * @brief Renderiza panel de edición de sprite.
             * @param sprite Puntero al componente sprite a editar
             * @return true si se realizaron cambios
             */
            bool panelSprite(ISprite *sprite);
            
            /**
             * @brief Renderiza panel de edición de shaders.
             * @param shader Puntero al componente shader a editar
             * @return true si se realizaron cambios
             */
            bool panelShaders(IShader *shader);
            
            /**
             * @brief Renderiza panel de edición de controles.
             * @param control Puntero al componente control a editar
             * @return true si se realizaron cambios
             */
            bool panelIControl(IControl *control);
            
            /**
             * @brief Renderiza panel de edición de trayectorias.
             * @param path Puntero al componente paths a editar
             * @return true si se realizaron cambios
             */
            bool panelIPaths(IPaths *path);

            /**
             * @brief Renderiza panel de Maquina de Estado.
             * @param path Puntero al componente IMaquinaEstado a mostrar
             * @return true si se muestra
             */
            bool panelIMaquinaEstado(IVJ::IMaquinaEstado *me);
            
        private:
            /** @brief Puntero al objeto siendo editado */
            Objeto *objeto_select{nullptr};
            
            /** @brief Editor de paths delegado */
            GPaths *guipath{nullptr};
            
            /** @brief Puntero al tilemap siendo editado */
            IVJ::TileMap *tilemap_select{nullptr};
            
            /** @brief Versión mayor de OpenGL del sistema */
            int m_openglMajor{};
            
            /** @brief Versión menor de OpenGL del sistema */
            int m_openglMinor{};
    };
}
