#pragma once
#include "Escena.hpp"
//#include <cstdarg>


namespace CE
{
    /**
     * @class GestorEscenas
     * @brief Gestor singleton de escenas del juego con transiciones dinámicas.
     * 
     * Administra el registro, almacenamiento y cambio de escenas del juego.
     * Mantiene la escena actual activa y permite transiciones entre escenas
     * con opción de limpiar memoria de la escena anterior.
     * 
     * @note Patrón Singleton - instancia única a través de Get()
     */
    class GestorEscenas
    {
        /** @brief Tipo alias para mapa de escenas: clave → escena */
        typedef std::map<std::string,std::shared_ptr<Escena>> HEscenas;
        
        public:
            /**
             * @brief Obtiene la instancia singleton del gestor de escenas.
             * @return Referencia mutable al gestor único
             */
            static GestorEscenas& Get();
            
            /**
             * @brief Registra una nueva escena en el gestor.
             * @param key Identificador único de la escena
             * @param escena Puntero compartido a la escena a registrar
             * 
             * La escena se almacena pero no se activa hasta llamar a cambiarEscena().
             */
            void registrarEscena(const std::string& key, const std::shared_ptr<Escena>& escena);
            
            /**
             * @brief Cambia a una escena registrada.
             * @param key Identificador de la escena destino
             * 
             * Activa la escena especificada. La escena anterior se mantiene en memoria.
             */
            void cambiarEscena(const std::string& key);
            
            /**
             * @brief Cambia a una escena con opción de limpiar la anterior.
             * @param key Identificador de la escena destino
             * @param eliminar true para liberar la escena anterior, false para mantenerla
             */
            void cambiarEscenayEliminar(const std::string& key, bool eliminar=false);
            
            /**
             * @brief Obtiene la escena actualmente activa.
             * @return Referencia mutable a la escena actual
             */
            Escena& getEscenaActual();
            
            /**
             * @brief Obtiene todas las escenas registradas.
             * @return Referencia mutable al mapa de escenas
             */
            HEscenas& getTodasEscenas()
            {
                return escenas;
            }
            
            /**
             * @brief Obtiene los identificadores de todas las escenas registradas.
             * @return Vector con todas las claves de escenas
             */
            std::vector<std::string> getKeys()const
            {
                std::vector<std::string> keys;
                for(auto it= escenas.begin(); it!=escenas.end();++it)
                    keys.push_back(it->first);
                return keys;
            }
            
        private:
            /** @brief Identificador de la escena actualmente activa */
            std::string escena_actual;
            
            /** @brief Mapa de todas las escenas registradas */
            HEscenas escenas;
            
            /** @brief Instancia singleton del gestor */
            static inline GestorEscenas* instancia=0;
    };
}
