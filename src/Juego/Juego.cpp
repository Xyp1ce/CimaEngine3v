#include "Juego.hpp"
#include<iostream>

#include<SFML/Graphics.hpp>
#include <memory>
#include <string>

#include "../Motor/Primitivos/GestorEscenas.hpp"
#include "../Motor/Primitivos/GestorAssets.hpp"
#include "Motor/Camaras/Camaras.hpp"
#include "Motor/Camaras/CamarasGestor.hpp"


#include <cstdlib>
#include <ctime>
#include <Juego/Escenas/Escena_Init.hpp>
#include <Juego/objetos/Entidad.hpp>



namespace IVJ
{

    Juego::Juego()
        :CE::GameLayer{}
    {
        termino=false;
    }
    void Juego::OnInit(void)
    {
        std::cout<<"Inicializando Juego\n";
    
        
        
        //crear el jugador
        jugador = std::make_shared<Entidad>();
        jugador->getStats()->hp_max = 255;
        jugador->getStats()->hp = 255;
        jugador->getStats()->str = 255;
        jugador->getStats()->def = 255;
        jugador->getStats()->agi = 255;

        CE::GestorEscenas::Get().registrarEscena("Menu",std::make_shared<Escena_Init>());

        CE::GestorEscenas::Get().cambiarEscena("Menu"); //ejecuta onInit()A
        escena_actual = &CE::GestorEscenas::Get().getEscenaActual();

        
    }
    
    void Juego::OnInputs()
    {
        auto br = escena_actual->getBotonesRegistrados();

        CE::Botones::TipoAccion tipo_accion = CE::Botones::TipoAccion::OnRelease;
        std::string strAccion ="None";
        sf::Keyboard::Scancode scan = sf::Keyboard::Scancode::Comma;

        for(auto &sk : br)
        {
            if(sf::Keyboard::isKeyPressed(sk))
            {
                tipo_accion = CE::Botones::TipoAccion::OnPress;
                scan = sk;
                strAccion = escena_actual->getBotones().at(scan);
            }
        }
        escena_actual->onInputs(CE::Botones(strAccion,tipo_accion,scan));
    }
    void Juego::OnInputs(std::optional<sf::Event>& eventos)
    {
        if(eventos->getIf<sf::Event::Closed>())
            termino =true;

        if(eventos->getIf<sf::Event::MouseMoved>())
        {
            
        }

    }
    void Juego::OnInputs(CE::InputBuffer& buffer)
    {
        //procesar la lista circular
        while(!buffer.estaVacio())
        {
            auto evento_ptr = buffer.popData();
            if(!evento_ptr)
                continue;
            auto& evento = *evento_ptr;
            //eventos de la ventana
            if(evento.getIf<sf::Event::Closed>())
            {
                termino=true;
                continue; //o break si no necesitamos procesar más
            }
            //procesar inputs
            sf::Keyboard::Scancode scan = sf::Keyboard::Scancode::Unknown;
            CE::Botones::TipoAccion b_tipo;
            bool procesado=false;

            if(const auto* tecla_presionada = evento.getIf<sf::Event::KeyPressed>())
            {
                scan = tecla_presionada->scancode;
                b_tipo = CE::Botones::TipoAccion::OnPress;
                procesado = true;
            }
            else if(const auto* tecla_liberada = evento.getIf<sf::Event::KeyReleased>())
            {
                scan = tecla_liberada->scancode;
                b_tipo = CE::Botones::TipoAccion::OnRelease;
                procesado = true;
            }

            if(procesado)
            {
                auto iterador = escena_actual->getBotones().find(scan);
                if(iterador!=escena_actual->getBotones().end())
                {
                    //se encontro el input del control
                    std::string strAccion = iterador->second;
                    escena_actual->onInputs(CE::Botones(strAccion,b_tipo,scan));
                }
            }

        }
    }
    void Juego::OnUpdate(float dt)
    {
        escena_actual = &CE::GestorEscenas::Get().getEscenaActual();
        escena_actual->onUpdate(dt);
    }

    void Juego::OnRender()
    {
        escena_actual->onRender();
    }

}
