# Laboratorio 5 Simulación

## Lab

En este laboratorio vamos a crear una simulación de selección natural (tipo presa depredador), antes que nada creamos una nueva escena llamada **Escena_sim.hpp y Escena_Sim.cpp** en el folder de **Juego/Escenas/**

Como vamos a simular?, vamos a crear entidades con las figuras y la idea de la simulación es que estas entidades sobrevivan durante muchas generaciones, en esta simulación existe un mundo el cual lo vamos a representar por medio de un **Rectangulo** blanco gigante, y las entidades serán un **Pentangono**, estas entidades para poder sobrevivir tienen que buscar y consumir comida las cuales las vamos a representar por medio de un **Circulo** .

* Las entidades deben buscar la comida que se encuentren más cerca de ellas.
* Una vez que encuentren la comida, deberán ir hacia la comida y agarrar la comida
* Una vez que tengan la comida deberan regresar a casa (punto inicial donde spwaniaron) para consumirla 
* Una vez que consuma la comida puede ir a buscar otra (repitiendo el ciclo)

**Después de 1 min de ejecución** termina la generación, **repetir todo el proceso hasta 100 generaciones y graficamos los resultados finales**

## Reglas

* Los estats del ente al inicio de la simulación son al azar.
* stats: energia, velocidad, alcance
* El ente se mueve hacia la comida a una velocidad constante 
* El **moverse consume energia**
* Cuando el ente **ya no tenga energia, deberá morir**
* Si el ente consigue consumir 2 o más de comida, en la sigiuente generación se reproduce (spwanea un ente más)
* El ente que spwanea deberá tener los mismos stats que el ente que lo spwaneo
* En cada generación habra 150 trozos de comida
* Al inicio de la simulación habrá 100 entes
* Cada generación va **durar 30 segundos o hasta que ya toda la comida se consumio**


## Graficas

* hay que graficar, los diferentes valores de energia, velocidad y alcance de cada ente que sobrevive al final de la simulación (tipo histograma)
* Durante las generaciones grafica los diferentes valores de energia, velocidad y alcance (tipo histograma)


## Archivo Juego/Figuras/Figuras.hpp

Para este ejercicio, necesitamos exponer la variable de tipo  **sf::CircleShape** del Pentagono (para saber hacia donde esta apuntando), para poder rotar la figura, por lo que vamos a hacer un setter de esa variable. (debemos regresar la referencia)

```cpp
// Pentagono
class Pentagono : public Figuras
{
  public:
    explicit Pentagono(float radio, const sf::Color &relleno, const sf::Color &contorno);
    ~Pentagono() override {};
    void draw(sf::RenderTarget &target, sf::RenderStates state) const override;
    void onUpdate(float dt) override;

    sf::CircleShape &getShape()
    {
        return circ_img;
    }

  private:
    sf::CircleShape circ_img;

  public:
    float radio;
};
```

## Archivo Juego/Escenas/Escena_Sim.hpp

```cpp
#pragma once
#include "Motor/Utils/Vector2D.hpp"
#include <Juego/Figuras/Figuras.hpp>
#include <Motor/Primitivos/Escena.hpp>
namespace IVJ
{
class Escena_Sim : public CE::Escena
{
  public:
    explicit Escena_Sim();
    virtual ~Escena_Sim() {};
    void onInit() override;
    void onFinal() override;
    void onUpdate(float dt) override;
    void onInputs(const CE::Botones &accion) override;
    void onRender() override;

  private:
    int inicializar{1};
    int entes_iniciales;
    int comida_iniciales;
    CE::Vector2D punto_inferior;
    CE::Vector2D punto_superior;
    std::shared_ptr<Rectangulo> mundo;
};
} // namespace IVJ
```

### Archivo Juego/Escenas/Escena_Sim.cpp

```cpp
#include "Escena_Sim.hpp"
#include <Juego/Componentes/IJComponentes.hpp>
#include <Juego/Figuras/Figuras.hpp>
#include <Juego/Sistemas/Sistemas.hpp>
#include <Juego/objetos/Texto.hpp>
#include <Motor/Primitivos/GestorAssets.hpp>
#include <Motor/Primitivos/GestorEscenas.hpp>
#include <Motor/Render/Render.hpp>
#include <memory>
#include <random>
namespace IVJ
{
Escena_Sim::Escena_Sim() : CE::Escena{}
{
    punto_inferior.x = 50.f;
    punto_inferior.y = 50.f;
    punto_superior.x = 1000.f;
    punto_superior.y = 650.f;
    entes_iniciales = 100;
    comida_iniciales = 150;

    float ancho = punto_superior.x - punto_inferior.x;
    float largo = punto_superior.y - punto_inferior.y;

    mundo = std::make_shared<Rectangulo>(ancho, largo, sf::Color::White, sf::Color::Black);

    float mx = punto_inferior.x + ancho / 2.f;
    float my = punto_inferior.y + largo / 2.f;

    mundo->setPosicion(mx, my);
}
void Escena_Sim::onInit()
{
    if (!inicializar)
        return;

    objetos.agregarPool(mundo);

    static std::random_device rd;
    static std::mt19937 rand(rd());
    float x;
    float y;
    float angulo;
    // comida
    // deben estar dentro del mundo
    for (int i = 0; i < comida_iniciales; i++)
    {
        x = std::uniform_real_distribution(100.f + punto_inferior.x, punto_superior.x - 100.f)(rand);
        y = std::uniform_real_distribution(100.f + punto_inferior.y, punto_superior.y - 100.f)(rand);
        auto comida = std::make_shared<Circulo>(5.f, sf::Color::Green, sf::Color::Green);
        comida->setPosicion(x, y);
        objetos.agregarPool(comida);
    }

    // entes iniciales
    // donde respawn la idea es sobre la linea del mundo
    for (int i = 0; i < entes_iniciales; i++)
    {
        const float num = std::uniform_real_distribution(0.0f, 1.0f)(rand);
        if (num <= 0.5) // abajo
        {
            y = punto_superior.y;
            angulo = 0.f;
        }
        else // arriba
        {
            y = punto_inferior.y;
            angulo = 180.f;
        }
        x = std::uniform_real_distribution(punto_inferior.x, punto_superior.x)(rand);

        auto ente = std::make_shared<Pentagono>(15.f, sf::Color::Red, sf::Color::Black);
        ente->setPosicion(x, y);
        //usamos el getter de la figura para rotarla
        ente->getShape().setRotation(sf::degrees(angulo));

        objetos.agregarPool(ente);
    }

    inicializar = false;
}
void Escena_Sim::onFinal()
{
}
void Escena_Sim::onUpdate(float dt)
{
    for (auto &obj : objetos.getPool())
    {
        obj->onUpdate(dt);
    }
}
void Escena_Sim::onInputs(const CE::Botones &accion)
{
    switch (accion.getTipo())
    {
    case CE::Botones::TipoAccion::OnPress: {

        break;
    }
    case CE::Botones::TipoAccion::OnRelease: {
        break;
    }
    case CE::Botones::TipoAccion::None: {
        break;
    }
    }
}
void Escena_Sim::onRender()
{
    for (auto &obj : objetos.getPool())
        CE::Render::Get().AddToDraw(*obj);
}
} // namespace IVJ
```

### Juego/Juego.cpp

agregamos la escena en juego.cpp y probamos.

```cpp
void Juego::OnInit(void)
{
    std::cout << "Inicializando Juego\n";

    // crear el jugador
    jugador = std::make_shared<Entidad>();
    jugador->getStats()->hp_max = 255;
    jugador->getStats()->hp = 255;
    jugador->getStats()->str = 255;
    jugador->getStats()->def = 255;
    jugador->getStats()->agi = 255;

    CE::GestorEscenas::Get().registrarEscena("Init", std::make_shared<Escena_Init>());
    CE::GestorEscenas::Get().registrarEscena("Menu", std::make_shared<Escena_Menu>());
    //agregamos la escena nueva, no se nos olvide #include <Juego/Escenas/Escena_Sim.hpp>
    CE::GestorEscenas::Get().registrarEscena("Sim", std::make_shared<Escena_Sim>());

    CE::GestorEscenas::Get().cambiarEscena("Menu"); // ejecuta onInit()A
    escena_actual = &CE::GestorEscenas::Get().getEscenaActual();
}

```

![](https://external-content.duckduckgo.com/iu/?u=http%3A%2F%2Fdrive.google.com/uc?id=1Y_WOtz4BOdOG3oOa5phclmtSA8C7a5n1)

## Generaciones

Para esto ocupamos un timer (como dice las reglas cada 30 segundos se termina la generación), en este caso usaremos el componente CE::ITimer que se encuentra en Motor/Componentes/IComponentes.hpp

También ocuparemos un contador de que generación se esta ejecutando.

**Juego/Escenas/Escena_Simp.hpp**
```cpp
//incluir #include <Motor/Componentes/IComponentes.hpp>

class Escena_Sim : public CE::Escena
{
  public:
    explicit Escena_Sim();
    virtual ~Escena_Sim() {};
    void onInit() override;
    void onFinal() override;
    void onUpdate(float dt) override;
    void onInputs(const CE::Botones &accion) override;
    void onRender() override;

  private:
    int inicializar{1};
    int entes_iniciales;
    int comida_iniciales;
    int generacion;
    bool generacion_termino;
    CE::ITimer timer_generacion;
    CE::Vector2D punto_inferior;
    CE::Vector2D punto_superior;
    std::shared_ptr<Rectangulo> mundo;
};
```

En el constructor los inicializamos, CE::ITimer se tiene que inicializar por lista de inicialización, **ITimer recibe tiempo máximo en frames**, en este caso 60 frames = 1 seg, por lo que $60*30$ para 30 segundos

**Juego/Escena/Escena_Simp.cpp**
```cpp
Escena_Sim::Escena_Sim() : CE::Escena{}, timer_generacion{60 * 30}
{
    punto_inferior.x = 50.f;
    punto_inferior.y = 50.f;
    punto_superior.x = 1000.f;
    punto_superior.y = 650.f;
    entes_iniciales = 100;
    comida_iniciales = 150;
    generacion = 0;
    generacion_termino = false;

    float ancho = punto_superior.x - punto_inferior.x;
    float largo = punto_superior.y - punto_inferior.y;

    mundo = std::make_shared<Rectangulo>(ancho, largo, sf::Color::White, sf::Color::Black);

    float mx = punto_inferior.x + ancho / 2.f;
    float my = punto_inferior.y + largo / 2.f;

    mundo->setPosicion(mx, my);
}
```

## Entidades Stats

**Todo CE::Objeto** tiene un componente llamado **stats** ( lo pueden ver en Motor/Componentes/IComponentes.hpp), lo vamos utilizar para simular los stats que necesitamos.

Para este caso vamos a usar lo siguiente:
* hp_max = energia máxima
* hp = energia actual
* agi = alcance

**Porque usar este componente** el pool ligado con la escena esta relaconado con los stats del CE::Objeto, ya que si el hp es 0 o menor, el pool automáticamente borrara el objeto , y esa es la razón de utilizarlo.


Con eso en cuenta, vamos a cambiar el orden en que agregamos entes al pool, como el mundo y la comida no tiene energia, cada nueva generación los vamos agergar. y de una vez registramos el boton que usaremos para seguir con otras generaciones.


Para eso vamos a crear una nueva función para el CE::Pool, en donde nos permita hacer inserción enfrente del arreglo. Por el momento es una operación lenta pero como se ejecuta una sola vez en el init, no importa

**Motor/Primitivos/CEPool.hpp**

Solamente agregar el prototipo de la función en el hpp

```cpp
class Pool
{
  public:
    /**
     * @brief Constructor del pool.
     * @param size Tamaño inicial/máximo sugerido del pool
     */
    Pool(int size);

    /**
     * @brief Agrega un objeto al pool.
     * @param obj Puntero compartido al objeto a agregar
     */
    void agregarPool(const std::shared_ptr<Objeto> &obj);
    //AGREGAR ESTA FUNCIÓN
    void agregarPoolEnfrete(const std::shared_ptr<Objeto> &obj);

