#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include<map>
#include<memory>
#include <string>
#include "../Utils/Vector2D.hpp"

namespace CE
{
    /**
     * @class GestorAssets
     * @brief Gestor singleton de recursos (fuentes, texturas, sonidos, música).
     * 
     * Implementa un cargador y caché de recursos multimedia del juego.
     * Utiliza punteros compartidos para gestión automática de memoria y almacena
     * recursos por clave textual para acceso fácil.
     * 
     * Soporta:
     * - Fuentes de texto (TrueType)
     * - Texturas de imágenes (PNG, JPG, BMP, etc)
     * - Sonidos cortos (WAV, OGG, FLAC)
     * - Música con loop automático (WAV, OGG, FLAC)
     * 
     * @note Patrón Singleton - instancia única a través de Get()
     */
    class GestorAssets
    {
        public:
            /**
             * @brief Obtiene la instancia singleton del gestor de assets.
             * @return Referencia mutable al gestor único
             */
            static GestorAssets& Get();
            
            /**
             * @brief Carga una fuente de archivo.
             * @param key Identificador único para acceso posterior
             * @param filepath Ruta del archivo de fuente (.ttf)
             */
            void agregarFont(const std::string& key, const std::string& filepath);
            
            /**
             * @brief Carga una textura de archivo.
             * @param key Identificador único para acceso posterior
             * @param filepath Ruta del archivo de imagen
             * @param pos_init Posición inicial (no usada actualmente)
             * @param dim Dimensión (no usada actualmente)
             */
            void agregarTextura(const std::string& key, const std::string& filepath,
                                const CE::Vector2D& pos_init,const CE::Vector2D& dim);
            
            /**
             * @brief Carga un sonido de archivo.
             * @param key Identificador único para acceso posterior
             * @param filepath Ruta del archivo de audio (WAV, OGG, FLAC)
             */
            void agregarSonido(const std::string& key, const std::string& filepath);
            
            /**
             * @brief Carga música de archivo.
             * @param key Identificador único para acceso posterior
             * @param filepath Ruta del archivo de música (WAV, OGG, FLAC)
             * 
             * @note La música cargada soporta loop automático
             */
            void agregarMusica(const std::string& key, const std::string& filepath);
            
            /**
             * @brief Obtiene una fuente por clave.
             * @param key Identificador de la fuente
             * @return Referencia mutable a la fuente SFML
             */
            sf::Font& getFont(const std::string& key)
            {
                return *hashFonts[key];
            }
            
            /**
             * @brief Obtiene una textura por clave.
             * @param key Identificador de la textura
             * @return Referencia mutable a la textura SFML
             */
            sf::Texture& getTextura(const std::string& key)
            {
                return *hashTexturas[key];
            }
            
            /**
             * @brief Obtiene un sonido por clave.
             * @param key Identificador del sonido
             * @return Referencia mutable al sonido SFML
             */
            sf::Sound& getSonido(const std::string& key)
            {
                return *hashSonidos[key];
            }
            
            /**
             * @brief Obtiene música por clave.
             * @param key Identificador de la música
             * @return Referencia mutable a la música SFML (con loop automático)
             */
            sf::Music& getMusica(const std::string& key)
            {
                return *hashMusica[key];
            }

        private:
            /** @brief Caché de fuentes cargadas */
            std::map<std::string, std::shared_ptr<sf::Font>> hashFonts;
            
            /** @brief Caché de texturas cargadas */
            std::map<std::string, std::shared_ptr<sf::Texture>> hashTexturas;
            
            /** @brief Caché de sonidos cargados (WAV, OGG, FLAC) */
            std::map<std::string, std::shared_ptr<sf::Sound>> hashSonidos;
            
            /** @brief Caché de música cargada (soporta loop) */
            std::map<std::string, std::shared_ptr<sf::Music>> hashMusica;
            
            /** @brief Buffer de sonido compartido para carga de efectos */
            sf::SoundBuffer sound_buffer;
            
            /** @brief Instancia singleton del gestor */
            static inline GestorAssets* instancia=nullptr;
    };
}
