#include "Botones.hpp"

namespace CE
{
    Botones::Botones(const std::string& nombre, const TipoAccion& tipo, const sf::Keyboard::Scancode& scan)
    :nombre{nombre},accion{tipo},tecla{scan}{}
    const std::string Botones::getNombre() const
    {
        return nombre;
    }
    const std::string Botones::getTipoString() const
    {
        switch(accion)
        {
            case TipoAccion::OnPress:
                return "OnPress";
            case TipoAccion::OnRelease:
                return "OnRelease";
            default:
                return "None";
        }
    }
    const sf::Keyboard::Scancode& Botones::getScancode() const
    {
        return tecla;
    }
    const Botones::TipoAccion& Botones::getTipo() const
    {
        return accion;
    }
    const std::string Botones::toString() const
    {
        return nombre + "--> "+getTipoString();  
    }
}
