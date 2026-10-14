# Laboratorio 6 $\rightarrow$ Camaras

## Creamos un componente ITriángulo

Antes de empezar la practica, creamos un componente que nos deje pintar un triángulo en donde se supone que estará el personaje.

**Juego/Componentes/IJComponentes.hpp**

```cpp
    class ITriangulo : public CE::IComponentes
    {
        public:
            explicit ITriangulo(float r, float ang);
            ~ITriangulo() override{};
            std::shared_ptr<IComponentes> clonar() const override
            {
                return std::make_shared<IMaquinaEstado>(*this);
            };
        public:
            sf::CircleShape tri_shape;
            float angulo;
    };
```
**Juego/Componentes/IJComponentes.cpp**

```cpp
    ITriangulo::ITriangulo(float r, float ang)
        :CE::IComponentes{},
        tri_shape{r,3},
        angulo{ang}
    {
        tri_shape.setOrigin({r/2.f,r/2.f});
    }
```

## Clase Entidad

Como el componente de ITriangulo será parte de jugador, jugador es una entidad, por lo que debemos definir que hacer con el componente de ITriangulo, en el update de entidad podemos actualizar el ángulo de la dirección que debe apuntar el triángulo y en draw pintar el triángulo.

**Juego/objetos/Entidad.cpp**
```cpp
    void Entidad::onUpdate(float dt)
    {
    // revisamos que tenga el componente ITriángulo
        if(tieneComponente<ITriangulo>())
        {
            auto fig = getComponente<ITriangulo>();
            auto pos = getTransformada()->posicion;
            //dibujamos el triángulo en la posición correcta
            fig->tri_shape.setPosition({pos.x,pos.y});
            //aplicamos rotación dependiendo de la dirección
            auto r = sf::radians(fig->angulo);
            fig->tri_shape.setRotation(r);
        }
    //.. más código
    }

```
**Juego/objetos/Entidad.cpp**
```cpp
    void Entidad::draw(sf::RenderTarget &target, sf::RenderStates state) const
    {
        state.transform *= getTransform();
#if DEBUG
        if(tieneComponente<CE::IBoundingBox>())
        {
            auto escala = 1;//getComponente<CE::ISprite>()->escala;
            auto bound = getComponente<CE::IBoundingBox>();
            sf::RectangleShape box{{bound->tam.x*escala,bound->tam.y*escala}};
            box.setFillColor(sf::Color::Transparent);
            box.setOutlineColor(sf::Color::Red);
            box.setOutlineThickness(1.5f);
            box.setOrigin({bound->mitad.x*escala,bound->mitad.y*escala});
            auto pos = transform->posicion;
            box.setPosition({pos.x,pos.y});
            target.draw(box);
        }
#endif
        //Después del debug pintamos el triángulo
        if(tieneComponente<ITriangulo>())
        {
            auto fig = getComponente<ITriangulo>();
            target.draw(fig->tri_shape);
        }
    }
```

## Sistema Mover

Modificamos nuestra función para mover individualmente un entidad, le agregamos el factor de IControl y aquí mismo podemos calcular la dirección que nuestra entidad esta apuntando.

### IControl

|boton|propósito|valores|
|:---:|:-------:|:-----:|
|arr  |$\uparrow$| true$|$false|
|abj  |$\downarrow$| true$|$false|
|izq  |$\leftarrow$| true$|$false|
|der  |$\rightarrow$| true$|$false|
|run  |correr| true$|$false|
|jmp  |brincar| true$|$false|
|acc  |acción| true$|$false|
|sacc |acción secundaria| true$|$false|

La idea del control es que en los **sistemas revisen que boton esta presionado y realice una acción correspondiente** 

