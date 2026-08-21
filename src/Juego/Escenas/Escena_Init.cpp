#include "Escena_Init.hpp"
#include <Motor/Inputs/Botones.hpp>
#include <Motor/Primitivos/GestorAssets.hpp>
#include <Motor/Primitivos/GestorEscenas.hpp>
#include <Motor/Render/Render.hpp>
#include <Juego/Figuras/Figuras.hpp>
#include <Juego/objetos/Texto.hpp>
#include <Juego/Componentes/IJComponentes.hpp>
#include <Juego/Sistemas/Sistemas.hpp>
#include <memory>
#include <Juego/Figuras/ClasePrueba.hpp>
namespace IVJ
{
    Escena_Init::Escena_Init()
        :CE::Escena{}
    {
    }
    void Escena_Init::onInit()
    {
        if(!inicializar) return;
        std::cout<<"Variable apuntador inteligente"<<"\n";
        std::shared_ptr<Alumno> al = std::make_shared<Alumno>("Ramon", 2210376, 6);
        std::cout << al->getNombre() << "\n";

        auto rec = std::make_shared<Rectangulo>(200.f, 150.f,sf::Color::Red, sf::Color{255,255,0});
        auto circ = std::make_shared<Circulo>(50.f, sf::Color::Blue, sf::Color::Green);
        auto pen = std::make_shared<Pentagono>(150.f, sf::Color::Green, sf::Color::Black);

        rec->setPosicion(500,360);
        circ->setPosicion(320,320);
        pen->setPosicion(250,250);

        objetos.agregarPool(rec);
        objetos.agregarPool(circ);
        objetos.agregarPool(pen);

        inicializar=false;
    }
    void Escena_Init::onFinal()
    {
    }
    void Escena_Init::onUpdate(float dt)
    {
        for(auto& obj: objetos.getPool())
        {
            obj->onUpdate(dt);
        }
    }
    void Escena_Init::onInputs(const CE::Botones& accion)
    {
        switch(accion.getTipo())
        {
            case CE::Botones::TipoAccion::OnPress:
            {   
                break;
            }
            case CE::Botones::TipoAccion::OnRelease:
            {
                break;
            }
            case CE::Botones::TipoAccion::None:
            {
                break;
            }
        }
    }
    void Escena_Init::onRender()
    {
        for(auto& obj: objetos.getPool())
            CE::Render::Get().AddToDraw(*obj);

    }
}
