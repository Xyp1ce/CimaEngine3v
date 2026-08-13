#pragma once

#include "../GUI/GLayer.hpp"
#include <imgui.h>

namespace CE
{
    /**
     * @class GViewport
     * @brief Panel de ImGui que muestra información del viewport de juego.
     * 
     * Renderiza un panel mostrando las dimensiones actuales del viewport,
     * útil para debug y verificación de resolución en tiempo real.
     * 
     * @note Derivada de GLayer para integración en sistema GUI del motor
     */
    class GViewport : public GLayer
    {
        public:
            /** @brief Destructor virtual */
            ~GViewport(){};
            
            /**
             * @brief Inicializa el panel viewport.
             * @param des Configuración del motor (se extrae ancho y alto)
             */
            void OnInit(const MotorConfig& des) override;
            
            /**
             * @brief Actualiza el panel viewport cada frame.
             * @param dt Delta time en segundos
             */
            void OnUpdate(float dt) override;
            
            /**
             * @brief Renderiza el panel viewport en ImGui.
             */
            void OnRender(void) override;
            
        private:
            /** @brief Ancho del viewport almacenado */
            unsigned int vw;
            
            /** @brief Alto del viewport almacenado */
            unsigned int vh;
            
            /** @brief Configuración del motor */
            MotorConfig motor_info;
    };
}