```
**Motor/Primitivos/CEPool.cpp**

Implementamos el prototipo

```cpp
void Pool::agregarPoolEnfrete(const std::shared_ptr<Objeto> &obj)
{
    // es lento
    objetos.insert(objetos.begin(), obj);
}
```

**Juego/Escenas/Escena_Simp.cpp**

Cambiamos el orden de hacer el agregar a pool en el init de nuestra escena

```cpp
void Escena_Sim::onInit()
{
    std::random_device rd;
    std::mt19937 rand(rd());
    float x;
    float y;
    float angulo;
    // comida
    // deben estar dentro del mundo
    for (int i = 0; i < comida_iniciales; i++)
    {
        x = std::uniform_real_distribution(100.f + punto_inferior.x, punto_superior.x - 100.f)(rand);
        y = std::uniform_real_distribution(100.f + punto_inferior.y, punto_superior.y - 100.f)(rand);
        auto comida = std::make_shared<Circulo>(5.f, sf::Color::Green, sf::Color::Green);
        comida->setPosicion(x, y);
        objetos.agregarPoolEnfrete(comida);
    }
    objetos.agregarPoolEnfrete(mundo);

    if (!inicializar)
        return;

    registrarBotones(sf::Keyboard::Scancode::Enter, "Ok");

    // entes iniciales
    // donde respawn la idea es sobre la linea del mundo
    for (int i = 0; i < entes_iniciales; i++)
    {
        const float num = std::uniform_real_distribution(0.0f, 1.0f)(rand);
        if (num <= 0.5) // abajo
        {
            y = punto_superior.y;
            angulo = 0.f;
        }
        else // arriba
        {
            y = punto_inferior.y;
            angulo = 180.f;
        }
        x = std::uniform_real_distribution(punto_inferior.x, punto_superior.x)(rand);

        auto ente = std::make_shared<Pentagono>(15.f, sf::Color::Red, sf::Color::Black);
        auto &stats = ente->getStats();
        auto &vel = ente->getTransformada()->velocidad;
        vel.x = std::uniform_real_distribution(1.f, 10.f)(rand);
        vel.y = std::uniform_real_distribution(1.f, 10.f)(rand);
        stats->hp_max = 100;
        stats->hp = 100;
        stats->agi = std::uniform_int_distribution(10, 255)(rand);
        ente->setPosicion(x, y);
        ente->getShape().setRotation(sf::degrees(angulo));

        objetos.agregarPool(ente);
    }

    inicializar = false;
}

