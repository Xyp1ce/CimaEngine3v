#pragma once

#include "../Componentes/IJComponentes.hpp"
#include "../objetos/Entidad.hpp"
#include <SFML/Graphics.hpp>

namespace IVJ
{
    class Entidad; //referencia circular
    class FSM
    {
        public:
            virtual ~FSM()=default;
            virtual FSM* onInputs(const CE::IControl& control)=0;
            virtual void onEntrar(const Entidad& obj)=0;
            virtual void onSalir(const Entidad& obj)=0;
            virtual void onUpdate(const Entidad& obj,float dt)=0;
            std::string getNombre() const{return nombre;}
        protected:
            std::string nombre;
    };

}
