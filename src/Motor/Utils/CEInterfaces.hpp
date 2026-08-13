#pragma once

namespace CE
{
    /**
     * @class IIterador
     * @brief Interfaz template para iteración sobre contenedores.
     * 
     * Define los métodos necesarios para navegar sobre una colección de elementos
     * de tipo genérico T, soportando movimiento hacia inicio, final, actual y siguiente.
     * 
     * @tparam T Tipo de datos de los elementos contenidos
     */
    template<typename T> class IIterador
    {
        public:
            /**
             * @brief Mueve el iterador al primer elemento.
             * @return Puntero al primer elemento o nullptr si está vacío
             */
            virtual T* iterInicio()=0; 
            
            /**
             * @brief Mueve el iterador al último elemento.
             * @return Puntero al último elemento o nullptr si está vacío
             */
            virtual T* iterFinal()=0; 
            
            /**
             * @brief Obtiene el elemento actual sin mover.
             * @return Puntero al elemento actual o nullptr
             */
            virtual T* iterActual()=0; 
            
            /**
             * @brief Avanza el iterador al siguiente elemento.
             * @return Puntero al siguiente elemento o nullptr si es el final
             */
            virtual T* iterSiguiente()=0; 
            
            /**
             * @brief Verifica si la iteración ha terminado.
             * @return true si se ha alcanzado el final, false en caso contrario
             */
            virtual bool iterTermino() const = 0;
    };

    /**
     * @class IManipularDatos
     * @brief Interfaz template para manipulación de datos en contenedores.
     * 
     * Define operaciones comunes para agregar y eliminar elementos de una colección,
     * permitiendo eliminación por índice, desde el final (pop) y desde el inicio (pop front).
     * 
     * @tparam T Tipo de datos de los elementos contenidos
     */
    template<typename T> class IManipularDatos
    {
        public:
            /**
             * @brief Agrega un elemento al contenedor.
             * @param d Referencia constante al elemento a agregar
             */
            virtual void agregarDato(const T& d)=0;
            
            /**
             * @brief Elimina un elemento por su índice.
             * @param id Índice del elemento a eliminar
             * @return Copia del elemento eliminado
             */
            virtual T eliminarDato(int id)=0;
            
            /**
             * @brief Elimina y retorna el elemento del final.
             * @return Copia del elemento del final removido
             */
            virtual T popDato()=0; 
            
            /**
             * @brief Elimina y retorna el elemento del inicio (pop front).
             * @return Copia del elemento del inicio removido
             */
            virtual T popFrontDato()=0; 
    };
}