```

**Probamos las generaciones, al azar asignamos el hp en 0 a algunos entes**

Agregamos un ITexto para mostrar un mensaje cuando termine una generación

El ciclo update, si ya termino la generación borramos los entes muertos, si no contamos y actualizamos los entes

En el render si ya termino la generación mostramos el mensaje de texto "Presiona Enter para continuar", de lo contrario
renderizamos el mundo.


**Agregamos el texto**

**Juego/Escenas/Escena_Sim.hpp**
```cpp
#pragma once
#include "Motor/Utils/Vector2D.hpp"
#include <Juego/Figuras/Figuras.hpp>
#include <Motor/Componentes/IComponentes.hpp>
#include <Motor/Primitivos/Escena.hpp>
namespace IVJ
{
class Escena_Sim : public CE::Escena
{
  public:
    explicit Escena_Sim();
    virtual ~Escena_Sim() {};
    void onInit() override;
    void onFinal() override;
    void onUpdate(float dt) override;
    void onInputs(const CE::Botones &accion) override;
    void onRender() override;

  private:
    int inicializar{1};
    int entes_iniciales;
    int comida_iniciales;
    int generacion;
    bool generacion_termino;
    CE::ITimer timer_generacion;
    //Agregar este atributo
    CE::ITexto mensaje;
    CE::Vector2D punto_inferior;
    CE::Vector2D punto_superior;
    std::shared_ptr<Rectangulo> mundo;
};
} // namespace IVJ
```

**Juego/Escenas/Escena_Sim.cpp**

El ITexto ocupa **un font** y el texto que se va desplegar, **el motor ya tiene 2 fonts por defecto** una es *default_font* y la otra es *nova*, más a delante veremos como agregar un font nuevo, pero practicamente ocupamos un archivo .ttf y usaer el gestor de assets para registrarlo.

```cpp
Escena_Sim::Escena_Sim()
    : CE::Escena{}, timer_generacion{60 * 10},
      mensaje{CE::GestorAssets::Get().getFont("default_font"), "Presiona Enter para continuar"}
{
    punto_inferior.x = 50.f;
    punto_inferior.y = 50.f;
    punto_superior.x = 1000.f;
    punto_superior.y = 650.f;
    entes_iniciales = 100;
    comida_iniciales = 150;
    generacion = 0;
    generacion_termino = false;

    float ancho = punto_superior.x - punto_inferior.x;
    float largo = punto_superior.y - punto_inferior.y;

    mundo = std::make_shared<Rectangulo>(ancho, largo, sf::Color::White, sf::Color::Black);

    float mx = punto_inferior.x + ancho / 2.f;
    float my = punto_inferior.y + largo / 2.f;

    mundo->setPosicion(mx, my);

    mensaje.m_texto.setPosition({mx, my});
    mensaje.m_texto.setFillColor(sf::Color::Yellow);
}
```

**Cambiamos el update y el input, en el update si no a terminado la generación hay que actualizar todo y en el input al presionar enter que se ejecute el onInit de la escena**, así mismo el render si ya termino de la generación mostrar el mensaje de lo contrario la simulación.

```cpp
//agregar #include <Motor/GUI/GLogger.hpp> para el logger
void Escena_Sim::onUpdate(float dt)
{
    if (generacion_termino)
    {
        // borrar los entes que ya murieron
        // boorarPool itera todos los objetos y va eliminando
        // a los objetos que stats->hp <= 0, en este caso
        // hp puede ser la energia.
        objetos.borrarPool();
    }
    else
    {
        for (auto &obj : objetos.getPool())
        {
            obj->onUpdate(dt);
        }

        timer_generacion.frame_actual++;
        if (timer_generacion.frame_actual >= timer_generacion.frame_maximo)
        {
            generacion_termino = true;
            timer_generacion.frame_actual = 0;
        }
    }

    CE::GLogger::Get().agregarLog(std::to_string(timer_generacion.frame_actual) + "/" +
                                      std::to_string(timer_generacion.frame_maximo),
                                  CE::GLogger::Niveles::LOG_SEVERO);
}

void Escena_Sim::onInputs(const CE::Botones &accion)
{
    switch (accion.getTipo())
    {
    case CE::Botones::TipoAccion::OnPress: {

        break;
    }
    case CE::Botones::TipoAccion::OnRelease: {

        if (accion.getNombre() == "Ok" && generacion_termino == true)
        {
            generacion_termino = false;
            this->onInit();
        }
        break;
    }
    case CE::Botones::TipoAccion::None: {
        break;
    }
    }
}

void Escena_Sim::onRender()
{
    if (generacion_termino)
    {
        // render texto
        CE::Render::Get().AddToDraw(mensaje.m_texto);
    }
    else
    {
        for (auto &obj : objetos.getPool())
            CE::Render::Get().AddToDraw(*obj);
    }
}
```

Si ejecutamos, y esperamos a que pasen los 30 seg ( en la imagen le puse 10 seg para probar), entonces saldra el mensaje para presionar enter y resetear la comida 

![](https://external-content.duckduckgo.com/iu/?u=http%3A%2F%2Fdrive.google.com/uc?id=1QRGHI-sS_c6iWecjhhlt3irb1wcA0kSn)

![](https://external-content.duckduckgo.com/iu/?u=http%3A%2F%2Fdrive.google.com/uc?id=1fdeXBiXyZ_KUy52jaW5Gmebs0nhCoeF9)

**agregamos lo de la prueba de los entes al azar que se mueran** se van a eliminar del pool hasta que termine la generación

```cpp
void Escena_Sim::onUpdate(float dt)
{
    if (generacion_termino)
    {
        // borrar los entes que ya murieron
        // boorarPool itera todos los objetos y va eliminando
        // a los objetos que stats->hp <= 0, en este caso
        // hp puede ser la energia.
        objetos.borrarPool();
    }
    else
    {
        for (auto &obj : objetos.getPool())
        {
            obj->onUpdate(dt);
            // prueba al azar de entes muriendo
            std::random_device rd;
            std::mt19937 rand(rd());
            float prob = std::uniform_real_distribution(0.f, 1.f)(rand);
            if (prob < 0.0002) // matamos ente
            {
                int id = std::uniform_int_distribution(0, (int)objetos.getPool().size() - 1)(rand);
                auto &ente = objetos.getPool()[id];
                auto &stats = ente->getStats();
                stats->hp = 0;
                // se puede repetir el id pero solo al final de la simulación lo borra
                std::cout << prob << " murio ente id ->" << id << "\n";
            }
        }

        timer_generacion.frame_actual++;
        if (timer_generacion.frame_actual >= timer_generacion.frame_maximo)
        {
            generacion_termino = true;
            timer_generacion.frame_actual = 0;
        }
    }

    CE::GLogger::Get().agregarLog(std::to_string(timer_generacion.frame_actual) + "/" +
                                      std::to_string(timer_generacion.frame_maximo),
                                  CE::GLogger::Niveles::LOG_SEVERO);
}
```

## Entidades buscar

Ya con la base funcionando ya podemos empezar a armar nuestra simulación, creando los sistemas necesarios

Vamos a crear un componente llamado IEstadoInterno y esto nos va ayudar a llevar el control de que debe hacer el ente, por el momento le asignaremos 5 estados:
* Buscar
* Movimiento a comida
* Movimiento a casa
* Comsumir
* Reproducirse

El componente lo haremos en **Juego/Componentes/IJComponentes.hpp y .cpp**

```cpp
class IEstadoInterno : public CE::IComponentes
{
  public:
    enum class Estados
    {
        BUSCAR,
        ENMOVIMIENTOCOMIDA,
        ENMOVIMIENTOCASA,
        CONSUMIR,
        REPRODUCIR // se agrego para reproducir
    };

