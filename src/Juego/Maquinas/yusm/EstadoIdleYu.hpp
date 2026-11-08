#pragma once
#include <Juego/Maquinas/FSM.hpp>

namespace IVJ
{
    class EstadoIdleYu : public FSM
    {
    public:
        explicit EstadoIdleYu(const CE::Vector2D &box, int frames, float frame_rate = 0.3f);
        ~EstadoIdleYu() override = default;
        FSM *onInputs(const CE::IControl &control) override;
        void onEntrar(const Entidad &obj) override;
        void onSalir(const Entidad &obj) override;
        void onUpdate(const Entidad &obj, float dt) override;

    private:
        CE::Vector2D dim_box;
        sf::Sprite *sprite;
        int frames;
        int id_frame;
        float frame_rate;
        float frame_actual;
        float padding{};
        CE::IShader *shader{};
    };
}