**Juego/Sistems/Sistemas.cpp**
```cpp
    void SistemaMover(const std::shared_ptr<CE::Objeto>& objeto, float dt)
    {
        auto trans = objeto->getTransformada();
        auto control = objeto->getComponente<CE::IControl>();

        //Si no tiene control o el control no esta activo salir
        if(!control || !control->isActivo()) return;

        //definimos cual va ser la velocidad correcta dependiendo
        //de que boton del control este presionado
        auto vel = CE::Vector2D{0.f,0.f};
        if(control->arr) vel.y = -trans->velocidad.y;
        if(control->abj) vel.y = trans->velocidad.y;
        if(control->der) vel.x = trans->velocidad.x;
        if(control->izq) vel.x = -trans->velocidad.x;
        
        //calculamos la dirección usando un triángulo rectángulo
        if(objeto->getComponente<ITriangulo>())
        {
            auto n = vel;
            n.normalizacion();
            if (vel.x!=0 || vel.y!=0)
                objeto->getComponente<ITriangulo>()->angulo = std::atan2(n.x,-n.y);
        }
        //actualizamos la posición del ente.
        trans->posicion.suma(vel.escala(dt));
    }
```

## Creamos una escena nueva llamada Escena_Camara

creamos la escena donde probaremos diferentes tipos de camaras.

**Crea los archivos Escena_Camara.hpp y Escena_Camara.cpp** dentro de folder de `src/Juego/Escenas/`

**Escena_Camara.hpp**
```cpp
#pragma once
#include <Motor/Primitivos/Escena.hpp>
namespace IVJ
{
    class Escena_Camara : public CE::Escena
    {
        public:
            explicit Escena_Camara(std::shared_ptr<Entidad>& pref);
            virtual ~Escena_Camara(){};
            void onInit() override;
            void onFinal() override;
            void onUpdate(float dt) override;
            void onInputs(const CE::Botones& accion) override;
            void onRender() override;
            std::shared_ptr<Entidad> getJugador() override {return jugador_ref;}
        private:
            int inicializar{1};
            std::shared_ptr<Entidad>& jugador_ref;
    };
}
```

**Escena_Camara.cpp**
```cpp
#include "Escena_Camara.hpp"
#include <Motor/Inputs/Botones.hpp>
#include <Motor/Render/Render.hpp>
#include <memory>
namespace IVJ
{
    Escena_Camara::Escena_Camara(std::shared_ptr<Entidad>& pref)
        :CE::Escena{},jugador_ref{pref}
    {
    }
    void Escena_Camara::onInit()
    {
        if(!inicializar) return;

        registrarBotones(sf::Keyboard::Scancode::W,"arriba");
        registrarBotones(sf::Keyboard::Scancode::Up,"arriba");
        registrarBotones(sf::Keyboard::Scancode::S,"abajo");
        registrarBotones(sf::Keyboard::Scancode::Down,"abajo");
        registrarBotones(sf::Keyboard::Scancode::A,"izquierda");
        registrarBotones(sf::Keyboard::Scancode::Left,"izquierda");
        registrarBotones(sf::Keyboard::Scancode::D,"derecha");
        registrarBotones(sf::Keyboard::Scancode::Right,"derecha");
        registrarBotones(sf::Keyboard::Scancode::Enter,"aceptar");

        inicializar=false;
    }
    void Escena_Camara::onFinal()
    {
    }
    void Escena_Camara::onUpdate(float dt)
    {
        jugador_ref->onUpdate(dt);
        for(auto& obj: objetos.getPool())
        {
            obj->onUpdate(dt);
        }
    }
    void Escena_Camara::onInputs(const CE::Botones& accion)
    {
        switch(accion.getTipo())
        {
            case CE::Botones::TipoAccion::OnPress:
            {
                break;
            }
            case CE::Botones::TipoAccion::OnRelease:
            {
                break;
            }
            case CE::Botones::TipoAccion::None:
            {
                break;
            }
        }
    }
    void Escena_Camara::onRender()
    {
        CE::Render::Get().AddToDraw(*jugador_ref);
        for(auto& obj: objetos.getPool())
            CE::Render::Get().AddToDraw(*obj);
    }
}
```