  public:
    explicit IEstadoInterno(const IEstadoInterno::Estados &estado_inicial);
    virtual ~IEstadoInterno() override {};
    std::shared_ptr<IComponentes> clonar() const override
    {
        return std::make_shared<IEstadoInterno>(*this);
    };

    void setEstadoInterno(const IEstadoInterno::Estados &ne);
    IEstadoInterno::Estados getEstadoInterno() const
    {
        return estado;
    };

  private:
    IEstadoInterno::Estados estado;
};
```

```cpp
// IEstadoInterno
IEstadoInterno::IEstadoInterno(const IEstadoInterno::Estados &estado_inicial)
    : CE::IComponentes{}, estado{estado_inicial}
{
}
void IEstadoInterno::setEstadoInterno(const IEstadoInterno::Estados &ne)
{
    estado = ne;
}
```

También necesitamos un componente de inventario en donde pueda cargar la comida de regreso a casa (un inventario de 1 espacio)

```cpp
class IInventarioComida : public CE::IComponentes
{
  public:
    virtual ~IInventarioComida() override {};
    void guardarComida(const std::shared_ptr<Circulo> &c);
    void sacarComida();
    std::weak_ptr<Circulo> getComidaGuardada() const
    {
        return comida;
    };
    std::shared_ptr<IComponentes> clonar() const override
    {
        return std::make_shared<IInventarioComida>(*this);
    };

  private:
    std::weak_ptr<Circulo> comida;
};
```

```cpp
// IInventarioComida
//
void IInventarioComida::guardarComida(const std::shared_ptr<Circulo> &c)
{
    if (comida.lock() == nullptr)
        comida = c;
}

void IInventarioComida::sacarComida()
{
    if (comida.lock() != nullptr)
        comida.reset();
}
```

También vamos a crear un componente que le indique al ente cual es su target y llevar un control cuando el target no este disponible cambiar de estado.

```cpp
class ITargetComida : public CE::IComponentes
{
  public:
    virtual ~ITargetComida() override {};
    void setTargetComida(const std::shared_ptr<Circulo> &c);
    void quitarTarget();
    std::weak_ptr<Circulo> getTargetComida() const
    {
        return target;
    };
    std::shared_ptr<IComponentes> clonar() const override
    {
        return std::make_shared<ITargetComida>(*this);
    };

  private:
    std::weak_ptr<Circulo> target;
};
```

```cpp
void ITargetComida::setTargetComida(const std::shared_ptr<Circulo> &c)
{
    if (target.lock() == nullptr)
        target = c;
}
void ITargetComida::quitarTarget()
{
    if (target.lock() != nullptr)
        target.reset();
}
```

También vamos agregar un componente de posicion inicial

```cpp
class IPosicionInicial : public CE::IComponentes
{
  public:
    explicit IPosicionInicial(float x, float y);
    virtual ~IPosicionInicial() override {};
    std::shared_ptr<IComponentes> clonar() const override
    {
        return std::make_shared<IPosicionInicial>(*this);
    };

  public:
    CE::Vector2D pos_init;
};
```
```cpp
// IPosicionInicial
//
IPosicionInicial::IPosicionInicial(float x, float y) : CE::IComponentes{}, pos_init{x, y}
{
}
```

Para la comida ocupamos de si un ente ya lo recogio o no, en este casi haremos un componente de ITieneDueño, solo con un boleano

```cpp
class ITieneDueño : public CE::IComponentes
{
  public:
    virtual ~ITieneDueño() override {};
    std::shared_ptr<IComponentes> clonar() const override
    {
        return std::make_shared<ITieneDueño>(*this);
    };

  public:
    bool tiene{false};
};
```

Con estos componentes podemos hacer el sistema de buscar, mientras el ente este en el estado buscar y no tenga comida en el inventario, entonces busca la comida más cercana a el, una vez adquirida el target, no lo cambia hasta que agarre la comida u otro ente le gane la comida.

**Juego/Sistemas/Sistemas.hpp y cpp**
```cpp
// BuscarComida
[[maybe_unused]] void SistemaBuscarComida(CE::Objeto &ente, const std::vector<std::shared_ptr<CE::Objeto>> &objetos);
```

```cpp
void SistemaBuscarComida(CE::Objeto &ente, const std::vector<std::shared_ptr<CE::Objeto>> &objetos)
{
    // comida y mundo son un ente pero no tienen estos componentes, por eso salir temprano.
    if (!ente.tieneComponente<IEstadoInterno>() && !ente.tieneComponente<IInventarioComida>())
        return;
    // si ya tiene una comida en mente, no ocupamos buscar
    if (ente.getComponente<ITargetComida>()->getTargetComida().lock() != nullptr)
        return;
    // si no esta buscando que no busque
    if (ente.getComponente<IEstadoInterno>()->getEstadoInterno() != IEstadoInterno::Estados::BUSCAR)
        return;

    // ahora si buscamos una comida lo más cerca posible
    std::shared_ptr<Circulo> comida_mas_cerca = nullptr;
    float dist_min = 9999999999999.f;
    for (auto &objeto : objetos)
    {
        // verificamos si el objeto es Circulo, de lo contrario no nos interesa como comida
        // usamos la libreria estandar para hacer este calculo
        std::shared_ptr<Circulo> comida = std::dynamic_pointer_cast<Circulo>(objeto);
        // no es comida
        if (!comida)
            continue;
        // tiene dueño
        if (comida->getComponente<ITieneDueño>()->tiene)
            continue;
        // calculamos la distancia
        auto mi_pos = ente.getTransformada()->posicion;
        float dist = mi_pos.distancia(comida->getTransformada()->posicion);
        if (dist < dist_min)
        {
            dist_min = dist;
            comida_mas_cerca = comida;
        }
    }
    // std::cout << "Ente  " << ente.getNombre()->nombre << " encontro: " << comida_mas_cerca->getNombre()->nombre <<
    // "\n";
    ente.getComponente<ITargetComida>()->setTargetComida(comida_mas_cerca);
    ente.getComponente<IEstadoInterno>()->setEstadoInterno(IEstadoInterno::Estados::ENMOVIMIENTO);
}

