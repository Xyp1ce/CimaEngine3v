#pragma once
#include "../GUI/GLayer.hpp"
#include "../Utils/Utils.hpp"
#include "../Primitivos/Objetos.hpp"

namespace CE
{
    /**
     * @class GPaths
     * @brief Panel de ImGui para editar trayectorias/paths de entidades.
     * 
     * Proporciona interfaz de edición visual para componentes IPaths,
     * permitiendo crear y modificar curvas Bézier para que los objetos
     * sigan trayectorias predefinidas.
     * 
     * @note Derivada de GLayer para integración en sistema GUI del motor
     * @note Trabajo en tándem con GPropiedades para selección de objetos
     */
    class GPaths : public GLayer
    {
        public:
            /** @brief Destructor virtual */
            ~GPaths() override {};
            
            /**
             * @brief Inicializa el panel de paths.
             * @param des Configuración del motor
             */
            void OnInit(const MotorConfig& des) override;
            
            /**
             * @brief Actualiza el panel de paths cada frame.
             * @param dt Delta time en segundos
             */
            void OnUpdate(float dt) override;
            
            /**
             * @brief Renderiza el editor de paths en ImGui.
             */
            void OnRender(void) override;
            
            /**
             * @brief Establece el objeto cuyas trayectorias se editan.
             * @param ref Puntero al objeto a editar (nullptr para desseleccionar)
             */
            void setObjetoSelect(Objeto *ref);
            
        private:
            /** @brief Puntero al objeto cuyas trayectorias se están editando */
            Objeto* objeto_select{nullptr};
    };
}
