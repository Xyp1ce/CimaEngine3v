#include "../../Motor/Componentes/IComponentes.hpp"
#include "../../Motor/Primitivos/GestorAssets.hpp"
#include "../../Motor/Render/Render.hpp"
#include "../../Motor/Utils/Lerp.hpp"
#include "../objetos/Entidad.hpp"
#include "Motor/GUI/GLogger.hpp"
#include "Motor/Utils/Vector2D.hpp"
#include "Sistemas.hpp"
#include <cmath>
#include <memory>

#include <Juego/Componentes/IJComponentes.hpp>
#include <string>

namespace IVJ
{
void SistemaControl(CE::Objeto &ente, float dt)
{
    // no hay necesidad verificar si tiene, se asume que tiene
    // control y transformada
    auto p = ente.getTransformada();
    auto c = ente.getComponente<CE::IControl>();
    (void)dt;
    if (c->arr)
        p->velocidad.y = -200;
    if (c->abj)
        p->velocidad.y = 200;
    if (c->der)
        p->velocidad.x = 200;
    if (c->izq)
        p->velocidad.x = -200;
    if (c->run)
        p->velocidad.escala(2.f);
}

void SistemaMover(const std::vector<std::shared_ptr<CE::Objeto>> &entes, float dt)
{

    for (auto &ente : entes)
    {
        // todo ente tiene ITransform por lo que no requiere verificación
        auto trans = ente->getTransformada();
        trans->posicion.suma(trans->velocidad.escala(dt));
    }
}

void SistemaMoverBalas(const std::shared_ptr<CE::Objeto> &ente, float dt)
{
    auto trans = ente->getTransformada();
    auto vel = ente->getTransformada()->velocidad;
    trans->posicion.suma(vel.escala(dt));
}

void SistemaMover(const std::shared_ptr<CE::Objeto> &objeto, float dt)
{
    auto trans = objeto->getTransformada();

    auto control = objeto->getComponente<CE::IControl>();
    if (!control || !control->isActivo())
        return;

    auto vel = CE::Vector2D{0.f, 0.f};
    if (control->arr)
        vel.y = -trans->velocidad.y;
    if (control->abj)
        vel.y = trans->velocidad.y;
    if (control->der)
        vel.x = trans->velocidad.x;
    if (control->izq)
        vel.x = -trans->velocidad.x;

    if (objeto->getComponente<ITriangulo>() || objeto->getComponente<CE::ISprite>())
    {
        auto n = vel;
        n.normalizacion();
        auto trian = objeto->getComponente<ITriangulo>(); // nulo
        if (vel.x != 0 || vel.y != 0)
        {
            if (trian)
                trian->angulo = std::atan2(n.x, -n.y);
            trans->angulo = std::atan2(n.x, -n.y);
        }
    }

    trans->posicion.suma(vel.escala(dt));
}

bool SistemaColAABB(CE::Objeto &A, CE::Objeto &B, bool resolucion)
{
    if (!A.tieneComponente<CE::IBoundingBox>() || !B.tieneComponente<CE::IBoundingBox>())
        return false;
    auto bA = A.getComponente<CE::IBoundingBox>()->tam;
    auto mA = A.getComponente<CE::IBoundingBox>()->mitad;
    auto *pa = &A.getTransformada()->posicion;
    auto prevA = A.getTransformada()->pos_prev;
    auto bB = B.getComponente<CE::IBoundingBox>()->tam;
    auto mB = B.getComponente<CE::IBoundingBox>()->mitad;
    auto pb = B.getTransformada()->posicion;
    // calculos
    bool H = pa->y - mA.y < pb.y + bB.y - mB.y && pb.y - mB.y < pa->y + bA.y - mA.y;
    bool V = pa->x - mA.x < pb.x + bB.x - mB.x && pb.x - mB.x < pa->x + bA.x - mA.x;
    bool hay_colision = H && V;
    if (hay_colision && resolucion)
        *pa = prevA;
    return hay_colision;
}

bool SistemaColAABBMid(CE::Objeto &A, CE::Objeto &B, bool resolucion)
{
    if (!A.tieneComponente<CE::IBoundingBox>() || !B.tieneComponente<CE::IBoundingBox>())
        return false;
    auto midA = A.getComponente<CE::IBoundingBox>()->mitad;
    auto *pa = &A.getTransformada()->posicion;
    auto prevA = A.getTransformada()->pos_prev;
    auto midB = B.getComponente<CE::IBoundingBox>()->mitad;
    auto *pb = &B.getTransformada()->posicion;
    // calculos
    float dX = std::abs(pb->x - pa->x);
    float dY = std::abs(pb->y - pa->y);
    float sumMidX = midA.x + midB.x;
    float sumMidY = midA.y + midB.y;
    bool V = sumMidX - dX > 0;
    bool H = sumMidY - dY > 0;
    bool hay_colision = V && H;
    if (resolucion && hay_colision)
        *pa = prevA;
    return hay_colision;
}

bool checkRayoHit(CE::Objeto &npc, CE::Vector2D &p1, CE::Vector2D &p2)
{
    if (!npc.tieneComponente<CE::IBoundingBox>())
        return false;

    auto midnpc = npc.getComponente<CE::IBoundingBox>()->mitad;
    auto posnpc = npc.getTransformada()->posicion;
    auto linea = (p2 - p1); //.escala(1.f);
    // std::cout<<"Mid linea: "<<midlinea<<"\n";
    float dx = std::abs(posnpc.x - p1.x);
    float dy = std::abs(posnpc.y - p1.y);
    float sumMidX = midnpc.x + std::abs(linea.x);
    float sumMidY = midnpc.y + std::abs(linea.y);

    return (sumMidX - dx > 0) && (sumMidY - dy > 0);
}

void SistemaSpawn(CE::Pool &spawns)
{
    for (auto &s : spawns.getPool())
    {
        auto respawndata = s->getComponente<CE::IRespawn>();
        if (!respawndata || respawndata->curr_spawn >= respawndata->max_spawn)
            continue;
        // respawn
        if (respawndata->timer_actual >= respawndata->timer_maximo)
        {
            // crear objeto
            auto malillo = std::make_shared<Entidad>(); //(*(respawndata->prefab));
            // auto comp = respawndata->prefab->getListaComponentes();
            // auto comp = respawndata->prefab->getComponente<CE::ISprite>();
            // for(auto &c : comp)
            //{
            // malillo->copyComponente<CE::ISprite>(comp);
            // malillo->copyComponente<CE::IPaths>(respawndata->prefab->getComponente<CE::IPaths>());
            //}

            malillo->addComponente(std::make_shared<CE::IPaths>(60))
                .addComponente(
                    std::make_shared<CE::ISprite>(CE::GestorAssets::Get().getTextura("hoja_blue"), 68, 91, 1.f))
                .addComponente(std::make_shared<CE::IShader>("", ASSETS "/shaders/contorno.frag"));
            malillo->getComponente<CE::IPaths>()->addCurva(CE::Vector2D{500.f, 300.f}, CE::Vector2D{600.f, 500.f},
                                                           CE::Vector2D{800.f, 300.f});
            malillo->getComponente<CE::IPaths>()->addCurva(CE::Vector2D{800.f, 300.f}, CE::Vector2D{600.f, 100.f},
                                                           CE::Vector2D{500.f, 300.f});

            // mandar datos al shader
            auto shader = malillo->getComponente<CE::IShader>();
            static sf::Texture textura = malillo->getComponente<CE::ISprite>()->m_sprite.getTexture();

            static sf::Glsl::Vec4 uOutlineColor{1.f, 0.f, 1.f, 1.f};
            // static  float uEdgeThreshold= 0.15f; //0.15-0.35
            // static  float uEdgeSoftness = 0.02f;
            static float uThickness = 0.15f;
            auto w = malillo->getComponente<CE::ISprite>()->width;
            auto h = malillo->getComponente<CE::ISprite>()->height;
            static sf::Glsl::Vec2 uTexSize{(float)w, (float)h};

            shader->setTextura("uTex", &textura);
            shader->setVector4("uOutlineColor", &uOutlineColor);
            // shader->setEscalar("uEdgeThreshold",&uEdgeThreshold);
            // shader->setEscalar("uEdgeSoftness",&uEdgeSoftness);
            shader->setEscalar("uThickness", &uThickness);
            shader->setVector2("uTexSize", &uTexSize);

            // posiciones
            malillo->getStats()->hp = 100;
            // asumimos que spwan tine un cuadro , tomamos el cuadro como el area
            int size_x = respawndata->width;
            int size_y = respawndata->height;
            float x0 = s->getTransformada()->posicion.x - size_x / 2.f;
            float y0 = s->getTransformada()->posicion.y - size_y / 2.f;
            float x = x0 + (rand() % size_x);
            float y = y0 + (rand() % size_y);
            malillo->setPosicion(x, y);
            // spawns.push_back(malillo);
            spawns.agregarPool(malillo);
            respawndata->timer_actual = 0;
            respawndata->curr_spawn++;
        }
        respawndata->timer_actual++;
    }
}
void SistemaPaths(std::vector<std::shared_ptr<CE::Objeto>> &obj)
{
    for (auto &o : obj)
    {
        if (o->tieneComponente<CE::IPaths>())
        {
            auto path = o->getComponente<CE::IPaths>();
            int num_curvas = path->puntos.size() / 3;
            if (path->puntos.size() == 0)
                continue;
            if (path->id_curva >= num_curvas)
                continue;
            float t = (path->frame_actual_curva % (path->frame_total_curva + 1)) / (float)path->frame_total_curva;
            // std::cout<<o<<" t: "<<t<<"\n";
            CE::Vector2D P0 = path->puntos[0 + path->id_curva * (path->offset - 0)];
            CE::Vector2D P1 = path->puntos[1 + path->id_curva * (path->offset - 0)];
            CE::Vector2D P2 = path->puntos[2 + path->id_curva * (path->offset - 0)];
            auto posiciones_actual = CE::lerp2(P0, P1, P2, t);
            path->frame_actual_curva++;
            o->setPosicion(posiciones_actual.x, posiciones_actual.y);
            if (t == 1.f)
                path->id_curva++;
            // path->frame_actual_curva=0;
            // std::cout<<"[ "<<o<<" ] "<<path->frame_actual_curva<<"\n";
            if (path->id_curva == 2 && o->tieneComponente<CE::IShader>())
            {
                // cambiar shader
                auto shader = o->getComponente<CE::IShader>();
                shader->cambiarShader("", ASSETS "/shaders/color.frag");
                static sf::Glsl::Vec4 color{0.f, 1.f, 0.f, 1.f};
                shader->m_vars.clear();
                shader->setVector4("color", &color);
            }
            else
            {
                // cambiar shader
                auto shader = o->getComponente<CE::IShader>();
                shader->cambiarShader("", ASSETS "/shaders/contorno.frag");
                static sf::Glsl::Vec4 outlinecolor{0.f, 1.f, 0.f, 1.f};
                static float thickness = 0.1;
                static sf::Glsl::Vec2 size{63, 92};
                shader->m_vars.clear();
                shader->setVector4("uOutlineColor", &outlinecolor);
                shader->setEscalar("uThickness", &thickness);
                shader->setVector2("uTexSize", &size);
            }
        }
    }
}

// Dialogos
void SistemaDialogos(IVJ::IDialogo *dialogo, CE::Objeto &obj)
{
    if (!dialogo)
        return;
    dialogo->activo = true;
    dialogo->onInteractuar(obj);
}

bool revisarDistanciaInteraccion(CE::Objeto &jugador, CE::Objeto &npc, float distancia_maxima)
{
    auto pos_jugador = jugador.getTransformada()->posicion;
    auto pos_npc = npc.getTransformada()->posicion;
    float distancia = pos_jugador.distancia(pos_npc);
    return distancia <= distancia_maxima;
}

// DEBUG
void pintarLinea(CE::Vector2D &p1, CE::Vector2D &p2, const sf::Color &color)
{
    sf::CircleShape cp1{2.f};
    cp1.setFillColor({0, 0, 0, 255});
    sf::CircleShape cp2{2.f};
    cp2.setFillColor({0, 0, 0, 255});
    cp1.setPosition({p1.x, p1.y});
    cp2.setPosition({p2.x, p2.y});
    // pintar lineas de guía
    for (float t = 0; t <= 1; t += 0.0015)
    {
        sf::CircleShape pixel{1};
        pixel.setFillColor(color);
        auto puntopos = CE::lerp(p1, p2, t);
        pixel.setPosition({puntopos.x, puntopos.y});
        CE::Render::Get().AddToDraw(pixel);
    }
    CE::Render::Get().AddToDraw(cp1);
    CE::Render::Get().AddToDraw(cp2);
}




} // namespace IVJ
