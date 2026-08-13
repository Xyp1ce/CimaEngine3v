#include "Camaras.hpp"
#include "../GUI/GLogger.hpp"
#include "../Utils/Lerp.hpp"

namespace CE
{
    int Camara::num_camaras =0;
    Camara::Camara(float x, float y,float w, float h)
        :cam_width{w},cam_height{h},esta_activa{false}
    {
        m_transform = std::make_shared<ITransform>(
            Vector2D{x,y},Vector2D{0,0},0);
        m_view = std::make_shared<sf::View>(
            sf::FloatRect{{x,y},{w,h}});
        m_view->setCenter({cam_width/2.f,cam_height/2.f});
        nombre = "Camara Default #"+std::to_string(Camara::num_camaras);
        Camara::num_camaras++;
    }

    Camara::Camara(const Vector2D& pos, const Vector2D& dim)
        :cam_width{dim.x},cam_height{dim.y},esta_activa{false}
    {
        m_transform = std::make_shared<ITransform>(
            pos,Vector2D{0,0},0);
        m_view = std::make_shared<sf::View>(
            sf::FloatRect{{pos.x,pos.y},{cam_width,cam_height}});

        m_view->setCenter({cam_width/2.f,cam_height/2.f});
        Camara::num_camaras++;
        nombre = "Camara Default #"+std::to_string(Camara::num_camaras);
    }

    void Camara::setViewSize(float x, float y)
    {
        m_view->setSize({x,y});
    }

    void Camara::lockEnObjeto(const std::shared_ptr<Objeto>& obj)
    {
        m_lockObj = obj;
    }

    void Camara::onUpdate(float dt)
    {
        (void)dt;
        //ajustamos el pivote de la camara
        m_view->setCenter(
                {m_transform->posicion.x,
                m_transform->posicion.y});
    }
    /*
     * CAMARACUADRO
     *
    */
    CamaraCuadro::CamaraCuadro(const Vector2D& pos, const Vector2D& dim)
        :Camara{pos,dim},limitex{dim.x},limitey{dim.y}
    {

        nombre = "Camara Cuadro #"+std::to_string(Camara::num_camaras);
    }

    void CamaraCuadro::onUpdate(float dt)
    {
        Camara::onUpdate(dt);
        //safe guard de nulo
        if(!m_lockObj.lock()) return;
        
        auto mitad = Vector2D(cam_width,cam_height).escala(0.5f);

        auto obj_trans = m_lockObj.lock()->getTransformada();
        auto opos = obj_trans->posicion;
        std::string log = "ObJ: ("+std::to_string(opos.x)+","+std::to_string(opos.y)+")";
        std::string log2 = nombre+": "+std::to_string(m_transform->posicion.x)+","+std::to_string(m_transform->posicion.y)+")";

        GLogger::Get().agregarLog(log,GLogger::Niveles::LOG_DEBUG);
        GLogger::Get().agregarLog(log2,GLogger::Niveles::LOG_DEBUG);
        if(obj_trans->posicion.x > (m_transform->posicion.x + mitad.x))
        {
            m_transform->posicion.x+= cam_width;
            limitex+=limitex; 
        }
        if(obj_trans->posicion.y > (m_transform->posicion.y + mitad.y))
        {
            m_transform->posicion.y+= cam_height;
            limitey+=limitey; 
        }
    }
    /*Camar LERP
     */
    CamaraLERP::CamaraLERP(const Vector2D& pos, const Vector2D& dim)
        :Camara{pos,dim},vel{4.f}
    {

        nombre = "Camara LERP #"+std::to_string(Camara::num_camaras);
    }
    void CamaraLERP::onUpdate(float dt)
    {
        Camara::onUpdate(dt);
        if(!m_lockObj.lock()) return;
        Vector2D pos_init  = m_transform->posicion;
        auto pos_lock = m_lockObj.lock()->getTransformada()->posicion;

        m_transform->posicion = lerp(pos_init,pos_lock,vel*dt);
        GLogger::Get().agregarLog("("+std::to_string(m_transform->posicion.x)+","+std::to_string(m_transform->posicion.y)+")",
                GLogger::Niveles::LOG_SEVERO);
    }

    //Camara SnapVentana
    CamaraSnapVentana::CamaraSnapVentana(const Vector2D& pos, const Vector2D& dim, const Vector2D& vdim)
        :Camara{pos,dim},m_vdim{vdim}
    {}

    void CamaraSnapVentana::onUpdate(float dt)
    {
        //llamar el comportamiento base.
        Camara::onUpdate(dt); 
        //si no esta siguiendo a nadie 
        if(!m_lockObj.lock()) return;
        if(!lock) // si no se le a asignado el jugador
        {
            m_transform->posicion = m_lockObj.lock()->getTransformada()->posicion;
            lock=true;
        }
        //copias
        auto jpos = m_lockObj.lock()->getTransformada()->posicion;
        float ld = (m_transform->posicion.x+m_vdim.x/2.f);
        float li = (m_transform->posicion.x-m_vdim.x/2.f);
        float lup = (m_transform->posicion.y-m_vdim.y/2.f);
        float ldn = (m_transform->posicion.y+m_vdim.y/2.f);
        //eje x
        if(jpos.x>= ld)
            m_transform->posicion.x +=(jpos.x-ld);
        else if(jpos.x<=li)
            m_transform->posicion.x +=(jpos.x-li);
        //ejey
        if(jpos.y>= ldn)
            m_transform->posicion.y +=(jpos.y-ldn);
        else if(jpos.y<=lup)
            m_transform->posicion.y +=(jpos.y-lup);
    }
}
