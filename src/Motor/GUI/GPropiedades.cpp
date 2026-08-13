#include "GPropiedades.hpp"
#include <imgui.h>
#include <imgui-SFML.h>
#include "../Render/Render.hpp"
#include "GLogger.hpp"
#include "Juego/Componentes/IJComponentes.hpp"
#include "Motor/Componentes/IComponentes.hpp"

namespace CE
{
    void GPropiedades::OnInit(const MotorConfig& des)
    {
        auto settings = Render::Get().GetVentana().getSettings();
        m_openglMajor = settings.majorVersion;
        m_openglMinor = settings.minorVersion;
        (void)des;
    }
    void GPropiedades::OnUpdate(float dt)
    {
        if(!guipath) guipath = new GPaths();
        guipath->setObjetoSelect(objeto_select);
        guipath->OnUpdate(dt);
    }
    void GPropiedades::OnRender(void)
    {
        if(objeto_select)
        {
            //revisar si tiene las interfaces y mostrar las 
            //propiedades adecuadas, de cajon tiene transformada
            ImGui::Begin("Objeto Propiedades",NULL,0);
            
            ImGui::Separator();
            ImGui::Text("%s",objeto_select->getNombre()->nombre.c_str());

            //lab9 colisiones
            if(panelStats())
                ImGui::Separator();
            if(panelTransformada())
                ImGui::Separator();
            if(panelSprite(objeto_select->getComponente<ISprite>()))
                ImGui::Separator();
            if(panelIControl(objeto_select->getComponente<IControl>()))
                ImGui::Separator();
            if(panelShaders(objeto_select->getComponente<IShader>()))
                ImGui::Separator();
            if(panelIPaths(objeto_select->getComponente<IPaths>()))
            {
                guipath->OnRender();
            }    
            if(panelIMaquinaEstado(objeto_select->getComponente<IVJ::IMaquinaEstado>()))
            {
                ImGui::Separator();
            }

            ImGui::End();
        }

        //debería tilemap ser un ente u objeto
        if(tilemap_select)
        {
            ImGui::Begin("TileMap Propiedades",NULL,0);

            if(panelShaders(tilemap_select->getTilemapShader())) ImGui::Separator();

            ImGui::End();
        }


    }
    //lab9 colisiones
    bool GPropiedades::panelStats()
    {
        //todos tienen stats por lo que no es necesario checkar
        ImGui::TextColored(ImVec4{0,255,0,255},"%10sStats%10s"," "," ");
        auto stats = objeto_select->getStats();
        int hp[2] ={stats->hp,stats->hp_max};
        int str = (int)stats->str;
        int def = (int)stats->def;
        int agi = (int)stats->agi;
        ImGui::InputInt2("HP/HP_MAX",hp);
        ImGui::InputInt("STR",&str);
        ImGui::InputInt("DEF",&def);
        ImGui::InputInt("AGI",&agi);
        //si cambia el inputbox entonces cambiar los valores
        stats->hp = hp[0];
        stats->hp_max = hp[1];
        stats->str = str;
        stats->def = def;
        stats->agi = agi;
        return true;
    }
    bool GPropiedades::panelTransformada()
    {
        ImGui::TextColored(ImVec4{0,255,0,255},"%10sITransformada%10s"," "," ");
        auto pos = objeto_select->getTransformada()->posicion;
        float p[2]={pos.x,pos.y};
        auto *vel = &objeto_select->getTransformada()->velocidad;
        float v[2]={vel->x,vel->y};
        auto* ang = &objeto_select->getTransformada()->angulo;
        ImGui::DragFloat2("Posición",p,1.f,0.f,0.f);
        ImGui::DragFloat2("Velocidad",v,1.f,0.f,0.f);
        ImGui::SliderFloat("Ángulo",ang,-10,10);
        
        objeto_select->setPosicion(p[0],p[1]);
        vel->x = v[0];
        vel->y = v[1];

        return true;
    }
    bool GPropiedades::panelSprite(ISprite *sprite)
    {
        if(!sprite) return false;
        
        ImGui::TextColored(ImVec4{0,255,0,255},"%15sISprite%15s"," "," ");

        auto *s = &sprite->m_sprite;
        auto o = s->getOrigin();
        float p[2] = {o.x,o.y};
        ImGui::Image(*s,sf::Vector2f{100.f,100.f});
        ImGui::DragFloat2("Pivote",p,0.01f,0.f,1.f);
        s->setOrigin({p[0],p[1]});
        return true;
    }
    bool GPropiedades::panelIControl(IControl *control)
    {
        if(!control)
            return false;
        ImGui::TextColored(ImVec4{0,255,0,255},"%15sIControl%15s"," "," ");
        
        int arr = control->arr;
        int abj = control->abj;
        int der = control->der;
        int izq = control->izq;
        int run = control->run;
        int jmp = control->jmp;
        int acc = control->acc;
        int sacc = control->sacc;
        bool activo = control->isActivo();

        int pad[4]={arr,abj,der,izq};
        int mov[2] ={run,jmp};
        int accion[2]={acc,sacc};

        ImGui::DragInt4("Pad",pad);
        ImGui::DragInt2("Mov",mov);
        ImGui::DragInt2("Acción",accion);
        ImGui::Checkbox("Activo",&activo);

        //modificar
        control->arr=pad[0];
        control->abj=pad[1];
        control->der=pad[2];
        control->izq=pad[3];
        control->run=mov[0];
        control->jmp=mov[1];
        control->acc=accion[0];
        control->sacc=accion[1];
        control->setActivo(activo);

        return true;
    }
    bool GPropiedades::panelShaders(IShader *shader)
    {
        if(!shader) return false;
        
        ImGui::TextColored(ImVec4{0,255,0,255},"%15sIShader%15s"," "," ");
        
        //https://en.wikipedia.org/wiki/OpenGL_Shading_Language
        ImGui::Text("OpenGL %d.%d",m_openglMajor,m_openglMinor);
        
        auto s_path = shader->m_fragshaderFile;
        auto sv_path = shader->m_vertshaderFile;
        ImGui::Text("VertShader %s",sv_path.c_str());
        ImGui::Text("FragShader %s",s_path.c_str());
        
        //variables modificables
        
        for(auto iter = shader->m_vars.begin();iter!=shader->m_vars.end();iter++)
        {
            auto key = iter->first;
            auto var = shader->m_vars[key];
            auto tipo = var.first;
            auto valor = var.second;

            if(tipo == IShader::ShaderVars::FLOAT)
            {
                float *v = static_cast<float*>(valor);
                ImGui::DragFloat(key.c_str(),v,0.01);
            }
            else if(tipo == IShader::ShaderVars::VEC2F)
            {
                auto vec2 = static_cast<sf::Vector2f*>(valor);
                float arr[2] = {vec2->x,vec2->y};
                ImGui::DragFloat2(key.c_str(),arr,0.01);
                vec2->x = arr[0]; 
                vec2->y = arr[1]; 
            }
            else if(tipo == IShader::ShaderVars::VEC3F)
            {
                auto vec3 = static_cast<sf::Vector3f*>(valor);
                float arr[3] = {vec3->x,vec3->y,vec3->z};
                ImGui::DragFloat3(key.c_str(),arr,0.01);
                vec3->x = arr[0]; 
                vec3->y = arr[1]; 
                vec3->z = arr[2]; 
            }
            else if(tipo == IShader::ShaderVars::VEC4F)
            {
                auto vec4 = static_cast<sf::Glsl::Vec4*>(valor);
                float arr[4] = {vec4->x,vec4->y,vec4->z,vec4->w};
                ImGui::DragFloat4(key.c_str(),arr,0.01);
                vec4->x = arr[0]; 
                vec4->y = arr[1]; 
                vec4->z = arr[2]; 
                vec4->w = arr[3]; 
            }
            else if(tipo == IShader::ShaderVars::TEX)
            {
                auto tex = static_cast<sf::Texture*>(valor);
                //char input[256] ={0};
                //ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize,ImVec2(50,50));
                //ImGui::InputText("imagen_path",input,256);
                //ImGui::PopStyleVar(1);
                
                ImGui::Image(*tex,sf::Vector2f{100.f,100.f},sf::Color::White,sf::Color::Transparent);
                ImGui::SameLine(); 
                ImGui::Text("%s",key.c_str());
            }
        }


        if(ImGui::Button("Reload"))
        {
            auto vars = shader->m_vars;
            shader->m_vars.clear();
            shader->cambiarShader(
                    shader->m_vertshaderFile,
                    shader->m_fragshaderFile);
            shader->m_vars = vars;
            for(auto& pair : shader->m_vars)
            {

                auto key = pair.first;
                auto var = shader->m_vars[key];
                auto tipo = var.first;
                auto valor = var.second;
                switch(tipo)
                {
                    case CE::IShader::ShaderVars::FLOAT:
                    {
                        shader->setEscalar(key,(float*)valor);
                        break;
                    }
                    case CE::IShader::ShaderVars::VEC2F:
                    {
                        shader->setVector2(key,(sf::Glsl::Vec2*)valor);
                        break;
                    }
                    case CE::IShader::ShaderVars::VEC3F:
                    {
                        shader->setVector3(key,(sf::Glsl::Vec3*)valor);
                        break;
                    }
                    case CE::IShader::ShaderVars::VEC4F:
                    {
                        shader->setVector4(key,(sf::Glsl::Vec4*)valor);
                        break;
                    }
                    case CE::IShader::ShaderVars::MAT3:
                    {
                        break;
                    }
                    case CE::IShader::ShaderVars::MAT4:
                    {
                        break;
                    }
                    case CE::IShader::ShaderVars::TEX:
                    {
                        shader->setTextura(key,(sf::Texture*)valor);
                        break;
                    }
                    default:
                        break;
                }
            }
            
            //if(!shader->m_vertshaderFile.empty() && shader->m_fragshaderFile.empty())
            //{
            //    if(!shader->m_shader.loadFromFile(shader->m_vertshaderFile,sf::Shader::Type::Vertex))
            //        GLogger::Get().agregarLog("No se pudo cargar Shader",GLogger::Niveles::LOG_ERROR);
            //}
            //else if(shader->m_vertshaderFile.empty() && !shader->m_fragshaderFile.empty())
            //{
            //    if(!shader->m_shader.loadFromFile(shader->m_fragshaderFile,sf::Shader::Type::Fragment))
            //        GLogger::Get().agregarLog("No se pudo cargar Shader",GLogger::Niveles::LOG_ERROR);
            //}
            //else
            //{
            //    if(!shader->m_shader.loadFromFile(shader->m_vertshaderFile,shader->m_fragshaderFile))
            //        GLogger::Get().agregarLog("No se pudo cargar Shader",GLogger::Niveles::LOG_ERROR);
            //}
        }

        return true;
    }
    void GPropiedades::seleccionarObjeto(Objeto* ref)
    {
        objeto_select = ref;
        tilemap_select=nullptr;
    }
    bool GPropiedades::panelIPaths(IPaths *path)
    {
        if(!path) return false;
        return true;
    }
    void GPropiedades::seleccionarTileMap(IVJ::TileMap *ref)
    {
        tilemap_select = ref;
        objeto_select = nullptr;
    }
    bool GPropiedades::panelIMaquinaEstado(IVJ::IMaquinaEstado *me)
    {
        if(!me ) return false;
        if(!me->fsm) return false;
        //ImGui::Separator();
        ImGui::TextColored(ImVec4{0,255,0,255},"%10sIMaquinaEstado%10s"," "," ");
        ImGui::Text("Estado: %s",me->fsm->getNombre().c_str());
        return true;
    }
}