```

**Juego/Escenas/Escena_Sim.cpp**

Ahora agregamos los componentes y utilizamos el sistema, para probar que si funciona correctamente, vamos a pintar una linea del ente a la comida seleccionada.

```cpp
void Escena_Sim::onInit()
{
    std::random_device rd;
    std::mt19937 rand(rd());
    float x;
    float y;
    float angulo;
    // comida
    // deben estar dentro del mundo
    for (int i = 0; i < comida_iniciales; i++)
    {
        x = std::uniform_real_distribution(100.f + punto_inferior.x, punto_superior.x - 100.f)(rand);
        y = std::uniform_real_distribution(100.f + punto_inferior.y, punto_superior.y - 100.f)(rand);
        auto comida = std::make_shared<Circulo>(5.f, sf::Color::Green, sf::Color::Green);
        //agregar si tiene dueño
        comida->addComponente(std::make_shared<ITieneDueño>());
        comida->setPosicion(x, y);
        objetos.agregarPoolEnfrete(comida);
    }
    objetos.agregarPoolEnfrete(mundo);

    if (!inicializar)
        return;

    registrarBotones(sf::Keyboard::Scancode::Enter, "Ok");

    // entes iniciales
    // donde respawn la idea es sobre la linea del mundo
    for (int i = 0; i < entes_iniciales; i++)
    {
        const float num = std::uniform_real_distribution(0.0f, 1.0f)(rand);
        if (num <= 0.5) // abajo
        {
            y = punto_superior.y;
            angulo = 0.f;
        }
        else // arriba
        {
            y = punto_inferior.y;
            angulo = 180.f;
        }
        x = std::uniform_real_distribution(punto_inferior.x, punto_superior.x)(rand);

        auto ente = std::make_shared<Pentagono>(15.f, sf::Color::Red, sf::Color::Black);
        auto &stats = ente->getStats();
        auto &vel = ente->getTransformada()->velocidad;
        vel.x = std::uniform_real_distribution(1.f, 10.f)(rand);
        vel.y = std::uniform_real_distribution(1.f, 10.f)(rand);
        stats->hp_max = 100;
        stats->hp = 100;
        stats->agi = std::uniform_int_distribution(10, 255)(rand);
        ente->setPosicion(x, y);
        ente->getShape().setRotation(sf::degrees(angulo));
        // guardar el ángulo para la reproducción
        ente->getTransformada()->angulo = angulo;

        // agregar estadointerno inicial
        ente->addComponente(std::make_shared<IEstadoInterno>(IEstadoInterno::Estados::BUSCAR))
            // agregar inventario
            .addComponente(std::make_shared<IInventarioComida>())
            // agregar target nulo
            .addComponente(std::make_shared<ITargetComida>())
            // agregar la posicion inicial
            .addComponente(std::make_shared<IPosicionInicial>(x, y));

        objetos.agregarPool(ente);
    }

    inicializar = false;
}
```

Borramos el código de agregar al azar un ente con 0 energia. como si funciona borrar ya no lo necesitamos, Solamente llamamos nuestro sistema

```cpp
void Escena_Sim::onUpdate(float dt)
{
    if (generacion_termino)
    {
        // borrar los entes que ya murieron
        // boorarPool itera todos los objetos y va eliminando
        // a los objetos que stats->hp <= 0, en este caso
        // hp puede ser la energia.
        objetos.borrarPool();
    }
    else
    {
        for (auto &obj : objetos.getPool())
        {
            obj->onUpdate(dt);
            SistemaBuscarComida(*obj, objetos.getPool());
        }

        timer_generacion.frame_actual++;
        if (timer_generacion.frame_actual >= timer_generacion.frame_maximo)
        {
            generacion_termino = true;
            timer_generacion.frame_actual = 0;
        }
    }

    CE::GLogger::Get().agregarLog(std::to_string(timer_generacion.frame_actual) + "/" +
                                      std::to_string(timer_generacion.frame_maximo),
                                  CE::GLogger::Niveles::LOG_SEVERO);
}
```

finalmente para debugear pintamos una linea del ente a la comida.

```cpp
void Escena_Sim::onRender()
{
    if (generacion_termino)
    {
        // render texto
        CE::Render::Get().AddToDraw(mensaje.m_texto);
        for (auto &obj : objetos.getPool())
        {
            if (obj->tieneComponente<IEstadoInterno>() && obj->tieneComponente<ITargetComida>() &&
                obj->tieneComponente<IInventarioComida>())
            {
                // resetear
                obj->getComponente<IEstadoInterno>()->setEstadoInterno(IEstadoInterno::Estados::BUSCAR);
                obj->getComponente<ITargetComida>()->quitarTarget();
                obj->getComponente<IInventarioComida>()->sacarComida();
                auto pos_init = obj->getComponente<IPosicionInicial>()->pos_init;
                obj->setPosicion(pos_init.x, pos_init.y);
            }
        }
    }
    else
    {
        for (auto &obj : objetos.getPool())
            CE::Render::Get().AddToDraw(*obj);

        // DEBUG
        for (auto &obj : objetos.getPool())
        {
            // solo ente tiene ITarget, comida ni mundo lo tienen
            if (!obj->tieneComponente<ITargetComida>())
                continue;
            auto target = obj->getComponente<ITargetComida>();
            // si aún no tiene target, no pintamos la linea
            if (!target->getTargetComida().lock())
                continue;
            auto po = obj->getTransformada()->posicion;
            auto pt = target->getTargetComida().lock()->getTransformada()->posicion;

            sf::VertexArray linea(sf::PrimitiveType::Lines, 2);
            linea[0].position = sf::Vector2f({po.x, po.y});
            linea[0].color = sf::Color::Black;
            linea[1].position = sf::Vector2f({pt.x, pt.y});
            linea[1].color = sf::Color::Black;
            CE::Render::Get().AddToDraw(linea);
        }
    }
}
```

debe salir algo parecido:


![](https://external-content.duckduckgo.com/iu/?u=http%3A%2F%2Fdrive.google.com/uc?id=1wQlaBy13MoCUxD6viqGZG1Js-X2rTmLL)

## Entidades mover

Ahora vamos a crear los sistemas, empecemos con MoveraComidaoCasa , esta función dependiendo del estado , moverá hacia la comida o a casa o mandará a buscar

en **Juego/Sistamas/Sistemas.hpp y cpp**

```cpp
// Mover a comida o casa
[[maybe_unused]] void SistemaMoveraComidaoCasa(CE::Objeto &ente, float dt);
```

```cpp
void SistemaMoveraComidaoCasa(CE::Objeto &ente, float dt)
{
    // comida y mundo no tienen IEstadoInterno
    if (!ente.tieneComponente<IEstadoInterno>())
        return;

    // aún esta buscando
    if (ente.getComponente<IEstadoInterno>()->getEstadoInterno() != IEstadoInterno::Estados::ENMOVIMIENTOCASA &&
        ente.getComponente<IEstadoInterno>()->getEstadoInterno() != IEstadoInterno::Estados::ENMOVIMIENTOCOMIDA)
        return;

    // desplazarse hacia casa o comida
    auto estado = ente.getComponente<IEstadoInterno>()->getEstadoInterno();
    switch (estado)
    {
    case IEstadoInterno::Estados::ENMOVIMIENTOCOMIDA: {
        auto po = ente.getTransformada()->posicion;
        auto vo = ente.getTransformada()->velocidad;
        std::shared_ptr<Circulo> target = ente.getComponente<ITargetComida>()->getTargetComida().lock();
        // no tiene target
        if (!target)
        {
            ente.getComponente<IEstadoInterno>()->setEstadoInterno(IEstadoInterno::Estados::BUSCAR);
            ente.getComponente<ITargetComida>()->quitarTarget();
            return;
        }
        auto pt = target->getTransformada()->posicion;
        auto dir = pt - po;
        float dx = po.x + (dir.x * vo.x * dt);
        float dy = po.y + (dir.y * vo.y * dt);
        ente.setPosicion(dx, dy);
        float dist = po.distancia(pt);
        if (target->getComponente<ITieneDueño>()->tiene)
        {
            ente.getComponente<IEstadoInterno>()->setEstadoInterno(IEstadoInterno::Estados::BUSCAR);
            ente.getComponente<ITargetComida>()->quitarTarget();
        }
        else if (dist < 0.15)
        {
            // si target no tiene dueño, ser el dueño del target y guardar en inventario
            ente.getComponente<IEstadoInterno>()->setEstadoInterno(IEstadoInterno::Estados::ENMOVIMIENTOCASA);
            ente.getComponente<IInventarioComida>()->guardarComida(target);
            target->getComponente<ITieneDueño>()->tiene = true;
        }
    }
    break;
    case IEstadoInterno::Estados::ENMOVIMIENTOCASA: {
        auto po = ente.getTransformada()->posicion;
        auto vo = ente.getTransformada()->velocidad;
        auto pt = ente.getComponente<IPosicionInicial>()->pos_init;
        auto dir = pt - po;
        float dx = po.x + (dir.x * vo.x * dt);
        float dy = po.y + (dir.y * vo.y * dt);
        ente.setPosicion(dx, dy);
        float dist = po.distancia(pt);
        // mover comida con el ente
        auto comida = ente.getComponente<IInventarioComida>()->getComidaGuardada().lock();
        comida->setPosicion(dx, dy);
        if (dist < 0.15)
            ente.getComponente<IEstadoInterno>()->setEstadoInterno(IEstadoInterno::Estados::CONSUMIR);
    }
    break;
    default:
        break;
    }
}
```

compilamos y debemos tener algo parecido a esto ( los entes van a comida y si otro se la gana, busca otra comida)

![](https://external-content.duckduckgo.com/iu/?u=http%3A%2F%2Fdrive.google.com/uc?id=1cHEWSuf8eY2FM-ODohPk-yIcJ5NfvdGZ)

## Entidades consumir

para consumir, debemos hacer un componente de score, en donde si el ente consume 2 ó más comidas entonces nacerá un ente nuevo con las mismas características que el ente original.

**Juego/Componentes/IJComponentes.hpp**

```cpp
class IScore : public CE::IComponentes
{
  public:
    virtual ~IScore() override {};
    std::shared_ptr<IComponentes> clonar() const override
    {
        return std::make_shared<IScore>(*this);
    };
  public:
    int score{0};
};
```

Le agregamos el componente a los entes

**Juego/Escenas/Escena_Sim.cpp**

```cpp
void Escena_Sim::onInit()
{
    ...
    ...
    ...
    for (int i = 0; i < entes_iniciales; i++)
    {
        ..
        ..
        ..
        ..
        // agregar estadointerno inicial
        ente->addComponente(std::make_shared<IEstadoInterno>(IEstadoInterno::Estados::BUSCAR))
            // agregar inventario
            .addComponente(std::make_shared<IInventarioComida>())
            // agregar target nulo
            .addComponente(std::make_shared<ITargetComida>())
            // agregar la posicion inicial
            .addComponente(std::make_shared<IPosicionInicial>(x, y))
            // agregar score
            .addComponente(std::make_shared<IScore>());

        objetos.agregarPool(ente);
    }

    inicializar = false;
}
```

para consumir la comida, **el ente debe de llegar a su posición inicial** y consumir implica: quitar la comida  del inventario, quitar la comida  del target y marcarlo para eliminación (aún que este por defecto ya esta), también incrementamos el score más 1

**Juego/Sistemas/sistemas.hpp y cpp**

```cpp
// consumir comida
[[maybe_unused]] void SistemaConsumirComida(CE::Objeto &ente);
```

```cpp
void SistemaConsumirComida(CE::Objeto &ente)
{
    // comida y mundo no tiene IEstadoInterno ni ITargetComida
    if (!ente.tieneComponente<IEstadoInterno>() && !ente.tieneComponente<ITargetComida>())
        return;
    // si no tiene target
    if (ente.getComponente<ITargetComida>()->getTargetComida().lock() == nullptr)
        return;
    // si no esta en estado consumir
    if (ente.getComponente<IEstadoInterno>()->getEstadoInterno() != IEstadoInterno::Estados::CONSUMIR)
        return;

    // si llega aquí es que esta en el estado consumir y solo puede estar en este estado si ya llego a la posición
    // inicial. quitar comida del inventario
    ente.getComponente<IInventarioComida>()->sacarComida();
    // quitar comida del target del ente
    ente.getComponente<ITargetComida>()->quitarTarget();
    // no quitar propetario de comida, para que nadie lo busque, y por defecto ya esta marcado para borrar
    // incrementar score
    ente.getComponente<IScore>()->score++;
    // cambiar estado a buscar nueva comida
    ente.getComponente<IEstadoInterno>()->setEstadoInterno(IEstadoInterno::Estados::REPRODUCIR);
}
```

Llamamos nuestra función en **Juego/Escenas/Escena_Sim.cpp** y de una vez marcamos a los entes que no obtuvieron comida como muertos para la siguiente generacion

Es necesario mover algunas cosas de orden, por ejemplo el marcar y borrar los entes, se deben ejecutar al moento de presionar la tecla enter, de lo contrario va a borrar todo, por lo que en elupdate quitaremos el borrar el pool

```cpp
void Escena_Sim::onUpdate(float dt)
{
    if (!generacion_termino)
    {
        for (auto &obj : objetos.getPool())
        {
            obj->onUpdate(dt);
            SistemaBuscarComida(*obj, objetos.getPool());
            SistemaMoveraComidaoCasa(*obj, dt);
            SistemaConsumirComida(*obj);
            // SistemaReproducirEnte(*obj);
        }

        timer_generacion.frame_actual++;
        if (timer_generacion.frame_actual >= timer_generacion.frame_maximo)
        {
            generacion_termino = true;
            timer_generacion.frame_actual = 0;
        }
    }

    CE::GLogger::Get().agregarLog(std::to_string(timer_generacion.frame_actual) + "/" +
                                      std::to_string(timer_generacion.frame_maximo),
                                  CE::GLogger::Niveles::LOG_SEVERO);
}
```

En onInputs

```cpp
void Escena_Sim::onInputs(const CE::Botones &accion)
{
    switch (accion.getTipo())
    {
    case CE::Botones::TipoAccion::OnPress: {

        break;
    }
    case CE::Botones::TipoAccion::OnRelease: {

        if (accion.getNombre() == "Ok" && generacion_termino == true)
        {
            generacion_termino = false;
            // borrar para la siguiente generación y resetear
            for (auto &obj : objetos.getPool())
            {
                if (obj->tieneComponente<IEstadoInterno>() && obj->tieneComponente<ITargetComida>() &&
                    obj->tieneComponente<IInventarioComida>() && obj->tieneComponente<IScore>())
                {
                    // resetear
                    obj->getComponente<IEstadoInterno>()->setEstadoInterno(IEstadoInterno::Estados::BUSCAR);
                    obj->getComponente<ITargetComida>()->quitarTarget();
                    obj->getComponente<IInventarioComida>()->sacarComida();
                    auto pos_init = obj->getComponente<IPosicionInicial>()->pos_init;
                    obj->setPosicion(pos_init.x, pos_init.y);
                    // marcar muerto la siguiente generación
                    if (obj->getComponente<IScore>()->score == 0)
                    {
                        obj->getStats()->hp = 0;
                    }
                    obj->getComponente<IScore>()->score = 0;
                }
            }
            objetos.borrarPool();
            this->onInit();
        }
        break;
    }
    case CE::Botones::TipoAccion::None: {
        break;
    }
    }
}
```

El render que solo renderice, (se movio lo de resetear a cuando presionamos el enter)

```cpp
void Escena_Sim::onRender()
{
    if (generacion_termino)
    {
        // render texto
        CE::Render::Get().AddToDraw(mensaje.m_texto);
    }
    else
    {
        for (auto &obj : objetos.getPool())
            CE::Render::Get().AddToDraw(*obj);

        // DEBUG
        for (auto &obj : objetos.getPool())
        {
            if (!obj->tieneComponente<ITargetComida>())
                continue;
            auto target = obj->getComponente<ITargetComida>();
            if (!target->getTargetComida().lock())
                continue;
            auto po = obj->getTransformada()->posicion;
            auto pt = target->getTargetComida().lock()->getTransformada()->posicion;

            sf::VertexArray linea(sf::PrimitiveType::Lines, 2);
            linea[0].position = sf::Vector2f({po.x, po.y});
            linea[0].color = sf::Color::Black;
            linea[1].position = sf::Vector2f({pt.x, pt.y});
            linea[1].color = sf::Color::Black;
            CE::Render::Get().AddToDraw(linea);
        }
    }
}
```


![](https://external-content.duckduckgo.com/iu/?u=http%3A%2F%2Fdrive.google.com/uc?id=1dBaV14XgCwzNPcMT3xIqU51K9E1qu8kw)

## Entidades Reproducir

para este sistema, vamos a verificar si el ente consiguio 2 comidas para reproducirse,la idea es que cada 2 comidas se reproduzca.

por ejemplo si tiene 1 comida, no se reproduce, si tiene 2 si, si tiene 3 no, si tiene 4 si, si tiene 5 no etc..

en **Juego/Sistemas/Sistemas.hpp y cpp**

```cpp
// reproducir si  tienen 2 o más comidas
[[maybe_unused]] void SistemaReproducirEnte(CE::Objeto &ente, CE::Pool &pool);
```

```cpp
void SistemaReproducirEnte(CE::Objeto &ente, CE::Pool &pool)
{

    //no es ente
    if (!ente.tieneComponente<IScore>() && !ente.tieneComponente<IEstadoInterno>())
        return;

    // es ente pero aún no esta en la fase de reproducirse
    if (ente.getComponente<IEstadoInterno>()->getEstadoInterno() != IEstadoInterno::Estados::REPRODUCIR)
        return;

    // cambiar estado a buscar nueva comida 
    ente.getComponente<IEstadoInterno>()->setEstadoInterno(IEstadoInterno::Estados::BUSCAR);

    // no tiene suficiente score
    if (ente.getComponente<IScore>()->score < 2)
        return;
    
    // tiene suficiente score pero no es multiplo de 2
    if (ente.getComponente<IScore>()->score % 2 != 0)
        return;
    // si tiene un score mayor a 2 y es multiplo de 2 y es un ente, entonces lo reproducimos con los mismos stats y
    // guardamos en el pool de la escena
    auto nuevo = std::make_shared<Pentagono>(15.f, sf::Color::Blue, sf::Color::Black);
    nuevo->getStats()->hp_max = ente.getStats()->hp_max;
    nuevo->getStats()->hp = ente.getStats()->hp;
    nuevo->getStats()->agi = ente.getStats()->agi;
    auto pos_init = ente.getComponente<IPosicionInicial>()->pos_init;
    nuevo->getTransformada()->velocidad = ente.getTransformada()->velocidad;
    nuevo->getTransformada()->angulo = ente.getTransformada()->angulo;
    nuevo->getShape().setRotation(sf::degrees(nuevo->getTransformada()->angulo));
    nuevo->setPosicion(pos_init.x, pos_init.y);
    // componentes
    nuevo
        ->addComponente(std::make_shared<IEstadoInterno>(IEstadoInterno::Estados::BUSCAR))
        // agregar inventario
        .addComponente(std::make_shared<IInventarioComida>())
        // agregar target nulo
        .addComponente(std::make_shared<ITargetComida>())
        // agregar la posicion inicial
        .addComponente(std::make_shared<IPosicionInicial>(pos_init.x, pos_init.y))
        // agregar score
        .addComponente(std::make_shared<IScore>());
    pool.agregarPool(nuevo);
}
```

En **Juego/Escenas/Escena_Sim.cpp** agregamos el sistema en el update.

```cpp
void Escena_Sim::onUpdate(float dt)
{
    if (!generacion_termino)
    {
        for (auto &obj : objetos.getPool())
        {
            obj->onUpdate(dt);
            SistemaBuscarComida(*obj, objetos.getPool());
            SistemaMoveraComidaoCasa(*obj, dt);
            SistemaConsumirComida(*obj);
            SistemaReproducirEnte(*obj, objetos);
        }

        timer_generacion.frame_actual++;
        if (timer_generacion.frame_actual >= timer_generacion.frame_maximo)
        {
            generacion_termino = true;
            timer_generacion.frame_actual = 0;
        }
    }

    CE::GLogger::Get().agregarLog(std::to_string(timer_generacion.frame_actual) + "/" +
                                      std::to_string(timer_generacion.frame_maximo),
                                  CE::GLogger::Niveles::LOG_SEVERO);
    CE::GLogger::Get().agregarLog("Entes: " + std::to_string(objetos.getPool().size()),
                                  CE::GLogger::Niveles::LOG_SEVERO);
}
```

Si compilamos y ejecutamos tendremos algo similar.

![](https://external-content.duckduckgo.com/iu/?u=http%3A%2F%2Fdrive.google.com/uc?id=1AAJ_0j0zVBcCg2OCqIszOWrqfy-Pl6fR)


## Graficar usando la GUI 

vamos a aprender a implementar cosas con ImGui siguiendo el patrón de la GUI del motor.

primero vamos agregar el score en el panel de propiedades de los objetos

**Motor/GUI/GPropiedades.hpp y cpp**

agregamos una función privada de **panelIScore**
```cpp
class GPropiedades : public GLayer
{
 ....
 ....
 ...
  private:
    bool panelIMaquinaEstado(IVJ::IMaquinaEstado *me);

