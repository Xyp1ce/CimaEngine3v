#pragma once
#include "../Utils/Utils.hpp"

namespace CE
{
    /**
     * @class GLayer
     * @brief Clase base abstracta para capas de interfaz gráfica ImGui.
     * 
     * Define la interfaz mínima que deben implementar todas las capas GUI
     * del motor. Las capas se apilan en un vector y se actualizan/renderizan
     * secuencialmente cada frame.
     * 
     * Las capas típicas incluyen: dock (información), logger (logs), viewport,
     * propiedades de objetos, lista de objetos, etc.
     */
    class GLayer
    {
        public:
            /** @brief Destructor virtual */
            virtual ~GLayer(){};
            
            /**
             * @brief Inicializa la capa GUI.
             * @param des Configuración del motor (resolución, etc)
             * @note Método virtual puro - debe ser implementado por subclases
             */
            virtual void OnInit(const MotorConfig& des)=0;
            
            /**
             * @brief Actualiza la lógica de la capa.
             * @param dt Delta time en segundos
             * @note Método virtual puro - debe ser implementado por subclases
             */
            virtual void OnUpdate(float dt)=0;
            
            /**
             * @brief Renderiza la capa en ImGui.
             * @note Método virtual puro - debe ser implementado por subclases
             */
            virtual void OnRender(void)=0;
    };
}