Para probar vamos agregar una **figura** y **un control** como componente a el jugador (este objeto viene desde Juego.cpp) en nuestro caso es jugador_ref

**Escena_Camara.cpp**
```cpp
#include "Escena_Camara.hpp"
#include "Motor/Utils/Vector2D.hpp"
#include <Motor/Inputs/Botones.hpp>
#include <Motor/Render/Render.hpp>
#include <Juego/objetos/Entidad.hpp>
#include <Motor/Componentes/IComponentes.hpp>
#include <Juego/Componentes/IJComponentes.hpp>
#include <memory>
namespace IJV
{
//...
    void Escena_Camara::onInit()
    {
        if(!inicializar) return;

        registrarBotones(sf::Keyboard::Scancode::W,"arriba");
        registrarBotones(sf::Keyboard::Scancode::Up,"arriba");
        registrarBotones(sf::Keyboard::Scancode::S,"abajo");
        registrarBotones(sf::Keyboard::Scancode::Down,"abajo");
        registrarBotones(sf::Keyboard::Scancode::A,"izquierda");
        registrarBotones(sf::Keyboard::Scancode::Left,"izquierda");
        registrarBotones(sf::Keyboard::Scancode::D,"derecha");
        registrarBotones(sf::Keyboard::Scancode::Right,"derecha");
        registrarBotones(sf::Keyboard::Scancode::Enter,"aceptar");

        auto trans = jugador_ref->getTransformada();
        trans->velocidad = CE::Vector2D{120.f,120.f};
        jugador_ref->setPosicion(300.f,300.f);

        auto triangulo = std::make_shared<ITriangulo>(40.f,0.f);
        triangulo->tri_shape.setFillColor(sf::Color(255,191,0));
        triangulo->tri_shape.setOutlineColor(sf::Color::Black);
        triangulo->tri_shape.setOutlineThickness(3.f);

        jugador_ref->addComponente(triangulo);
        jugador_ref->addComponente(std::make_shared<CE::IControl>());

        inicializar=false;
    }
//...

```

**Agregamos la escena a Juego.cpp** para que nos aparezca en el gesto de escenas y podamos ligarla con otras escenas. **En la escena menu hay que cambiar el enter a que nos direccione a la escena de la camara**

**Juego.cpp**
```cpp
    // incluir Juego/Escenas/Escena_Camara.hpp
    void Juego::OnInit(void)
    {
        std::cout<<"Inicializando Juego\n";
        
        
        CE::GestorEscenas::Get().registrarEscena("Menu",std::make_shared<Escena_Menu>());
        //ESTA
        CE::GestorEscenas::Get().registrarEscena("ECamara",std::make_shared<Escena_Camara>(jugador));

        //crear el jugador
        jugador = std::make_shared<Entidad>();
        jugador->getStats()->hp = 100;

        CE::GestorEscenas::Get().cambiarEscena("Menu"); //ejecuta onInit()A
        escena_actual = &CE::GestorEscenas::Get().getEscenaActual();
        
    }

```

**Si compilamos nos debe de salir la siguiente imagen**

