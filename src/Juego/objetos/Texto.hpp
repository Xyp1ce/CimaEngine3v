#pragma once

#include "../../Motor/Primitivos/Objetos.hpp"
#include "../../Motor/Componentes/IComponentes.hpp"

namespace IVJ
{
    class Texto : public CE::Objeto
    {
        public:
            Texto(const sf::Font& f, const std::string &t);
            void onUpdate(float dt) override;
            void draw(sf::RenderTarget& target, sf::RenderStates state) const override;
            void setFontSize(unsigned int size)
            {
                font_size = size;
                texto.m_texto.setCharacterSize(font_size);
            }
            void setColor(const sf::Color &color)
            {
                texto.m_texto.setFillColor(color);
            };
        private:
            CE::ITexto texto;
            unsigned int font_size{60u};
    };
}
