#include "GLogger.hpp"
#include <imgui-SFML.h>

namespace CE
{
    GLogger::GLogger()
        :GLayer{}
    {
    }

    GLogger& GLogger::Get()
    {
        if(!instancia)
            instancia = new GLogger();
        return *instancia;
    }

    void GLogger::OnInit(const MotorConfig& des)
    {
        m_config = des;
    }

    void GLogger::OnUpdate(float dt)
    {
        m_sfps = std::to_string(1.f/dt)+" fps";
        m_config.motor_dt=dt;
        agregarLog("motor: "+m_config.titulo,Niveles::LOG);
        agregarLog("motor_w: "+std::to_string(m_config.vW),Niveles::LOG);
        agregarLog("motor_h: "+std::to_string(m_config.vH),Niveles::LOG);
        agregarLog("motor_dt: "+std::to_string(m_config.motor_dt),Niveles::LOG_DEBUG);
        //agregarLog(m_sfps,(Niveles)-1);
    }

    void GLogger::OnRender(void)
    {
        ImGui::Begin("Log",nullptr,0);
        ImGui::LabelText("","[%s]",m_sfps.c_str());        
        //dequeu a la cola
        while(queue.getSize()>0)
        {
            auto log = queue.popFrontDato();
            ImVec4 color = nivelColores(log.second);
            ImGui::TextColored(color, "%10s %s",nivelToString(log.second).c_str(),log.first.c_str());
        }

        ImGui::End();
    }

    void GLogger::agregarLog(const std::string& log, GLogger::Niveles n)
    {
        queue.agregarDato(std::make_pair(log,n));
    }
    
    ImVec4 GLogger::nivelColores(Niveles n)
    {
        switch(n)
        {
            case GLogger::Niveles::LOG:  //gris
                return ImVec4(169,169,169,255);
            case GLogger::Niveles::LOG_DEBUG: // amarillo
                return ImVec4(255,255,0,255);
            case GLogger::Niveles::LOG_ERROR: //rojo
                return ImVec4(255,0,0,255);
            case GLogger::Niveles::LOG_SEVERO:
                return ImVec4(148,0,211,255); //morado
            default:
                return ImVec4(255,255,255,255); //blanco
        }
    }
    std::string GLogger::nivelToString(Niveles n)
    {
        switch(n)
        {
            case GLogger::Niveles::LOG:  //gris
                return "[LOG] ";
            case GLogger::Niveles::LOG_DEBUG: // amarillo
                return "[DEBUG] ";
            case GLogger::Niveles::LOG_ERROR: //rojo
                return "[ERROR] ";
            case GLogger::Niveles::LOG_SEVERO:
                return "[SEVERO] ";
            default:
                return "[] ";
        }
    }
}
