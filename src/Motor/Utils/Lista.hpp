#pragma once

#include <memory>
#include "CEInterfaces.hpp"
#include "Nodo.hpp"

namespace CE
{
    /**
     * @class Lista
     * @brief Lista doblemente enlazada template con patrón iterador.
     * 
     * Implementa una lista enlazada genérica que actúa como pila/cola en 
     * operaciones de eliminación (pop) pero como lista en inserción (append).
     * Implementa los patrones IIterador e IManipularDatos.
     * 
     * Características:
     * - Inserción en O(1) al final
     * - Eliminación en O(1) desde inicio/final
     * - Eliminación en O(n) desde posición arbitraria
     * - Iteración bidireccional
     * - Gestión automática de memoria con shared_ptr
     * 
     * @tparam T Tipo de elementos almacenados
     */
    template<typename T> class Lista : public IIterador<T>, public IManipularDatos<T>
    {
        public:
            /**
             * @brief Constructor por defecto - crea lista vacía.
             */
            explicit Lista()
                :m_inicio{nullptr},m_final{nullptr},m_current{nullptr},size{0}
            {};
            
            /**
             * @brief Mueve iterador al inicio y retorna puntero al dato.
             * @return Puntero al dato del primer nodo
             */
            T* iterInicio(); 
            
            /**
             * @brief Mueve iterador al final y retorna puntero al dato.
             * @return Puntero al dato del último nodo
             */
            T* iterFinal(); 
            
            /**
             * @brief Retorna puntero al dato actual sin mover iterador.
             * @return Puntero al dato del nodo actual
             */
            T* iterActual(); 
            
            /**
             * @brief Avanza el iterador al siguiente nodo.
             * @return Puntero al dato del siguiente nodo
             */
            T* iterSiguiente(); 
            
            /**
             * @brief Verifica si iteración ha terminado.
             * @return true si el iterador es nullptr, false en caso contrario
             */
            bool iterTermino() const;

            /**
             * @brief Agrega un elemento al final de la lista.
             * @param d Referencia constante al elemento a agregar
             */
            void agregarDato(const T& d);
            
            /**
             * @brief Elimina elemento en posición específica.
             * @param id Índice del elemento (0 = inicio, size-1 = final)
             * @return Copia del elemento eliminado
             */
            T eliminarDato(int id);
            
            /**
             * @brief Elimina y retorna el último elemento.
             * @return Copia del elemento del final removido
             */
            T popDato(); 
            
            /**
             * @brief Elimina y retorna el primer elemento.
             * @return Copia del elemento del inicio removido
             */
            T popFrontDato(); 

            /**
             * @brief Obtiene el número de elementos en la lista.
             * @return Tamaño actual de la lista
             */
            unsigned int getSize() const {return size;};

        private:
            /** @brief Puntero al primer nodo */
            std::shared_ptr<Nodo<T>> m_inicio;
            
            /** @brief Puntero al último nodo */
            std::shared_ptr<Nodo<T>> m_final;
            
            /** @brief Puntero al nodo actual para iteración */
            std::shared_ptr<Nodo<T>> m_current;
            
            /** @brief Contador de elementos */
            unsigned int size;

    };
    
    /**
     * @brief Implementación de agregarDato - Agrega dato al final (O(1)).
     */
    template<typename T> void Lista<T>::agregarDato(const T& d)
    {
        //como el constructor es protegido no se puede usar
        //make_shared(es un componente) no te lo permite pero
        //lo que vamos hacer es instanciar un shared_ptr con un rawpointer
        auto nuevo = std::shared_ptr<Nodo<T>>(new Nodo<T>(d));
        if(m_inicio == nullptr)
            m_inicio = nuevo;
        else
        {
            m_final->m_next = nuevo;
            nuevo->m_prev = m_final;
        }
        m_final = nuevo;
        size++;
    }
    
    /**
     * @brief Implementación de eliminarDato - Elimina en posición id (O(n)).
     */
    template<typename T> T Lista<T>::eliminarDato(int id)
    {
        //buscar el dato por posicion
        if(id == 0)
            return popFrontDato();
        else if(id== (int)(size-1))
            return popDato();
        //buscar
        auto temp = m_inicio;
        for(int i=0;i<id;++i)
            temp=temp->m_next;

        auto d = temp->dato;
        
        temp->m_prev->m_next = temp->m_next;
        temp->m_next->m_prev = temp->m_prev;
        temp = nullptr;
        size--;
        
        return d;
    }
    
    /**
     * @brief Implementación de popDato - Elimina del final (O(1)).
     */
    template<typename T> T Lista<T>::popDato()
    {
        auto d = m_final->dato;
        size--;
        if(m_final == m_inicio)
        {
            m_final = nullptr;
            m_inicio= nullptr;
            return d;
        }
        m_final = m_final->m_prev;
        m_final->m_next = nullptr;
        return d;
    }
    
    /**
     * @brief Implementación de popFrontDato - Elimina del inicio (O(1)).
     */
    template<typename T> T Lista<T>::popFrontDato()
    {
        auto d = m_inicio->dato;
        size--;
        if(m_final == m_inicio)
        {
            m_final = nullptr;
            m_inicio= nullptr;
            return d;
        }
        m_inicio = m_inicio->m_next;
        m_inicio->m_prev = nullptr;
        return d;
    }
    
    /**
     * @brief Implementación de iterInicio.
     */
    template<typename T> T* Lista<T>::iterInicio()
    {
        m_current = m_inicio;
        return &m_inicio->dato;
    }
    
    /**
     * @brief Implementación de iterFinal.
     */
    template<typename T> T* Lista<T>::iterFinal()
    {
        m_current = m_final;
        return &m_final->dato;
    }
    
    /**
     * @brief Implementación de iterActual.
     */
    template<typename T> T* Lista<T>::iterActual()
    {
        return &m_current->dato;
    }
    
    /**
     * @brief Implementación de iterSiguiente.
     */
    template<typename T> T* Lista<T>::iterSiguiente()
    {
        if(!m_current) return nullptr;
        m_current = m_current->m_next;
        return &m_current->dato;
    }
    
    /**
     * @brief Implementación de iterTermino.
     */
    template<typename T> bool Lista<T>::iterTermino() const
    {
        return m_current == nullptr;
    }
}
