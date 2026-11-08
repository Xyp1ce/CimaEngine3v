#include "Escena_shaders.hpp"
#include <Juego/Maquinas/yusm/EstadoIdleYu.hpp>
#include <Juego/Sistemas/Sistemas.hpp>
#include <Juego/objetos/Entidad.hpp>
#include <Motor/Camaras/CamarasGestor.hpp>
#include <Motor/Primitivos/GestorAssets.hpp>
#include <Motor/Render/Render.hpp>

namespace IVJ
{
    Escena_shaders::Escena_shaders(std::shared_ptr<Entidad> &ref)
        : CE::Escena{}, player{ref}
    {
        nombre = "Shade";
    }

    void Escena_shaders::onInit()
    {
        if (!inicializar)
        {
            if (!player->tieneComponente<CE::IShader>())
            {
                player->getListaComponentes().clear();
                player->addComponente(
                          std::make_shared<CE::ISprite>(
                              CE::GestorAssets::Get().getTextura("yu1"),
                              494, 410, 1.f))
                    .addComponente(std::make_shared<CE::IControl>())
                    .addComponente(
                        std::make_shared<CE::IShader>(
                            "",
                            ASSETS "/shaders/yuioutline.frag"));
                auto me = std::make_shared<IVJ::IMaquinaEstado>();
                me->fsm = std::make_shared<EstadoIdleYu>(CE::Vector2D{496.f, 443.f}, 10, 0.15f);
                player->addComponente(me);
                player->setFSM(me->fsm);
                player->setPosicion(650.f, 250.f);
                player->getTransformada()->velocidad = CE::Vector2D{200.f, 200.f};
            }
            // la musica vuelve a tocar cuando volvemos a la escena
            CE::GestorAssets::Get().getMusica("heartbreak").play();
            return;
        }

        registrarBotones(sf::Keyboard::Scancode::W, "arriba");
        registrarBotones(sf::Keyboard::Scancode::Up, "arriba");
        registrarBotones(sf::Keyboard::Scancode::S, "abajo");
        registrarBotones(sf::Keyboard::Scancode::Down, "abajo");
        registrarBotones(sf::Keyboard::Scancode::A, "izquierda");
        registrarBotones(sf::Keyboard::Scancode::Left, "izquierda");
        registrarBotones(sf::Keyboard::Scancode::D, "derecha");
        registrarBotones(sf::Keyboard::Scancode::Right, "derecha");
        registrarBotones(sf::Keyboard::Scancode::Enter, "aceptar");
        registrarBotones(sf::Keyboard::Scancode::Space, "powerup");

        // cargar los sonidos
        CE::GestorAssets::Get().agregarMusica("heartbreak", ASSETS "/sonidos/lab10/Heartbreak.mp3");
        CE::GestorAssets::Get().agregarSonido("ki", ASSETS "/sonidos/lab10/ki2.mp3");
        // config, se repite cuando llega al final
        CE::GestorAssets::Get().getMusica("heartbreak").setLooping(true);
        CE::GestorAssets::Get().getSonido("ki").setLooping(true);

        CE::GestorAssets::Get().agregarTextura(
            "yu1",
            ASSETS "/sprites/yu_1.png",
            CE::Vector2D{0.f, 0.f},
            CE::Vector2D{4960.f, 2013.f});
        CE::GestorAssets::Get().agregarTextura(
            "hongo1",
            ASSETS "/sprites/Mushroom_1.png",
            CE::Vector2D{0.f, 0.f},
            CE::Vector2D{49.f, 41.f});

        // objeto
        auto hongo = std::make_shared<Entidad>();
        int hpadding = 100;
        hongo->addComponente(std::make_shared<CE::ISprite>(
                                 CE::GestorAssets::Get().getTextura("hongo1"),
                                 49, 41, 2.f))
            .addComponente(std::make_shared<CE::IShader>(
                ASSETS "/shaders/particula.vert",
                ASSETS "/shaders/hongofrag1.frag"));
        hongo->getStats()->hp_max = 100;
        hongo->getStats()->hp = 100;
        hongo->setPosicion(150.f, 430.f);

        // AJUSTE DE TAMAÑO DEL HONGO PARA PINTAR UN POCO MÁS TRANSPARENTE
        sf::IntRect original{{0, 0}, {49, 41}};

        hongo->getComponente<CE::ISprite>()->m_sprite.setTextureRect(
            sf::IntRect{
                {original.position.x, original.position.y - hpadding},
                {original.size.x, original.size.y + hpadding}});

        sf::Vector2f tam(
            hongo->getComponente<CE::ISprite>()->m_sprite.getTexture().getSize());
        static sf::Glsl::Vec4 frame_real{
            original.position.x / tam.x,
            original.position.y / tam.y,
            (original.position.x + original.size.x) / tam.x,
            (original.position.y + original.size.y) / tam.y};

        hongo->getComponente<CE::ISprite>()->m_sprite.setOrigin({24.f, 20.5f + hpadding});
        hongo->getComponente<CE::IShader>()->setVector4("frame_real", &frame_real);

        player->getListaComponentes().clear();
        player->addComponente(
                  std::make_shared<CE::ISprite>(
                      CE::GestorAssets::Get().getTextura("yu1"),
                      494, 410, 1.f))
            .addComponente(std::make_shared<CE::IControl>())
            .addComponente(
                std::make_shared<CE::IShader>(
                    "",
                    ASSETS "/shaders/yuioutline.frag"));
        auto me = std::make_shared<IVJ::IMaquinaEstado>();
        me->fsm = std::make_shared<EstadoIdleYu>(CE::Vector2D{496.f, 443.f}, 10, 0.15f);

        player->addComponente(me);
        // ejecuta el onEntrar
        player->setFSM(me->fsm);

        player->setPosicion(650.f, 250.f);
        player->getTransformada()->velocidad = CE::Vector2D{200.f, 200.f};

        objetos.agregarPool(hongo);
        objetos.agregarPool(player);
        inicializar = false;
        CE::GestorAssets::Get().getMusica("heartbreak").play();
    }

