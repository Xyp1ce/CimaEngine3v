#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>

namespace CE
{
    /**
     * @class Vector2D
     * @brief Clase para aritmética y geometría 2D.
     * 
     * Encapsula un vector 2D con operaciones comunes como magnitud, normalización,
     * distancia y operadores algebraicos. Se integra con SFML permitiendo conversión
     * desde/hacia sf::Vector2f.
     * 
     * @note Coordenadas públicas (x, y) para acceso directo
     */
    class Vector2D
    {   
        public:
            /**
             * @brief Constructor por defecto (0, 0).
             */
            Vector2D();
            
            /**
             * @brief Constructor con coordenadas explícitas.
             * @param x Coordenada X
             * @param y Coordenada Y
             */
            Vector2D(const float x, const float y);
            
            /**
             * @brief Constructor de valor uniforme.
             * @param v Valor para ambas coordenadas (x=v, y=v)
             */
            Vector2D(const float v);
            
            /**
             * @brief Constructor desde vector SFML.
             * @param cpy Vector sf::Vector2f a copiar
             */
            Vector2D(const sf::Vector2f& cpy);

            /**
             * @brief Calcula la longitud/magnitud del vector.
             * @return Magnitud como float (sqrt(x² + y²))
             */
            float longitud() const;
            
            /**
             * @brief Alias para longitud() - obtiene magnitud.
             * @return Magnitud del vector
             */
            float magnitud() const;
            
            /**
             * @brief Calcula distancia hacia otro vector.
             * @param o Otro vector
             * @return Distancia euclidiana entre los puntos
             */
            float distancia(const Vector2D& o) const;
            
            /**
             * @brief Normaliza el vector a magnitud 1.
             * @return Referencia a este vector modificado (encadenamiento)
             */
            Vector2D& normalizacion();
            
            /**
             * @brief Escala el vector por un factor.
             * @param s Factor de escala
             * @return Referencia a este vector modificado (encadenamiento)
             */
            Vector2D& escala(float s);
            
            /**
             * @brief Suma otro vector al presente.
             * @param o Vector a sumar
             * @return Referencia a este vector modificado (encadenamiento)
             */
            Vector2D& suma(const Vector2D& o);
            
            /**
             * @brief Resta otro vector del presente.
             * @param o Vector a restar
             * @return Referencia a este vector modificado (encadenamiento)
             */
            Vector2D& resta(const Vector2D& o);

            /**
             * @brief Compara igualdad entre vectores.
             * @param v Vector a comparar
             * @return true si x e y son iguales
             */
            bool operator==(const Vector2D& v)const;
            
            /**
             * @brief Suma de vectores (crea nuevo vector).
             * @param v Vector a sumar
             * @return Nuevo vector resultado de la suma
             */
            Vector2D operator+(const Vector2D& v)const;
            
            /**
             * @brief Resta de vectores (crea nuevo vector).
             * @param v Vector a restar
             * @return Nuevo vector resultado de la resta
             */
            Vector2D operator-(const Vector2D& v)const;
            
            /**
             * @brief Suma en lugar (modificación en situ).
             * @param v Vector a sumar
             */
            void operator+=(const Vector2D& v);
            
            /**
             * @brief Salida de stream para impresión.
             * @param os Stream de salida
             * @param vec Vector a imprimir
             * @return Referencia al stream
             */
            friend std::ostream& operator<<(std::ostream& os, const Vector2D& vec);
            
        public:
            /** @brief Coordenada X */
            float x;
            
            /** @brief Coordenada Y */
            float y;
    };
}
