#include "Render.hpp"
#include "../Camaras/Camaras.hpp"
#include "../Camaras/CamarasGestor.hpp"
#include "SFML/Window/ContextSettings.hpp"
#include <imgui.h>

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

        cursor_flecha = sf::Cursor::createFromSystem(sf::Cursor::Type::Arrow);
        if (cursor_flecha) {
            r_ventana->setMouseCursor(*cursor_flecha);
        }
        r_ventana->setMouseCursorVisible(true);

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

    void Render::setMouseCursorVisible(bool visible)
    {
        if (r_ventana)
            r_ventana->setMouseCursorVisible(visible);
    }

    void Render::setViewportRect(float x, float y, float w, float h)
    {
        viewport_x = x;
        viewport_y = y;
        viewport_w = w;
        viewport_h = h;
    }

    bool Render::isMouseInsideViewport() const
    {
        if (viewport_w <= 0.f || viewport_h <= 0.f)
            return true;
        ImVec2 mousePos = ImGui::GetIO().MousePos;
        return (mousePos.x >= viewport_x && mousePos.x <= viewport_x + viewport_w &&
                mousePos.y >= viewport_y && mousePos.y <= viewport_y + viewport_h);
    }

    sf::Vector2f Render::getMouseWorldPos() const
    {
        if (!r_ventana)
            return sf::Vector2f{0.f, 0.f};

        auto &cam = GestorCamaras::Get().getCamaraActiva();
        ImVec2 mousePos = ImGui::GetIO().MousePos;

        if (viewport_w > 0.f && viewport_h > 0.f && r_textura)
        {
            float relX = mousePos.x - viewport_x;
            float relY = mousePos.y - viewport_y;

            float normX = relX / viewport_w;
            float normY = relY / viewport_h;

            float texX = normX * (float)r_textura->getSize().x;
            float texY = normY * (float)r_textura->getSize().y;

            return r_textura->mapPixelToCoords(sf::Vector2i{(int)texX, (int)texY}, cam.getView());
        }

        auto mousePixel = sf::Mouse::getPosition(*r_ventana);
        return r_ventana->mapPixelToCoords(mousePixel, cam.getView());
    }
}
