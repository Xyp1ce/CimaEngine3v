#pragma once
#include <memory>

namespace CE
{
    /**
     * @class Nodo
     * @brief Nodo para lista doblemente enlazada.
     * 
     * Estructura que representa un nodo en una lista enlazada con punteros
     * previo y siguiente. Tiene constructor protegido para ser usado únicamente
     * por la clase amiga Lista<T>.
     * 
     * @tparam T Tipo de datos almacenados en el nodo
     * 
     * @note El constructor es protegido - solo Lista<T> puede crear nodos
     */
    template<typename T> class Lista;

    template<typename T> class Nodo
    {
        //nadie puede instanciar la clase Nodo solo clases amigables
        friend class Lista<T>;
        
        //para poder usar el constructor en la clase amigables
        protected:
            /**
             * @brief Constructor protegido (solo amigos pueden crear).
             * @param d Dato a almacenar en el nodo
             */
            Nodo(T d)
                :dato{d}
            {
                m_next = nullptr;
                m_prev = nullptr;
            }
            
        public:
            /** @brief Datos almacenados en el nodo */
            T dato;
            
            /** @brief Puntero al nodo anterior en la lista */
            std::shared_ptr<Nodo<T>> m_prev; 
            
            /** @brief Puntero al nodo siguiente en la lista */
            std::shared_ptr<Nodo<T>> m_next; 
            
            /** @brief Campo de tamaño (posiblemente sin usar) */
            unsigned int size;      
    };
}
