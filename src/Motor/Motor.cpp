#include "Motor.hpp"
#include <utility>
#include <chrono>
#include "Render/Render.hpp"
#include "GUI/GDock.hpp"
#include "GUI/GViewport.hpp"
#if DEBUG
    #include "GUI/GLogger.hpp"
    #include "GUI/GListaObjetos.hpp"
    //#include "GUI/GPaths.hpp"
#endif
#include "Camaras/CamarasGestor.hpp"
#include "Primitivos/GestorAssets.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui-SFML.h>

namespace CE
{

    Motor::Motor(const MotorConfig& config,std::unique_ptr<GameLayer> ref)
        :motor_info(std::move(config)),mi_app(std::move(ref))
    {}
    int Motor::OnRun(void)
    {
        OnInit();

        //Calcular tiempo sin librería
        auto frame_ant = std::chrono::high_resolution_clock::now();
        float ms = 0.16666f;
        sf::Clock clock = sf::Clock();
        while(mi_app->EstaCorriendo())
        {
            OnEventFrame();
            ImGui::SFML::Update(Render::Get().GetVentana(),clock.restart());
            OnUpdateFrame(ms);
            OnRenderFrame();

            //calcular el dt
            auto frame_act = std::chrono::high_resolution_clock::now();
            ms= std::chrono::duration_cast<std::chrono::duration<float>>(frame_act-frame_ant).count();
            frame_ant = frame_act;

        }
        Render::Terminar();
        ImGui::SFML::Shutdown();

        return 0;
    }
    void Motor::OnInit(void)
    {
        Render::Get().CrearVentana(motor_info);
        //Render::Get().GetVentana().setKeyRepeatEnabled(false);

        if(!ImGui::SFML::Init(Render::Get().GetVentana()))
            exit(-1);

        //ImGui Configuración
        auto& io= ImGui::GetIO();(void)io;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.ConfigDockingWithShift=true;

        gui_layers.push_back(std::make_shared<GDock>());
        gui_layers.push_back(std::make_shared<GViewport>());
#if DEBUG
        gui_layers.push_back(std::make_shared<GListaObjetos>());
        //gui_layers.push_back(std::make_shared<GPaths>());
#endif
        GestorCamaras::Get().agregarCamara(std::make_shared<Camara>(Vector2D{540,360},Vector2D{1080,720}));
        GestorCamaras::Get().setCamaraActiva(0);

        //font por defecto
        GestorAssets::Get().agregarFont("default_font",ASSETS "/fonts/Electrolize.ttf");
        GestorAssets::Get().agregarFont("nova",ASSETS "/fonts/NovaSquare.ttf");

        mi_app->OnInit();

        for(auto &gui: gui_layers)
            gui->OnInit(motor_info);

#if DEBUG
        GLogger::Get().OnInit(motor_info);
#endif

    }
    void Motor::OnEventFrame()
    {
        //GUI inputs
        while(auto eventos=Render::Get().GetVentana().pollEvent())
        {
            ImGui::SFML::ProcessEvent(Render::Get().GetVentana(),*eventos);
            //mi_app->OnInputs(dt,eventos);
            inputbuffer.pushData(eventos);
        }
        //controles inputs
        mi_app->OnInputs(inputbuffer);
        inputbuffer.clearData();
    }
    void Motor::OnUpdateFrame(float dt)
    {
        GestorCamaras::Get().onUpdateCamaras(dt);
        mi_app->OnUpdate(dt);
        for(auto &gui: gui_layers)
            gui->OnUpdate(dt);

#if DEBUG
        GLogger::Get().OnUpdate(dt);
#endif
    }
    void Motor::OnRenderFrame()
    {
        Render::Get().OnClearColor(sf::Color(118,118,255));
        GestorCamaras::Get().onRenderCamara(Render::Get().GetTextura());

        mi_app->OnRender();

        for(auto &gui: gui_layers)
            gui->OnRender();
#if DEBUG
        GLogger::Get().OnRender();
#endif

        Render::Get().OnDisplayTextura();

        //ImGui

        if(ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        ImGui::SFML::Render(Render::Get().GetVentana());

        Render::Get().OnDisplayVentana();

    }
}
