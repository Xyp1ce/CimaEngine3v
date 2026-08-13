#pragma once
#include "Vector2D.hpp"

namespace CE
{
    /**
     * @brief Interpolación lineal entre dos vectores.
     * 
     * Realiza una interpolación lineal suave entre dos puntos 2D.
     * Comúnmente usada para animaciones y movimientos suaves.
     * 
     * @param init Vector inicial
     * @param dest Vector destino
     * @param t Factor de interpolación (0.0 = inicio, 1.0 = destino)
     * @return Vector interpolado
     * 
     * @note Formula: resultado = init + (dest - init) * t
     */
    Vector2D lerp(const Vector2D& init, const Vector2D& dest, float t);
    
    /**
     * @brief Interpolación cuadrática de Bézier entre tres puntos.
     * 
     * Realiza una interpolación quadrática de Bézier entre dos puntos
     * usando un punto de control central para curvar el camino.
     * 
     * @param init Primer punto (inicio)
     * @param ctrl Punto de control central (define curvatura)
     * @param dest Tercer punto (destino)
     * @param t Factor de interpolación (0.0 = inicio, 1.0 = destino)
     * @return Punto interpolado en la curva de Bézier
     * 
     * @note Formula: B(t) = (1-t)²*P0 + 2*(1-t)*t*P1 + t²*P2
     * @note Useful para crear trayectorias curvas suaves
     */
    Vector2D lerp2(const Vector2D& init, const Vector2D& ctrl, const Vector2D& dest, float t);
}
