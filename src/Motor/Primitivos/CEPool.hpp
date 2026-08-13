#pragma once
#include "Objetos.hpp"

namespace CE
{
    /**
     * @class Pool
     * @brief Gestiona un pool de objetos del juego con tamaño dinámico.
     * 
     * Implementa un contenedor para almacenar y acceder a objetos compartidos.
     * Actúa como un administrador de recursos para una colección de entidades.
     * 
     * @note El tamaño puede exceder max_size dinámicamente (no es una restricción dura)
     */
    class Pool
    {  
        public:
            /**
             * @brief Constructor del pool.
             * @param size Tamaño inicial/máximo sugerido del pool
             */
            Pool(int size);
            
            /**
             * @brief Agrega un objeto al pool.
             * @param obj Puntero compartido al objeto a agregar
             */
            void agregarPool(const std::shared_ptr<Objeto>& obj); 
            
            /**
             * @brief Limpia todos los objetos del pool.
             */
            void borrarPool();
            
            /**
             * @brief Acceso mutable a un objeto por índice.
             * @param idx Índice del objeto
             * @return Referencia mutable al puntero compartido
             */
            std::shared_ptr<Objeto>& operator[](std::size_t idx);
            
            /**
             * @brief Acceso constante a un objeto por índice.
             * @param idx Índice del objeto
             * @return Referencia constante al puntero compartido
             */
            const std::shared_ptr<Objeto>& operator[](std::size_t idx) const;
            
            /**
             * @brief Obtiene la lista completa de objetos.
             * @return Referencia mutable al vector de objetos
             */
            std::vector<std::shared_ptr<Objeto>>& getPool()
            {
                return objetos;
            }
            
        public:
            /** @brief Tamaño máximo sugerido del pool */
            int max_size;
            
        private:
            /** @brief Contenedor de punteros compartidos a objetos */
            std::vector<std::shared_ptr<Objeto>> objetos;
    };

}
