#pragma once
#include <Motor/Primitivos/Escena.hpp>
#include <Juego/Figuras/Figuras.hpp>
#include <memory>

namespace IVJ
{
    class Escena_Init : public CE::Escena
    {
        public:

            explicit Escena_Init();
            virtual ~Escena_Init(){};
            void onInit() override;
            void onFinal() override;
            void onUpdate(float dt) override;
            void onInputs(const CE::Botones& accion) override;
            void onRender() override;
        private:
            int inicializar{1};
            
    };
}
