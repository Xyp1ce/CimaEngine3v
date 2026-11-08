#pragma once

#include <Juego/objetos/Entidad.hpp>
#include <Motor/Primitivos/Escena.hpp>

namespace IVJ
{
    class Escena_shaders : public CE::Escena
    {
    public:
        explicit Escena_shaders(std::shared_ptr<Entidad> &ref);
        ~Escena_shaders() override {};

        void onInit() override;
        void onFinal() override;
        void onUpdate(float dt) override;
        void onInputs(const CE::Botones &accion) override;
        void onRender() override;

    private:
        std::shared_ptr<Entidad> &player;
        bool inicializar{true};
    };
}
