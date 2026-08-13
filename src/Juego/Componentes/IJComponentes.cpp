#include "../../Motor/Primitivos/GestorAssets.hpp"
#include "../../Motor/Render/Render.hpp"
#include "IJComponentes.hpp"
#include "Motor/Componentes/IComponentes.hpp"
#include <Juego/Sistemas/Sistemas.hpp>
#include <memory>
#include <string>

namespace IVJ
{

ITriangulo::ITriangulo(float r, float ang) : CE::IComponentes{}, tri_shape{r, 3}, angulo{ang}
{
    tri_shape.setOrigin({r / 2.f, r / 2.f});
}

IMaquinaEstado::IMaquinaEstado()
{
    fsm = nullptr;
}

IRayo::IRayo(CE::Vector2D &pos, float &sig, const float magnitud) : magnitud{magnitud}, dir{sig}, lp1{pos}, lp2{0.f}
{
}

CE::Vector2D &IRayo::getP1() const
{
    return lp1;
}

CE::Vector2D &IRayo::getP2()
{
    // int direction = (dir > 0) ? 1 :(dir<0) ? -1 : 0;
    int direction = (dir > 0) ? 1 : -1;
    lp2.x = lp1.x + direction * magnitud;
    lp2.y = lp1.y;
    return lp2;
}

// Interactuables
IDialogo::IDialogo() : IInteractuable(), texto{L""}, id_texto{0}
{
    texto = L"Texto de Prueba ABC ABC, se debe cargar por el arbol, al precionar una tecla"
            L"se avanza al siguiente texto del dialogo, esto es solo una demostración."
            L" El texto debe tener saltos de linea automáticos para no salirse de la caja"
            L"  de dialogo que se renderiza en pantalla."
            L"Espero que funcione correctamente y se vea bien en pantalla.";
}
void IDialogo::onInteractuar(CE::Objeto &obj)
{
    (void)obj;
    // fetch el dialogo del árbol
    if (!activo)
        return;
    // auto control = obj.getComponente<CE::IControl>();
    // control->setActivo(false);
}

std::wstring IDialogo::agregarSaltoLinea(const std::wstring &str, size_t max_len)
{
    std::wstring resultado;
    size_t pos = 0;
    while (pos < str.size())
    {
        size_t chunk_size = std::min(max_len, str.size() - pos);
        resultado += str.substr(pos, chunk_size); // extraer un fragmento
        pos += chunk_size;
        if (pos < str.size())
            resultado += L"\n";
    }
    return resultado;
}
void IDialogo::onRender()
{
    if (!activo)
        return;
    auto dim = CE::Render::Get().GetVentana().getSize();
    sf::RectangleShape fondo;
    fondo.setSize({dim.x * 0.7f, 150.f});
    fondo.setFillColor({0, 0, 0, 150});
    fondo.setPosition({(dim.x / 3.f) - 200.f, 120.f});
    sf::Font font{CE::GestorAssets::Get().getFont("nova")};
    std::wstring texto_con_saltos = agregarSaltoLinea(texto, 76);
    sf::Text texto_render{font, texto_con_saltos, 20};
    texto_render.setPosition({(dim.x / 3.f) - 180.f, 130.f});
    CE::Render::Get().AddToDraw(fondo);
    CE::Render::Get().AddToDraw(texto_render);
    // renderizar el dialogo
}

IIndicador::IIndicador(const sf::Texture &ref, float escala, Entidad *p, const sf::Color &c)
    : IInteractuable(), sprite{ref}, color{c}, escala{escala}
{
    sprite.setScale({escala, escala});
    auto dim = sprite.getTextureRect().size;
    sprite.setOrigin({dim.x / 2.f, dim.y / 2.f});
    parent = p;
    if (!shader.loadFromFile(ASSETS "/shaders/prueba.frag", sf::Shader::Type::Fragment))
        exit(1);

    // blob
    shader.setUniform("blob", CE::GestorAssets::Get().getTextura("blob"));
    shader.setUniform("mask_color", sf::Glsl::Vec4(color));
}
void IIndicador::onInteractuar(CE::Objeto &obj)
{
    (void)obj;
}
void IIndicador::onRender()
{
    if (!activo)
        return;
    auto pos = parent->getTransformada()->posicion;
    auto sd = parent->getComponente<CE::ISprite>()->m_sprite.getGlobalBounds().size;
    pos = pos + CE::Vector2D{0, -sd.y};
    sprite.setPosition({pos.x, pos.y});
    sf::RenderStates estados;
    estados.shader = &shader;
    CE::Render::Get().AddToDraw(sprite, estados);
}




} // namespace IVJ
