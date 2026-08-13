#include "Lerp.hpp"

namespace CE
{
    Vector2D lerp(const Vector2D& init, const Vector2D& dest, float t)
    {
        float tm1 = (1-t);
        return Vector2D{
            tm1*init.x+t*dest.x,
            tm1*init.y+t*dest.y};
    }
    Vector2D lerp2(const Vector2D& init, const Vector2D& ctrl, const Vector2D& dest, float t)
    {
        float tm1 = (1-t);
        float tm12 = tm1*tm1;
        float t2 = t*t;
        return Vector2D{
            tm12*init.x+2*tm1*t*ctrl.x+t2*dest.x,
            tm12*init.y+2*tm1*t*ctrl.y+t2*dest.y
        };
    }
}
