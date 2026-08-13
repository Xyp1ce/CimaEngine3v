#include "Vector2D.hpp"
#include <cmath>

namespace CE
{

    Vector2D::Vector2D()
        :x{0},y{0}{}
    Vector2D::Vector2D(const float x, const float y)
        :x{x},y{y}{}
    Vector2D::Vector2D(const float v)
        :x{v},y{v}{}
    Vector2D::Vector2D(const sf::Vector2f& cpy)
        :x{cpy.x},y{cpy.y}{}
    
    //métodos
    float Vector2D::longitud() const
    {
        return sqrt((x*x)+(y*y));
    }
    float Vector2D::magnitud() const
    {
        return this->longitud();
    }
    float Vector2D::distancia(const Vector2D& o) const
    {
        return sqrt(
                pow(o.x-x,2)+
                pow(o.y-y,2));
    }


    Vector2D& Vector2D::normalizacion()
    {
        float mag = longitud();
        x/=mag;
        y/=mag;
        return *this;
    }
    Vector2D& Vector2D::escala(float s)
    {
        x*=s;
        y*=s;
        return *this;
    }
    Vector2D& Vector2D::suma(const Vector2D& o)
    {
        x+=o.x;
        y+=o.y;
        return *this;
    }
    Vector2D& Vector2D::resta(const Vector2D& o)
    {
        x-=o.x;
        y-=o.y;
        return *this;
    }
    bool Vector2D::operator==(const Vector2D& v)const
    {
        return (x==v.x) && (y==v.y);
    }
    Vector2D Vector2D::operator+(const Vector2D& v)const
    {
        return Vector2D{x+v.x,y+v.y};
    }
    Vector2D Vector2D::operator-(const Vector2D& v)const
    {
        return Vector2D{x-v.x,y-v.y};
    }
    void Vector2D::operator+=(const Vector2D& v)
    {
        x+=v.x;
        y+=v.y;
    }
    std::ostream& operator<<(std::ostream& os, const Vector2D& vec)
    {
        os<<"("<<vec.x<<", "<<vec.y<<") ";
        return os;
    }

}
