#pragma once
#include "GLayer.hpp"
#include "GPropiedades.hpp"

namespace CE
{
    /**
     * @class GListaObjetos
     * @brief Panel de ImGui para listar y seleccionar objetos de la escena.
     * 
     * Renderiza una lista de todos los objetos en el pool actual permitiendo
     * seleccionar uno para editar sus propiedades. Integra el inspector de
     * propiedades (GPropiedades) para edición en tiempo real.
     * 
     * @note Derivada de GLayer para integración en sistema GUI del motor
     * @note Contiene instancia embedded de GPropiedades para edición
     */
    class GListaObjetos : public GLayer
    {
        public:
            /** @brief Destructor virtual */
            ~GListaObjetos() override {};

            /**
             * @brief Inicializa el panel de lista de objetos.
             * @param des Configuración del motor
             */
            void OnInit(const MotorConfig& des)override;
            
            /**
             * @brief Actualiza el panel cada frame.
             * @param dt Delta time en segundos
             */
            void OnUpdate(float dt) override;
            
            /**
             * @brief Renderiza la lista de objetos y propiedades en ImGui.
             */
            void OnRender(void) override;
            
        private:
            /** @brief Inspector de propiedades embebido para edición */
            GPropiedades propiedades;
    };
}
