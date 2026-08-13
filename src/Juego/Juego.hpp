#pragma once
#include"../Motor/App/ALayer.hpp"
//#include <vector>
//#include "../Motor/Primitivos/Objetos.hpp"
#include "../Motor/Primitivos/Escena.hpp"

//#include <memory>
namespace IVJ
{
    class Juego: public CE::GameLayer
    {
        public:
            explicit Juego();
            ~Juego()override{};
            void OnInit(void) override;
            void OnInputs(std::optional<sf::Event>& eventos) override;
            void OnInputs(CE::InputBuffer& buffer) override;
            void OnInputs() override;
            void OnUpdate(float dt) override;
            void OnRender() override;
        private:
            //std::vector<std::shared_ptr<CE::Objeto>> objetos;
            //CE::Pool pool{1000};
            CE::Escena* escena_actual;
            std::shared_ptr<Entidad> jugador;
    };

}

