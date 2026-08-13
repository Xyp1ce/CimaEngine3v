#pragma once
#include "GLayer.hpp"
#include "../Utils/Lista.hpp"
#include <utility> //pair
#include <imgui.h>

namespace CE
{
    /**
     * @class GLogger
     * @brief Gestor singleton de logging integrado con ImGui.
     * 
     * Proporciona un sistema de logs en tiempo real con colores por nivel,
     * FPS en vivo, y visualización en panel de ImGui. Los logs se almacenan
     * en una cola (Lista) y se renderizan con colores asignados por nivel.
     * 
     * @note Patrón Singleton - instancia única a través de Get()
     * @note Derivada de GLayer para integración en sistema GUI del motor
     */
    class GLogger : public GLayer
    {
        public:
            /**
             * @enum Niveles
             * @brief Niveles de severidad para los logs.
             */
            enum class Niveles
            {
                LOG=0,          ///< Información general
                LOG_DEBUG,      ///< Información de debug
                LOG_ERROR,      ///< Error no fatal
                LOG_SEVERO      ///< Error crítico
            };

        public:
            /**
             * @brief Obtiene la instancia singleton del logger.
             * @return Referencia mutable al logger único
             */
            static GLogger& Get(void);
            
            /** @brief Destructor virtual */
            virtual ~GLogger() override{};

            /**
             * @brief Inicializa el logger GUI.
             * @param des Configuración del motor
             */
            void OnInit(const MotorConfig& des) override;
            
            /**
             * @brief Actualiza el logger cada frame (calcula FPS).
             * @param dt Delta time en segundos
             */
            void OnUpdate(float dt) override;
            
            /**
             * @brief Renderiza el panel de logs en ImGui.
             */
            void OnRender(void) override;

            /**
             * @brief Agrega un mensaje a la cola de logs.
             * @param log Texto del mensaje a registrar
             * @param n Nivel de severidad del log
             */
            void agregarLog(const std::string& log,Niveles n);
            
        private:
            /**
             * @brief Obtiene el color ImGui para un nivel de log.
             * @param n Nivel de log
             * @return Vector de color RGBA ImGui (ImVec4)
             */
            ImVec4 nivelColores(Niveles n);
            
            /**
             * @brief Convierte nivel de log a cadena de texto.
             * @param n Nivel de log
             * @return Representación textual del nivel
             */
            std::string nivelToString(Niveles n);
            
        private:
            /** @brief Constructor privado (singleton) */
            explicit GLogger();
            
            /** @brief Cola de logs con pares (mensaje, nivel) */
            Lista<std::pair<std::string,Niveles>> queue;
            
            /** @brief Instancia singleton del logger */
            static inline GLogger *instancia = nullptr;
            
            /** @brief String con FPS actual */
            std::string m_sfps;
            
            /** @brief Configuración del motor almacenada */
            MotorConfig m_config;
    };
}
