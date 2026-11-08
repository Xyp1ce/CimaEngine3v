#include "EstadoMoverYu.hpp"
#include "EstadoIdleYu.hpp"
#include <Motor/Render/Render.hpp>

namespace IVJ
{
    EstadoMoverYu::EstadoMoverYu(const CE::Vector2D &box, int frames, float frame_rate, bool flip)
        : FSM{}, dim_box{box}, frames{frames}, id_frame{0},
          frame_rate{frame_rate}, frame_actual{frame_rate}, flip{flip}
    {
        nombre = "YU_MOVER";
    }

    void EstadoMoverYu::onEntrar(const IVJ::Entidad &obj)
    {
        sprite = &obj.getComponente<CE::ISprite>()->m_sprite;

        if (flip)
        {
            sprite->setScale(sf::Vector2f(-1.f, 1.f));
            // como obj es const, no se puede editar la posición(getTransformada)
            // en la interfaz se tendría que cambiar el prototipo a sin const
        }
        else
        {
            sprite->setScale(sf::Vector2f(1.f, 1.f));
            //-
        }
        if (obj.tieneComponente<CE::IShader>())
        {
            shader = obj.getComponente<CE::IShader>();
        }
        else
            shader = nullptr;
        int padding = 5;

        // almacenar la posición del sprite y
        // el tamaño del cuadro
        frames_sprite[0].first.x = 0;     // pos
        frames_sprite[0].second.x = 348;  // size
        frames_sprite[1].first.x = 352;   // pos
        frames_sprite[1].second.x = 320;  // size
        frames_sprite[2].first.x = 676;   // pos
        frames_sprite[2].second.x = 312;  // size
        frames_sprite[3].first.x = 993;   // pos
        frames_sprite[3].second.x = 312;  // size
        frames_sprite[4].first.x = 1310;  // pos
        frames_sprite[4].second.x = 312;  // size
        frames_sprite[5].first.x = 1629;  // pos
        frames_sprite[5].second.x = 318;  // size
        frames_sprite[6].first.x = 1952;  // pos
        frames_sprite[6].second.x = 334;  // size
        frames_sprite[7].first.x = 2290;  // pos
        frames_sprite[7].second.x = 327;  // size
        frames_sprite[8].first.x = 2621;  // pos
        frames_sprite[8].second.x = 315;  // size
        frames_sprite[9].first.x = 2940;  // pos
        frames_sprite[9].second.x = 313;  // size
        frames_sprite[10].first.x = 3257; // pos
        frames_sprite[10].second.x = 312; // size
        frames_sprite[11].first.x = 3574; // pos
        frames_sprite[11].second.x = 311; // size

        for (auto &frame : frames_sprite)
        {
            frame.first.y = 435;
            frame.first.z = padding * 3;
            frame.second.y = 425;
        }

        id_frame = 0;
    }

    FSM *IVJ::EstadoMoverYu::onInputs(const CE::IControl &control)
    {
        if (!control.der && !control.izq)
            return new EstadoIdleYu(CE::Vector2D{496.f, 443.f}, 10, 0.15f);
        return nullptr;
    }

    void EstadoMoverYu::onSalir(const Entidad &obj)
    {
        (void)obj;
    }

    void EstadoMoverYu::onUpdate(const Entidad &obj, float dt)
    {
        (void)obj;
        static float time = 0;
        time += dt;
        if (shader)
        {
            shader->setEscalar("time", &time);
        }
        frame_actual -= dt;
        // animación
        if (frame_actual <= 0)
        {
            int dp = frames_sprite[id_frame].first.z;
            int dx = frames_sprite[id_frame].first.x;
            int dy = frames_sprite[id_frame].first.y + dp;
            // std::cout<<"sprite_frame: "<<id_frame<<"("<<dx<<","<<dy<<")"<<"\n";
            sprite->setTextureRect(
                sf::IntRect{
                    {// posición
                     dx, dy},
                    {// tamaño
                     frames_sprite[id_frame].second.x,
                     frames_sprite[id_frame].second.y}});

            id_frame = (id_frame + 1) % frames;
            frame_actual = frame_rate;
        }
    }
}