![](https://external-content.duckduckgo.com/iu/?u=http%3A%2F%2Fdrive.google.com/uc?id=1_QYFFlUlZGblEz3aitObT8nEZGY_F9O_)


## Agregamos el sistema mover

En la función update agregamos el sistema mover y movemos al jugador

**Escena_Camara.cpp**
```cpp
    void Escena_Camara::onUpdate(float dt)
    {
        jugador_ref->onUpdate(dt);
        SistemaMover(jugador_ref,dt);
        for(auto& obj: objetos.getPool())
        {
            obj->onUpdate(dt);
        }
    }
```

### Agregamos el esquema del control

mapeamos el esquema a IControl en la función de onInputs

```cpp
    void Escena_Camara::onInputs(const CE::Botones& accion)
    {
        switch(accion.getTipo())
        {
            case CE::Botones::TipoAccion::OnPress:
            {   
                if(accion.getNombre() == "arriba")
                {
                    jugador_ref->getComponente<CE::IControl>()->arr=true;
                }
                if(accion.getNombre() == "abajo") 
                {
                    jugador_ref->getComponente<CE::IControl>()->abj=true;
                }
                if(accion.getNombre() == "derecha") 
                {
                    jugador_ref->getComponente<CE::IControl>()->der=true;
                }
                if(accion.getNombre() == "izquierda") 
                {
                    jugador_ref->getComponente<CE::IControl>()->izq=true;
                }
                break;
            }
            case CE::Botones::TipoAccion::OnRelease:
            {
                if(accion.getNombre() == "arriba")
                {
                    jugador_ref->getComponente<CE::IControl>()->arr=false;
                }
                if(accion.getNombre() == "abajo") 
                {
                    jugador_ref->getComponente<CE::IControl>()->abj=false;
                }
                if(accion.getNombre() == "derecha") 
                {
                    jugador_ref->getComponente<CE::IControl>()->der=false;
                }
                if(accion.getNombre() == "izquierda") 
                {
                    jugador_ref->getComponente<CE::IControl>()->izq=false;
                }
                break;
            }
            case CE::Botones::TipoAccion::None:
            {
                break;
            }
        }
    }
```

### Probamos los controles

Al compilar debemos poder mover el triángulo con W,S,A,D o flechas

![](https://external-content.duckduckgo.com/iu/?u=http%3A%2F%2Fdrive.google.com/uc?id=1KZKOej4SnE8yc3XPtnhEG2cp7w6K2KYq)

## Donde creamos las camaras

Para salvaguardar que de las camaras se creen antes de acceder a las escenas, lo más recomendable es que se creeen en **Juego.cpp** de esa forma se cargan antes de ejecutar la escena.

## Motor/Camaras/Camara.hpp y .cpp

Camara es una clase abstracta con comportamiento (camara estática), en donde la vamos usar para expander sus capacidades creando hijas de esa clase y polimorfismo. 

El motor utilizá **Un gestor de camaras GestorCamaras** de la misma forma que se utiliza el GestorEscenas, para gestionar cual camara mostrar/utilizar

Observemos que contiene la clase Camara.

```cpp
class Camara
{
    public:
        Camara(float x, float y,float w, float h);              //posición y dimensión de la camara
        Camara(const Vector2D& pos, const Vector2D& dim);
        virtual ~Camara(){};
        [[nodiscard]]ITransform& getTransformada();
        [[nodiscard]]sf::View& getView() const;
        void lockEnObjeto(const std::shared_ptr<Objeto>& obj);  //referencia del ente que queremos seguir/sensar.
        void setViewSize(float x, float y);                     //cambiar las dimensiones
        virtual void onUpdate(float dt);                        //cada camara implementa como se actualiza el estado del objeto camara
    public:
        float cam_width;
        float cam_height;
        static int num_camaras;
        std::string nombre;
        bool esta_activa;
    protected:
        std::shared_ptr<sf::View> m_view;
        std::weak_ptr<Objeto> m_lockObj;
        std::shared_ptr<ITransform> m_transform;
};
```

**Si queremos hacer nuestra propia camara, solamente tenemos que sobre escribir el método de onUpdate**

Observemos que contiene la clase GestorCamara (singleton), del gestor lo más probable esque solamente usemos *agregarCamara, setCamaraActiva, getCamaraActiva o getCamara* ya que los démas métodos se utilizan en la capa más abajo del motor.

**GestorCamaras**
```cpp
    class GestorCamaras
    {
        public:
            static GestorCamaras& Get();
            void agregarCamara(const std::shared_ptr<Camara>& nueva);
            void setCamaraActiva(int id);
            void onUpdateCamaras(float dt);
            void onRenderCamara(sf::RenderTexture &tex);
            [[nodiscard]]Camara& getCamaraActiva()
            [[nodiscard]]Camara& getCamara(int id)
            [[nodiscard]]std::vector<std::shared_ptr<Camara>>& getListaCamaras()
        private:
            static inline GestorCamaras* instancia = nullptr;
            std::weak_ptr<Camara> m_camaraActiva;
            std::vector<std::shared_ptr<Camara>> m_listaCamaras;
    };
```

### Camara con interpolación (LERP)

El motor ya tiene implementada una camara que usa la interpolación lineal, vamos a realizar el proceso de crear una camara y agregarla a nuestra escena.

En **Juego.cpp** vamos a instanciar una camara de tipo LERP

En el constructor le pasamos la dimensión de la camara, entre más chica, va tener el efecto de zoom, y entre más grande lo contrario

```cpp
    void Juego::OnInit(void)
    {
        std::cout<<"Inicializando Juego\n";
        CE::GestorEscenas::Get().registrarEscena("Prueba",std::make_shared<Escena_Prueba>(jugador));
        CE::GestorEscenas::Get().registrarEscena("SS",std::make_shared<Escena_SS>(jugador));
        CE::GestorEscenas::Get().registrarEscena("Menu",std::make_shared<Escena_Menu>());
        CE::GestorEscenas::Get().registrarEscena("ECamara",std::make_shared<Escena_Camara>(jugador));

        //crear el jugador
        jugador = std::make_shared<Entidad>();
        jugador->getStats()->hp = 100;

        CE::GestorEscenas::Get().cambiarEscena("Menu"); //ejecuta onInit()A
        escena_actual = &CE::GestorEscenas::Get().getEscenaActual();

        //creamos la Camara
        CE::GestorCamaras::Get().agregarCamara(
                std::make_shared<CE::CamaraLERP>(
                    CE::Vector2D{0.f,0.f}, //posición
                    CE::Vector2D{1024,720} //dimensiones
                    )
                );
    }
```

**En nuestra GUI nos debe aparecer en la sección de camaras, la camara LERP** si seleccionamos la camara y seleccionamos un objeto, la camara lo va seguir. Sin embargo esto solo funciona en DEBUG mode, para el juego en si, tenemos que especificar por codigo a quien seguir.


![](https://external-content.duckduckgo.com/iu/?u=http%3A%2F%2Fdrive.google.com/uc?id=1BP92eJEg1g1TbCL4RdI7ChuwqHkybSpv)
Para asignarle por código hay que lockear el objeto a la camara. 

**Escena_Camara.cpp**
```cpp
    void Escena_Camara::onInit()
    {
        //la asiignamos antes, porque podemos entrar y salir de escenas
        //así la camara regresa a la que debería en la escena
        //ID = 1 = LERP es en el orden que se agreguen
        CE::GestorCamaras::Get().setCamaraActiva(1);
        //le decimos a quien persigue
        CE::GestorCamaras::Get().getCamaraActiva().lockEnObjeto(jugador_ref);
        if(!inicializar) return;
        registrarBotones(sf::Keyboard::Scancode::W,"arriba");
        registrarBotones(sf::Keyboard::Scancode::Up,"arriba");
        registrarBotones(sf::Keyboard::Scancode::S,"abajo");
        registrarBotones(sf::Keyboard::Scancode::Down,"abajo");
        registrarBotones(sf::Keyboard::Scancode::A,"izquierda");
        registrarBotones(sf::Keyboard::Scancode::Left,"izquierda");
        registrarBotones(sf::Keyboard::Scancode::D,"derecha");
        registrarBotones(sf::Keyboard::Scancode::Right,"derecha");
        registrarBotones(sf::Keyboard::Scancode::Enter,"aceptar");
        auto trans = jugador_ref->getTransformada();
        trans->velocidad = CE::Vector2D{120.f,120.f};
        jugador_ref->setPosicion(300.f,300.f);
        auto triangulo = std::make_shared<ITriangulo>(40.f,0.f);
        triangulo->tri_shape.setFillColor(sf::Color(255,191,0));
        triangulo->tri_shape.setOutlineColor(sf::Color::Black);
        triangulo->tri_shape.setOutlineThickness(3.f);
        jugador_ref->addComponente(triangulo);
        jugador_ref->addComponente(std::make_shared<CE::IControl>());
        inicializar=false;
    }
    void Escena_Camara::onFinal()
    {
        //reseteamos la camara a la estática al salir/cambiar de escena
        CE::GestorCamaras::Get().setCamaraActiva(0);
    }
```

![](https://external-content.duckduckgo.com/iu/?u=http%3A%2F%2Fdrive.google.com/uc?id=1GO-0qNqXFCey-4vy-9VklJC8a4M9RNTy)


## Implementar una Camara nueva

Para hacer eso podemos crear archivos nuevos o en el de `Motor/Camaras/Camras.hpp y cpp`, en este caso vamos a implementar una camara con ventana de activación.

### Camara Snap Ventana

Esta camara tiene una ventana en medio, cuando el jugador trata de salirse de esa ventana, entonces la camara se mueve dejando al jugador atrapado en la ventana.

```
______________
|  _______   |
|  |     |   |
|  |  j  |   |
|  -------   |
|____________|

```

**Motor/Camaras/Camara.hpp**
Para esta implementación vamos ocupar las dimensiones de la ventana, colocarla en el centro de la camara.

```cpp
class CamaraSnapVentana: public Camara
{
    public:
        CamaraSnapVentana(const Vector2D& pos, const Vector2D& dim, const Vector2D& dven);
        ~CamaraSnapVentana() override{};
        void onUpdate(float dt) override;
    public:
        Vector2D m_vdim;
    private:
        bool lock{false};
};

```

**Motor/Camaras/Camara.cpp**
Tenemos que revisar los limietes izquierdos, derechos, arriba y abajo de la ventana, cada vez que el jugador sobre pase esos límites, ajustamos la camara por la diferencia de la distancia que se sobrepaso.

```cpp
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
    //verificaciones
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
``` 

### Prueba CamaraSnapVentana

Para probar la ventana vamos a agregar unos rectángulos que actuaran como montañas en el horizonte, para poder apreciar que se esta moviendo la camara.

**Juego/Juego.cpp** Primero agregamos la nueva camara

```cpp
    void Juego::OnInit(void)
    {
        std::cout<<"Inicializando Juego\n";
        CE::GestorEscenas::Get().registrarEscena("Prueba",std::make_shared<Escena_Prueba>(jugador));
        CE::GestorEscenas::Get().registrarEscena("SS",std::make_shared<Escena_SS>(jugador));
        CE::GestorEscenas::Get().registrarEscena("Menu",std::make_shared<Escena_Menu>());
        CE::GestorEscenas::Get().registrarEscena("ECamara",std::make_shared<Escena_Camara>(jugador));

        //crear el jugador
        jugador = std::make_shared<Entidad>();
        jugador->getStats()->hp = 100;

        CE::GestorEscenas::Get().cambiarEscena("Menu"); //ejecuta onInit()A
        escena_actual = &CE::GestorEscenas::Get().getEscenaActual();

        CE::GestorCamaras::Get().agregarCamara(
                std::make_shared<CE::CamaraLERP>(
                    CE::Vector2D{0.f,0.f}, //posición
                    CE::Vector2D{1024,720} //dimensiones
                    )
                );
        //SnapVentana
        CE::GestorCamaras::Get().agregarCamara(
                std::make_shared<CE::CamaraSnapVentana>(
                    CE::Vector2D{0.f,0.f}, //posición
                    CE::Vector2D{1024,720}, //dimensiones
                    CE::Vector2D{500,300} //ventana
                    )
                );
    }
```

**Juego/Escenas/Escena_Camara.cpp** Aquí agregamos los montes y asignamos la camara 2 como la principal (2 porque es la segunda que agregamos)

```cpp
void Escena_Camara::onInit()
{
    //la asiignamos antes, porque podemos entrar y salir de escenas
    //así la camara regresa a la que debería en la escena
    //ID = 1 = LERP es en el orden que se agreguen
    CE::GestorCamaras::Get().setCamaraActiva(2);
    //le decimos a quien persigue
    CE::GestorCamaras::Get().getCamaraActiva().lockEnObjeto(jugador_ref);
    if(!inicializar) return;
    registrarBotones(sf::Keyboard::Scancode::W,"arriba");
    registrarBotones(sf::Keyboard::Scancode::Up,"arriba");
    registrarBotones(sf::Keyboard::Scancode::S,"abajo");
    registrarBotones(sf::Keyboard::Scancode::Down,"abajo");
    registrarBotones(sf::Keyboard::Scancode::A,"izquierda");
    registrarBotones(sf::Keyboard::Scancode::Left,"izquierda");
    registrarBotones(sf::Keyboard::Scancode::D,"derecha");
    registrarBotones(sf::Keyboard::Scancode::Right,"derecha");
    registrarBotones(sf::Keyboard::Scancode::Enter,"aceptar");

    auto trans = jugador_ref->getTransformada();
    trans->velocidad = CE::Vector2D{500.f,500.f};
    jugador_ref->setPosicion(300.f,300.f);
    auto triangulo = std::make_shared<ITriangulo>(40.f,0.f);
    triangulo->tri_shape.setFillColor(sf::Color(255,191,0));
    triangulo->tri_shape.setOutlineColor(sf::Color::Black);
    triangulo->tri_shape.setOutlineThickness(3.f);
    jugador_ref->addComponente(triangulo);
    jugador_ref->addComponente(std::make_shared<CE::IControl>());

    //objetos para que se muestre el movimiento
    int montes_count=100;
    float dstd= 20.f;
    for(int i=0;i<montes_count;i++)
    {
        //gauss (talves se tiene que agregar cmath)
        double por = std::exp(-0.5*(((i- montes_count/2.f)*(i-montes_count/2.f))/(dstd*dstd))); 
        int inc = 200;
        auto monte = std::make_shared<Rectangulo>(
                    200.f,200.f+(inc*por),
                    sf::Color{184, 134, 11},sf::Color::Black);
        monte->setPosicion(100+(i*200),100-(inc*por/2.f));
        objetos.agregarPool(monte);
    }
    inicializar=false;
}
```

En el onRender vamos a pintar la ventana solo cuando estemos en modo debug

```cpp
void Escena_Camara::onRender()
{
    for(auto& obj: objetos.getPool())
        CE::Render::Get().AddToDraw(*obj);
    CE::Render::Get().AddToDraw(*jugador_ref);

#if DEBUG
    auto cam = &CE::GestorCamaras::Get().getCamaraActiva();
    auto csv = (CE::CamaraSnapVentana*)cam;
    auto csvpos = csv->getTransformada().posicion;
    sf::RectangleShape debugcam{{csv->m_vdim.x,csv->m_vdim.y}};
    debugcam.setOrigin({csv->m_vdim.x/2.f,csv->m_vdim.y/2.f});
    debugcam.setPosition({csvpos.x,csvpos.y});
    debugcam.setOutlineThickness(3.f);
    debugcam.setOutlineColor(sf::Color::Yellow);
    debugcam.setFillColor(sf::Color::Transparent);
    CE::Render::Get().AddToDraw(debugcam);
#endif
}
```
Al compilar nos debe dar lo siguiente:

*cuando el jugador se sobrepase de los límites la camara se movera no dejando escapar al jugador.


![](https://external-content.duckduckgo.com/iu/?u=http%3A%2F%2Fdrive.google.com/uc?id=1vgJuVJmfxUmgX3h835wp9OPdIaQB7e20)
