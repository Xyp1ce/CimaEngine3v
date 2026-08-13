#pragma once
#include "../GUI/GLayer.hpp"
#include "../Utils/Utils.hpp"

namespace CE
{
    /**
     * @class GDock
     * @brief Panel dock de ImGui para mostrar información del motor.
     * 
     * Implementa un panel de información/debug que muestra detalles sobre
     * la configuración del motor (resolución, título, etc).
     * 
     * @note Derivada de GLayer para integración en sistema GUI del motor
     */
    class GDock : public GLayer
    {
        public:
            /** @brief Destructor virtual */
            ~GDock(){};
            
            /**
             * @brief Inicializa el panel dock.
             * @param des Configuración del motor (se almacena para mostrar)
             */
            void OnInit(const MotorConfig& des) override;
            
            /**
             * @brief Actualiza el panel dock cada frame.
             * @param dt Delta time en segundos
             */
            void OnUpdate(float dt) override;
            
            /**
             * @brief Renderiza el panel dock en ImGui.
             */
            void OnRender(void) override;
            
        private:
            /** @brief Configuración del motor almacenada */
            MotorConfig des;
            
            /** @brief Flags de ventana ImGui */
            unsigned int flags;
            
            /** @brief Dimensión X para ImGui */
            unsigned int dx;
            
            /** @brief Dimensión Y para ImGui */
            unsigned int dy;
    };
}