    bool panelIScore(IVJ::IScore *componente);
    ...     
    ...
    ...
};
} // namespace CE

```

**en el cpp***
```cpp
bool GPropiedades::panelIScore(IVJ::IScore *componente)
{
    if (!componente)
        return false;

    ImGui::TextColored(ImVec4(0, 255, 0, 255), "###IScore");

    int score = componente->score;
    ImGui::InputInt("Score", &score);

    return true;
}
//la llamamos en OnRender

void GPropiedades::OnRender(void)
{
    if (objeto_select)
    {
        // revisar si tiene las interfaces y mostrar las
        // propiedades adecuadas, de cajon tiene transformada
        ImGui::Begin("Objeto Propiedades", NULL, 0);

        ImGui::Separator();
        ImGui::Text("%s", objeto_select->getNombre()->nombre.c_str());

        // lab9 colisiones
        if (panelStats())
            ImGui::Separator();
        if (panelTransformada())
            ImGui::Separator();
        if (panelSprite(objeto_select->getComponente<ISprite>()))
            ImGui::Separator();
        if (panelIControl(objeto_select->getComponente<IControl>()))
            ImGui::Separator();
        if (panelShaders(objeto_select->getComponente<IShader>()))
            ImGui::Separator();
        if (panelIPaths(objeto_select->getComponente<IPaths>()))
        {
            guipath->OnRender();
        }
        if (panelIMaquinaEstado(objeto_select->getComponente<IVJ::IMaquinaEstado>()))
        {
            ImGui::Separator();
        }
        if (panelIScore(objeto_select->getComponente<IVJ::IScore>()))
            ImGui::Separator();

        ImGui::End();
    }

    // debería tilemap ser un ente u objeto
    if (tilemap_select)
    {
        ImGui::Begin("TileMap Propiedades", NULL, 0);

        if (panelShaders(tilemap_select->getTilemapShader()))
            ImGui::Separator();

        ImGui::End();
    }
}
```

Si ejecutamos al finalizar o durante la simulación podemos ver cuanto tienen de score.


![](https://external-content.duckduckgo.com/iu/?u=http%3A%2F%2Fdrive.google.com/uc?id=1-NSju3juvO1YdqxajOhoo4LNVpC9CFnE)

una vez que tengamos el panel  vamos hacer una clase en **Motor/GUI/** llamada GHistograma.hpp y cpp

```cpp
#pragma once
#include "../GUI/GLayer.hpp"
#include "../Primitivos/Objetos.hpp"
#include "../Utils/Utils.hpp"
#include <vector>

