#include "GViewport.hpp"
#include "../Render/Render.hpp"
#include<imgui_internal.h>
#include <imgui.h>
#include<imgui-SFML.h>
namespace CE
{

    void GViewport::OnInit(const MotorConfig& des) 
    {
        vw=0;
        vh=0;
        motor_info = des;
    }
    void GViewport::OnUpdate(float dt) {(void)dt;}
    void GViewport::OnRender(void)
    {
#if DEBUG
        ImGui::Begin("GViewPort" ,nullptr,ImGuiWindowFlags_NoCollapse);
#else
        ImGui::Begin("GViewPort",nullptr,ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoDecoration |
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
#endif
        vw = (unsigned int)ImGui::GetContentRegionAvail().x;
        vh = (unsigned int)ImGui::GetContentRegionAvail().y;

//#if DEBUG
//        if(vw==0 || vh == 0)
//        {
//            vw = motor_info.vW - motor_info.vW*0.25f;
//            vh = motor_info.vH - motor_info.vH*0.25f;
//        }
//#endif
        ImGui::Image(Render::Get().GetTextura(),
                    sf::Vector2f(vw,vh), 
                    sf::Color::White, 
                    sf::Color::Transparent);
        Render::Get().AutoResizeTextura(vw,vh);
        ImGui::End();
    }
}
