#pragma once
#include "App/ALayer.hpp"
#include "GUI/GLayer.hpp"
#include "Utils/Utils.hpp"
#include<memory>
namespace CE
{
    /**
     * @class Motor
     * @brief Controlador principal del motor de juego que orquesta el ciclo de juego.
     * 
     * Esta clase es responsable de:
     * - Inicializar el motor con la configuración proporcionada
     * - Ejecutar el bucle principal de juego (game loop)
     * - Procesar eventos de entrada del usuario
     * - Actualizar la lógica del juego
     * - Renderizar la escena actual
     * - Gestionar las capas de interfaz gráfica (GUI)
     * 
     * El motor utiliza un patrón singleton implícito a través de la inyección de
     * dependencias, permitiendo múltiples instancias pero típicamente una sola activa.
     */
    class Motor
    {
        public:
            /**
             * @brief Constructor del motor.
             * @param config Configuración del motor (dimensiones de ventana, título, delta time)
             * @param ref Puntero único a la capa de juego que gestiona la lógica principal
             */
            Motor(const MotorConfig& config,std::unique_ptr<GameLayer> ref);
            
            /**
             * @brief Inicia la ejecución del bucle principal del motor.
             * @return Código de salida de la aplicación (0 = éxito)
             * 
             * Este método contiene el bucle principal del juego que:
             * 1. Procesa eventos de entrada
             * 2. Actualiza la lógica del juego
             * 3. Renderiza la escena
             * 4. Actualiza capas GUI
             */
            int OnRun(void);
        private:
            /**
             * @brief Inicializa los componentes del motor.
             * 
             * Llama a la inicialización de la capa de juego y prepara el sistema de renderizado.
             */
            void OnInit(void);
            
            /**
             * @brief Procesa todos los eventos de entrada (teclado, ratón, ventana, etc).
             * 
             * Captura eventos del sistema operativo y los almacena en el buffer de entrada.
             */
            void OnEventFrame();
            
            /**
             * @brief Actualiza la lógica del juego.
             * @param dt Delta time (tiempo transcurrido desde el último frame en segundos)
             * 
             * Actualiza la capa de juego y todos los sistemas del motor.
             */
            void OnUpdateFrame(float dt);
            
            /**
             * @brief Renderiza el frame actual.
             * 
             * Limpia la pantalla y renderiza todos los objetos, efectos y capas GUI.
             */
            void OnRenderFrame();

        private:
            /** @brief Configuración del motor (resolución, título, velocidad) */
            MotorConfig motor_info;
            
            /** @brief Referencia a la capa principal de lógica de juego */
            std::unique_ptr<GameLayer> mi_app;
            
            /** @brief Colección de capas de interfaz gráfica (ImGui) */
            std::vector<std::shared_ptr<GLayer>> gui_layers;
            
            /** @brief Buffer circular para almacenar eventos de entrada (capacidad: 32 eventos) */
            InputBuffer inputbuffer{32};

    };

}
