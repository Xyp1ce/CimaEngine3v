#pragma once
#include "Camaras.hpp"
#include <memory>

namespace CE
{
    /**
     * @class GestorCamaras
     * @brief Gestor singleton para administración de múltiples cámaras.
     * 
     * Mantiene una colección de cámaras, designa cuál es la activa y orquesta
     * su actualización y renderizado. Permite cambiar cámaras en tiempo de ejecución.
     * 
     * @note Patrón Singleton - instancia única a través de Get()
     * @note Usa weak_ptr para la cámara activa para permitir eliminación externa
     */
    class GestorCamaras
    {
        public:
            /**
             * @brief Obtiene la instancia singleton del gestor de cámaras.
             * @return Referencia mutable al gestor único
             */
            static GestorCamaras& Get();
            
            /**
             * @brief Agrega una nueva cámara al gestor.
             * @param nueva Puntero compartido a la cámara a agregar
             */
            void agregarCamara(const std::shared_ptr<Camara>& nueva);
            
            /**
             * @brief Establece cuál cámara es la activa.
             * @param id Índice de la cámara en la lista
             */
            void setCamaraActiva(int id);
            
            /**
             * @brief Actualiza todas las cámaras.
             * @param dt Delta time en segundos
             */
            void onUpdateCamaras(float dt);
            
            /**
             * @brief Renderiza en una textura usando la cámara activa.
             * @param tex Referencia a la textura de renderizado
             */
            void onRenderCamara(sf::RenderTexture &tex);
            
            /**
             * @brief Obtiene la cámara actualmente activa.
             * @return Referencia mutable a la cámara activa
             * 
             * @warning Puede lanzar excepción si la cámara activa fue eliminada externamente
             */
            [[nodiscard]]Camara& getCamaraActiva()
            {
                return *m_camaraActiva.lock();
            }
            
            /**
             * @brief Obtiene una cámara por su índice.
             * @param id Índice en la lista de cámaras
             * @return Referencia mutable a la cámara
             */
            [[nodiscard]]Camara& getCamara(int id)
            {
                return *m_listaCamaras[id];
            }
            
            /**
             * @brief Obtiene la lista completa de cámaras.
             * @return Referencia mutable al vector de cámaras
             */
            [[nodiscard]]std::vector<std::shared_ptr<Camara>>& getListaCamaras()
            {
                return m_listaCamaras;
            }
            
        private:
            /** @brief Instancia singleton del gestor */
            static inline GestorCamaras* instancia = nullptr;
            
            /** @brief Referencia débil a la cámara actualmente activa */
            std::weak_ptr<Camara> m_camaraActiva;
            
            /** @brief Lista de todas las cámaras registradas */
            std::vector<std::shared_ptr<Camara>> m_listaCamaras;

    };
}
