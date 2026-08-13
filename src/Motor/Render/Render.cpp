#include "Render.hpp"
#include "SFML/Window/ContextSettings.hpp"

namespace CE
{
    Render& Render::Get()
    {
        if(!instancia)
            instancia = new Render();
        return *instancia;
    }

    void Render::Terminar()
    {
        delete instancia;
        //instancia = nullptr;
    }
    void Render::CrearVentana(const MotorConfig& motor_info)
    {
        sf::ContextSettings videosettings;
        videosettings.antiAliasingLevel=3;
        //depende del driver, MacOs = 4.1 , windows/linux 4.6
        //videosettings.majorVersion = 4;
        //videosettings.minorVersion = 1;
        //videosettings.attributeFlags = sf::ContextSettings::Core;

        r_ventana = std::make_unique<sf::RenderWindow>(
                sf::VideoMode({motor_info.vW,motor_info.vH}, 32),
                sf::String(motor_info.titulo),
                sf::Style::Default,
                sf::State::Windowed,
                videosettings);

        r_ventana->setFramerateLimit(65);
        unsigned int tw,th;
#if DEBUG
        tw = motor_info.vW-motor_info.vW*0.25f;
        th = motor_info.vH-motor_info.vH*0.25f;
#else
        tw = motor_info.vW;
        th = motor_info.vH;
#endif

        r_textura=std::make_unique<sf::RenderTexture>(
                sf::Vector2u{tw,th},videosettings);
    }

    void Render::AutoResizeTextura(unsigned int  w, unsigned int h)
    {
        
        auto cs = r_textura->getSize();
        unsigned cw = cs.x;
        unsigned ch = cs.y;
        //std::cout<<w<<"=="<<cw<<" && "<<h<<"=="<<ch<<"\n";
        //no update
        if( (cw==w && ch ==h) || (w==0 || h==0) ) return;

        sf::ContextSettings video;
        video.antiAliasingLevel=3;
        //video.majorVersion = 4;
        //video.minorVersion = 1;
        //video.attributeFlags = sf::ContextSettings::Core;
        r_textura->clear(sf::Color::Black);
        if(!r_textura->resize(sf::Vector2u{w,h},video))
            exit(EXIT_FAILURE);

    }

    void Render::AddToDraw(const sf::Drawable &obj)
    {
        r_textura->draw(obj);
    }

    void Render::AddToDraw(const sf::Drawable &obj, sf::RenderStates shader)
    {
        r_textura->draw(obj,shader);
    }

    void Render::OnClearColor(const sf::Color& clearcolor)
    {
        r_textura->clear(clearcolor);
        r_ventana->clear(clearcolor);
    }

    void Render::OnDisplayTextura(void)
    {
        r_textura->display();
    }

    void Render::OnDisplayVentana(void)
    {
        r_ventana->display();
    }

    sf::RenderWindow& Render::GetVentana(void)
    {
        return *r_ventana;
    }
    sf::RenderTexture& Render::GetTextura(void)
    {
        return *r_textura;
    }
}
