#include "GPaths.hpp"
#include "SFML/Graphics/CircleShape.hpp"
#include <imgui.h>
#include <SFML/Graphics.hpp>
#include "../Render/Render.hpp"
#include "../Utils/Lerp.hpp"

namespace CE
{

    void GPaths::OnInit(const MotorConfig& des)
    {
        (void)des; //quitar el warning
    }
    void GPaths::OnUpdate(float dt)
    {
        (void) dt;
        if(!objeto_select || !objeto_select->tieneComponente<IPaths>()) return;

        ImGui::Begin("IPaths Componente",NULL,0);
        auto path = objeto_select->getComponente<IPaths>();
        auto pos = objeto_select->getTransformada()->posicion;
        ImGui::InputInt("id_curva  ",&path->id_curva);
        ImGui::Text("frame      %d",path->frame_actual_curva);
        ImGui::InputInt("max_frame ",&path->frame_total_curva);
        ImGui::Separator();
        ImGui::Text("Posición  (%.3f, %.3f)",pos.x,pos.y);
        ImGui::Separator();
        ImGui::TextColored(ImVec4{255,255,0,255},"==== Path ====" );
        if(ImGui::Button("Reset"))
        {
            path->id_curva=0;
            path->frame_actual_curva=0;
        }
        for(int i=0;i<(int)path->puntos.size();i+=3)
        {
            ImGui::PushID(i);
            auto p1 = path->puntos[i];
            float fp1[2] = {p1.x,p1.y};
            auto p2 = path->puntos[i+1];
            float fp2[2] = {p2.x,p2.y};
            auto p3 = path->puntos[i+2];
            float fp3[2] = {p3.x,p3.y};
            ImGui::DragFloat2("Inicio   ",fp1);
            ImGui::DragFloat2("control  ",fp2);
            ImGui::DragFloat2("target   ",fp3);
            path->puntos[i].x = fp1[0];
            path->puntos[i].y = fp1[1];
            path->puntos[i+1].x = fp2[0];
            path->puntos[i+1].y = fp2[1];
            path->puntos[i+2].x = fp3[0];
            path->puntos[i+2].y = fp3[1];
            ImGui::Separator();
            ImGui::PopID();
        }
        ImGui::End();
    }
    void GPaths::OnRender(void)
    {
        if(!objeto_select || !objeto_select->tieneComponente<IPaths>()) return;

        auto path = objeto_select->getComponente<IPaths>();
        for(int i=0;i<(int)path->puntos.size();i+=3)
        {
            sf::CircleShape p1{10.f};
            p1.setFillColor({255,255,0,255});
            sf::CircleShape p2{10.f};
            p2.setFillColor({255,0,255,255});
            sf::CircleShape p3{10.f};
            p3.setFillColor({255,255,0,255});
            auto posp1 = path->puntos[i];
            p1.setPosition({posp1.x,posp1.y});
            auto posp2 = path->puntos[i+1];
            p2.setPosition({posp2.x,posp2.y});
            auto posp3 = path->puntos[i+2];
            p3.setPosition({posp3.x,posp3.y});
            
            //pintar lineas de guía
            for(float t=0;t<=1;t+=0.0015)
            {
                sf::CircleShape pixel{1};
                pixel.setFillColor(sf::Color{0,0,0,255});
                auto puntopos  = lerp2(posp1,posp2,posp3,t);
                pixel.setPosition({puntopos.x,puntopos.y});
                Render::Get().AddToDraw(pixel);
                
            }
            Render::Get().AddToDraw(p1);
            Render::Get().AddToDraw(p2);
            Render::Get().AddToDraw(p3);
        }
    }
    void GPaths::setObjetoSelect(Objeto *ref)
    {
        objeto_select = ref;
    }
}
