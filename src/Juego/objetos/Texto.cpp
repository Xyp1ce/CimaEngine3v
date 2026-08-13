#include "Texto.hpp"
namespace IVJ
{
    
    Texto::Texto(const sf::Font& f, const std::string &t)
        :CE::Objeto{},texto{CE::ITexto(f,t)}
    {
        texto.m_texto.setCharacterSize(font_size);
        texto.m_texto.setFillColor(sf::Color::Red);
        texto.m_texto.setStyle(sf::Text::Style::Bold);
    }
    void Texto::onUpdate(float dt)
    {
        (void)dt;
        auto pos  = getTransformada()->posicion;
        texto.m_texto.setPosition({pos.x,pos.y});
        texto.m_texto.setCharacterSize(font_size);
    }
    void Texto::draw(sf::RenderTarget& target, sf::RenderStates state) const
    {
        state.transform *= getTransform();
        
        target.draw(texto.m_texto);

    }
}