namespace CE
{
class GHistograma : public GLayer
{
  public:
    struct Histograma
    {
        std::vector<float> bins; // conteo por bin
        float minVal = 0.0f;
        float maxVal = 1.0f;
        float binWidth = 0.0f;
        float peak = 0.0f;
    };

  public:
    ~GHistograma() override {};

    void OnInit(const MotorConfig &des) override;
    void OnUpdate(float dt) override;
    void OnRender(void) override;

  private:
    GHistograma::Histograma crearHistograma(const std::vector<std::shared_ptr<CE::Objeto>> &objetos, int bins);
    GHistograma::Histograma histograma;
    size_t total{0};
    size_t getEntesSim(const std::vector<std::shared_ptr<CE::Objeto>> &objetos);
};
} // namespace CE
```

**cpp**
```cpp
#include "GHistograma.hpp"
#include <Juego/Componentes/IJComponentes.hpp>
#include <Juego/Escenas/Escena_Sim.hpp>
#include <Motor/Primitivos/GestorEscenas.hpp>
#include <algorithm>
#include <cmath>
#include <imgui.h>
#include <memory>
#include <vector>

namespace CE
{
void GHistograma::OnInit(const MotorConfig &des)
{
    (void)des;
}
void GHistograma::OnUpdate(float dt)
{
    if (total == getEntesSim(CE::GestorEscenas::Get().getEscenaActual().getPool().getPool()))
        return;
    total = this->getEntesSim(CE::GestorEscenas::Get().getEscenaActual().getPool().getPool());
    histograma = crearHistograma(CE::GestorEscenas::Get().getEscenaActual().getPool().getPool(), 10);
}
void GHistograma::OnRender(void)
{
    if (ImGui::Begin("Estadísticas"))
    {

        char overlay[64];
        snprintf(overlay, sizeof(overlay), "n=%zu  [%.2f, %.2f]", total, histograma.minVal, histograma.maxVal);

        ImGui::PlotHistogram("##hist", histograma.bins.data(), (int)histograma.bins.size(),
                             0,                                     // offset
                             overlay, 0.0f, histograma.peak * 1.1f, // escala Y
                             ImVec2(-1, 150));                      // -1 = ancho disponible
    }
    ImGui::End();
}
size_t GHistograma::getEntesSim(const std::vector<std::shared_ptr<CE::Objeto>> &objetos)
{
    size_t cuenta = 0;
    for (auto &ente : objetos)
    {
        if (!ente->tieneComponente<IVJ::IScore>())
            continue;

        cuenta++;
    }
    return cuenta;
}
GHistograma::Histograma GHistograma::crearHistograma(const std::vector<std::shared_ptr<CE::Objeto>> &objetos, int bins)
{
    GHistograma::Histograma hist;

    if (objetos.size() <= 0 || bins <= 0)
        return hist;
    std::vector<int> data;
    for (auto &ente : objetos)
    {
        if (!ente->tieneComponente<IVJ::IScore>())
            continue;
        data.push_back(ente->getComponente<IVJ::IScore>()->score);
    }
    if (data.size() <= 0)
        return hist;
    auto mm = std::minmax_element(data.begin(), data.end());
    hist.minVal = *mm.first;
    hist.maxVal = *mm.second;
    if (hist.maxVal <= hist.minVal)
        hist.maxVal = hist.minVal + 1.f;

    hist.binWidth = (hist.maxVal - hist.minVal) / bins;
    hist.bins.assign(bins, 0.0f);

    for (size_t i = 0; i < total; i++)
    {
        int idx = data[i];
        // en teoria se debe de interpolar el valor y ver en que lote cae
        // pero en nuestro caso con 10 lotes es suficiente
        //(int)(data[i] - hist.minVal) / hist.binWidth;
        // idx = std::clamp(idx, 0, bins - 1);
        hist.bins[idx]++;
    }

    hist.peak = *std::max_element(hist.bins.begin(), hist.bins.end());
    return hist;
}

} // namespace CE
```

**Una vez que hagamos nuestra GUI** en Motor tenemos que instanciar la GUI,**No hay forma de distinguir que Escena es por lo que la GUI saldra en la otras escenas pero vacia**


**Motor/Motor.cpp**

```cpp
//Hay que incluir <Motor/GUI/GHistograma.hpp
void Motor::OnInit(void)
{
    Render::Get().CrearVentana(motor_info);
    // Render::Get().GetVentana().setKeyRepeatEnabled(false);

    if (!ImGui::SFML::Init(Render::Get().GetVentana()))
        exit(-1);

    // ImGui Configuración
    auto &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigDockingWithShift = true;

    gui_layers.push_back(std::make_shared<GDock>());
    gui_layers.push_back(std::make_shared<GViewport>());
#if DEBUG
    gui_layers.push_back(std::make_shared<GListaObjetos>());
    //INCLUIMOS ESta
    gui_layers.push_back(std::make_shared<GHistograma>());
    // gui_layers.push_back(std::make_shared<GPaths>());
#endif
    GestorCamaras::Get().agregarCamara(std::make_shared<Camara>(Vector2D{540, 360}, Vector2D{1080, 720}));
    GestorCamaras::Get().setCamaraActiva(0);

    // font por defecto
    GestorAssets::Get().agregarFont("default_font", ASSETS "/fonts/Electrolize.ttf");
    GestorAssets::Get().agregarFont("nova", ASSETS "/fonts/NovaSquare.ttf");

    mi_app->OnInit();

    for (auto &gui : gui_layers)
        gui->OnInit(motor_info);

#if DEBUG
    GLogger::Get().OnInit(motor_info);
#endif
}
```
![](https://external-content.duckduckgo.com/iu/?u=http%3A%2F%2Fdrive.google.com/uc?id=1i_TPURf6z6RO4xAj2cRAUDEGOu5Ahkfs)
