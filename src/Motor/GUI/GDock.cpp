#include "GDock.hpp"

#include<imgui.h>
#include<imgui_internal.h>

namespace CE
{
    void GDock::OnInit(const MotorConfig &des)
    {
        this->des = des;
#if DEBUG
        flags = ImGuiWindowFlags_NoTitleBar | 
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoBringToFrontOnFocus | 
                ImGuiWindowFlags_NoNavFocus |
                ImGuiWindowFlags_NoDocking | 
                ImGuiWindowFlags_MenuBar;
#else
        flags = ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoBringToFrontOnFocus | 
                ImGuiWindowFlags_NoNavFocus |
                ImGuiWindowFlags_NoDocking;
#endif

    }

    void GDock::OnUpdate(float dt)
    {
        this->des.motor_dt = dt;
    }

    void GDock::OnRender(void)
    {
        //https://github.com/ocornut/imgui/issues/2109
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGui::Begin("GDock", nullptr, flags); //inicio de GDock
#if DEBUG
        ImGuiDockNodeFlags dockflags=  0;

        if(ImGui::BeginMenuBar())
        {
            if(ImGui::Button("Archivos")){}
            if(ImGui::Button("Edit")){}
            ImGui::EndMenuBar();
        }
#else
        ImGuiDockNodeFlags dockflags = ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_AutoHideTabBar;
        
#endif
        auto id= ImGui::GetID("GDock");
        dx = (unsigned int)ImGui::GetContentRegionAvail().x;
        dy = (unsigned int)ImGui::GetContentRegionAvail().y;
        ImGui::DockSpace(id,ImVec2(dx,dy),dockflags);

        ImGui::DockBuilderDockWindow("GViewPort",id);
        ImGui::DockBuilderFinish(id);

        ImGui::End(); //fin de GDock
    }

}
