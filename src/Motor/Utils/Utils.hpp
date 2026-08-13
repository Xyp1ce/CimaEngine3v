#pragma once
#include<string>

namespace CE
{
    /**
     * @struct MotorConfig
     * @brief Estructura de configuración para inicialización del motor.
     * 
     * Almacena los parámetros de configuración global del motor de juego
     * incluyendo dimensiones de ventana, título y velocidad de actualización.
     */
    struct MotorConfig
    {
        /** @brief Ancho de la ventana/viewport en píxeles (defecto: 0) */
        unsigned int vW=0;
        
        /** @brief Alto de la ventana/viewport en píxeles (defecto: 0) */
        unsigned int vH=0;
        
        /** @brief Título de la ventana (defecto: "") */
        std::string titulo="";
        
        /** @brief Delta time / intervalo de tiempo del motor en segundos (defecto: 0) */
        float motor_dt=0;
    };
}