    void Escena_shaders::onFinal()
    {
        // la musica se pausa cuando salimos de la escena
        CE::GestorAssets::Get().getMusica("heartbreak").pause();
    }

    void Escena_shaders::onInputs(const CE::Botones &accion)
    {
        // un estado del sonido para saber cuando estamos presionando el boton
        static bool presionado = false;
        switch (accion.getTipo())
        {
        case CE::Botones::TipoAccion::OnPress:
        {
            if (accion.getNombre() == "derecha")
            {
                player->getComponente<CE::IControl>()->der = true;
            }
            if (accion.getNombre() == "izquierda")
            {
                player->getComponente<CE::IControl>()->izq = true;
            }
            if (accion.getNombre() == "powerup")
            {
                player->getComponente<CE::IControl>()->jmp = true;
                auto glowRadio = (float *)player->getComponente<CE::IShader>()->m_vars["glowRadius"].second;
                *glowRadio = 20.f;
                player->getComponente<CE::IShader>()->setEscalar("glowRadius", glowRadio);
                // ejecutar sonido y cambiar a presionado
                if (!presionado)
                {
                    CE::GestorAssets::Get().getSonido("ki").play();
                    presionado = true;
                }
            }
            break;
        }
        case CE::Botones::TipoAccion::OnRelease:
        {
            if (accion.getNombre() == "derecha")
            {
                player->getComponente<CE::IControl>()->der = false;
            }
            if (accion.getNombre() == "izquierda")
            {
                player->getComponente<CE::IControl>()->izq = false;
            }
            if (accion.getNombre() == "powerup")
            {
                player->getComponente<CE::IControl>()->jmp = false;
                auto glowRadio = (float *)player->getComponente<CE::IShader>()->m_vars["glowRadius"].second;
                *glowRadio = 10.f;
                player->getComponente<CE::IShader>()->setEscalar("glowRadius", glowRadio);

                // apagar sonido y cambiar a no presionado
                if (presionado)
                {
                    CE::GestorAssets::Get().getSonido("ki").stop();
                    presionado = false;
                }
            }
            break;
        }
        case CE::Botones::TipoAccion::None:
        {
            break;
        }
        }
    }

    void Escena_shaders::onUpdate(float dt)
    {
        static float tiempo = 0;
        tiempo += dt;
        player->inputFSM();
        player->onUpdate(dt);
        SistemaMover(player, dt);
        //-1 porque el player es el ultimo  y a el lo queremos actualizar individualmente.
        for (size_t i = 0; i < objetos.getPool().size() - 1; i++)
        {
            auto ente = objetos.getPool()[i];
            ente->onUpdate(dt);
            if (ente->tieneComponente<CE::IShader>())
            {
                // los ponemos aquí para poder actualizarlos en tiempo real en la GUI
                // sin necesidad de picarle al boton de reload
                static sf::Glsl::Vec3 hcolor{226.f / 255.f, 22.f / 255.f, 100.f / 255.f};
                static sf::Glsl::Vec4 htinta{1.f, 0.f, 0.f, 1.f};
                static float tolerancia = 0.4f;
                static sf::Glsl::Vec2 htexel{1.f / 49.f, 1.f / 41.f};
                static float maxAltura = 80.f;
                static float velocidad = 0.25f;
                auto hongoShader = ente->getComponente<CE::IShader>();
                hongoShader->setEscalar("velocidad", &velocidad);
                hongoShader->setEscalar("maxAltura", &maxAltura);
                hongoShader->setVector3("colorTarget", &hcolor);
                hongoShader->setVector4("tinta", &htinta);
                hongoShader->setEscalar("tolerancia", &tolerancia);
                hongoShader->setVector2("texel", &htexel);
                hongoShader->setEscalar("tiempo", &tiempo);
            }
        }
    }

    void Escena_shaders::onRender()
    {
        for (auto obj : objetos.getPool())
            CE::Render::Get().AddToDraw(*obj);
    }

}
