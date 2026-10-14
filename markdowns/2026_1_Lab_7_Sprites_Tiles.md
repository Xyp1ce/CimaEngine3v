# Laboratorio 7 $\rightarrow$ Sprites y Tilemaps

## Donde guardar los sprites 

En nuestro folder raíz hay un folder llamado assets, dentro de ahí hay un folder llamado sprites, aquí podemos guardar todo lo referente a sprites.

Al compilar generamos una variable llamada ASSETS la cual nos da la dirección hacia ese folder, solamente es concatenar el path del asset que queramos. por ejemplo
```
ASSETS "/sprites/jugador/mi_sprite.png"
```

En este caso guardare la siguiente imagen en `assets/sprites/naves/player_b.png`

![](https://external-content.duckduckgo.com/iu/?u=http%3A%2F%2Fdrive.google.com/uc?id=1hErlBogPrHHUnjEaIdEzBy_zZ6WiA0b1)

## Componente ISprite y Gestor de Assets

Este componente se encuentra en `Motor/Componentes/Icomponentes` ua la clase de `sf::Sprite` para pintar en pantalla el sprite. Solo le tenemos que proporcionar una textura (imagen) y sus dimensiones


```cpp
ISprite::ISprite(const sf::Texture& textura,int w,int h,float escala)
```


La textura la tenemos que sacar del **gestor de assets** o cualquier otro archivo multimedio (sonidos, fonts) necesitamos utilizar el GestorAssets, se encuentra en: `Motor/Primitivos/GestorAssets.hpp`, es un singleton donde registra en memoria el archivo que queremos utilizar después o frecuentemente en nuestro juego.

Tiene métodos como:
* agregarFont("key",pathdelfont)
* getFont("key")
* agregarTextura("key",pathdelaimagen)
* getTextura("key")
* agregarMusica("key", pathdelmp3)
* getMusica("key")
* agregarSonido("key", pathdelogg)
* getSonido("key")

En este caso solo usaremos los referentes a textura.
```cpp

void GestorAssets::agregarTextura(const std::string& key, const std::string& filepath,
                    const CE::Vector2D& pos_init,const CE::Vector2D& dim);

sf::Texture& GestorAssets::getTextura(const std::string& key)
```


Entonces lo más ideal es que los sprites que vamos a cargar  siempre ( como el jugador) los carguemos en juego.cpp para siempre tenerlos disponibles, pero de igualmanera se pueden cargar en la escena que se van utilizar. En este caso haremos lo segundo.

**Creamos o modificamos la escena de laboratorio anterior** en este caso solo fue copi paste y se llamará Escena_SpriteTiles

**Escena_SpriteTiles.cpp** ONINIT
```cpp
//agregar <Motor/Primitivos/GestorAssets.hpp>

void Escena_Sprite::onInit()
{
    CE::GestorCamaras::Get().setCamaraActiva(2);
    CE::GestorCamaras::Get().getCamaraActiva().lockEnObjeto(player);
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

    //Cargar el sprite
    CE::GestorAssets::Get().agregarTextura(
            "naveb",                                //llave
            ASSETS "/sprites/naves/player_b.png",   //path del sprite
            CE::Vector2D{0.f,0.f},                  //pos dentro de la hoja
            CE::Vector2D{64.f,64.f});               // dimensiones


    auto trans = player->getTransformada();
    trans->velocidad = CE::Vector2D{500.f,500.f};
    player->setPosicion(300.f,300.f);
    
    auto sprite = std::make_shared<CE::ISprite>(
            CE::GestorAssets::Get().getTextura("naveb"), //textura
            64,64,                                      // dim
            1.f);                                       // escala
    
    player->addComponente(sprite);
    player->addComponente(std::make_shared<CE::IControl>());


    //objetos para que se muestre el movimiento
    int montes_count=100;
    float dstd= 20.f;
    for(int i=0;i<montes_count;i++)
    {
        //gauss
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

al compilar nos debería de dar lo siguiente:

![](https://external-content.duckduckgo.com/iu/?u=http%3A%2F%2Fdrive.google.com/uc?id=15ciVLPGUIsSClE1F4fr70Djjhh2X3z-m)


Si movemos al jugador, vemos que las rotaciones no las tiene, por lo que vamos a arreglar eso

**Juego/Sistemas/Sistemas.cpp**
En nuestra función donde calculabamos el movimiento y angulo, simplemente el valor guardarlo en la transformada->angulo

```cpp
void SistemaMover(const std::shared_ptr<CE::Objeto>& objeto, float dt)
{
    auto trans = objeto->getTransformada();
    
    auto control = objeto->getComponente<CE::IControl>();
    if(!control || !control->isActivo()) return;

    auto vel = CE::Vector2D{0.f,0.f};
    if(control->arr) vel.y = -trans->velocidad.y;
    if(control->abj) vel.y = trans->velocidad.y;
    if(control->der) vel.x = trans->velocidad.x;
    if(control->izq) vel.x = -trans->velocidad.x;

    if(objeto->getComponente<ITriangulo>() 
    || objeto->getComponente<CE::ISprite>())
    {
        auto n = vel;
        n.normalizacion();
        auto trian = objeto->getComponente<ITriangulo>(); //nulo
        if (vel.x!=0 || vel.y!=0)
        {
            if(trian)
                trian->angulo = std::atan2(n.x,-n.y);
            trans->angulo = std::atan2(n.x,-n.y);
        }
    }

    trans->posicion.suma(vel.escala(dt));
}
```

Después nos vamos a `Juego/objetos/Entidad.cpp` y en el update donde esta ISprite actualizamos el ángulo

**Juego/objetos/Entidad.cpp**
```cpp
void Entidad::onUpdate(float dt) 
{

    if(tieneComponente<ITriangulo>())
    {
        auto fig = getComponente<ITriangulo>();
        auto pos = getTransformada()->posicion;
        fig->tri_shape.setPosition({pos.x,pos.y});
        auto r = sf::radians(fig->angulo); 
        //std::cout<<r.asRadians()<<"-> "<<r.asDegrees()<<std::endl;
        fig->tri_shape.setRotation(r);
    }
    //actualizar el angulo y posición del sprite
    if(tieneComponente<CE::ISprite>())
    {
        auto sprite = getComponente<CE::ISprite>();
        auto pos = getTransformada()->posicion;
        sprite->m_sprite.setPosition({pos.x,pos.y});
        auto angulo = sf::radians(getTransformada()->angulo);
        sprite->m_sprite.setRotation(angulo);
    }
    //..más código
    //..
}
```
Si ejecutamos ahora los ángulos de las direcciones estan correctos.

![](https://external-content.duckduckgo.com/iu/?u=http%3A%2F%2Fdrive.google.com/uc?id=1PfJCXqtQRO43eDyVMLSKqJsx4ZNi0C2M)

## TileMaps

Dentro de nuestro folder de assets hay un folder atlas, **este folder contiene un script en python** para poner un overlay a el atlas y saber cual es el índice de cada tile, se usa de la siguiente manera

```
python tile_over.py mi_imagen_atlas.png dim1
```
asume que la dimensión es cuadrada osea si dim1=64, asume que es un tile de 64x64

para este caso usaremos el siguiente atlas:

![](https://external-content.duckduckgo.com/iu/?u=http%3A%2F%2Fdrive.google.com/uc?id=1U1MATkkb8HY9faiD4hYJ9ESCDjiddz8p)

Si usamos el script 
```
python tile_over.py playa_atlas.png 64
```
nos genera la siguiente imagen con sus indices correctos

![](https://external-content.duckduckgo.com/iu/?u=http%3A%2F%2Fdrive.google.com/uc?id=1n-1sPbfWRDLnyQO1qfOxi51--WnGuBRL)

Con estos indices podemos construir la matriz que representa nuestro mapa.

### Clase TileMap

esta clase se encarga de cargar el mapa a través de una matriz de IDs de los tiles, es la matriz lógica de los tiles, también se encarga de pintarlos en la escena.

**nuestra escena tiene una variable llamada tiles_layers** el cual es un **arreglo de Tilemaps** lo que nos permite tener diferentes capas para pintar el mapa.

la funciones que utilizaremos son:

```cpp
bool TileMap::loadTileMap(const std::string& atlas_path);
```
Con esta función cargamos la matriz lógica.


### Matriz Lógica

Consiste en un archivo de texto que lo guardamos en `assets/mapas/` y contiene la siguiente información

```
[Atlas_info]
/direccion/del/atlas.png
dimension_atlas_W dimension_atlas_H
dimension_tile_W dimension_tile_H
mapa_renglones mapa_columnas
[ID_Matrix]
IDS Separados Por Espacio
```

por ejemplo, si hacemos una capa y la llamamos `assets/mapa/playa_layer1.txt` y el contenido sería :

**playa_layer1.txt** puro mar
```
[Atlas_info]
/atlas/playa_atlas.png
2048 768
64 64
30 30
[ID_Matrix]
272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 
304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 
272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 
304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 
272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 
304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 
272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 
304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 
272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 
304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 
272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 
304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 
272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 
304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 
272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 
304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 
272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 
304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 
272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 
304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 
272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 
304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 
272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 
304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 
272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 
304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 
272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 
304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 
272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 
304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 
272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 
304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 
272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 
304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 
272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 
304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 
272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 
304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 
272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 
304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 
272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 
304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 
272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 
304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 
272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 
304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 
272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 
304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 
272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 
304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 
272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 
304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 
272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 
304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 
272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 
304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 
272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 272 273 
304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 304 305 
```

Para partes de la matriz que no queremos que tenga sprite, le asignamos un -1

**playa_layer2.txt** las puras islas
```
[Atlas_info]
/atlas/playa_atlas.png
2048 768
64 64
30 30
[ID_Matrix]
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 0 1 2 3 4 5 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 32 33 34 35 36 37 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 0 1 2 3 4 5 -1 -1 -1
-1 -1 -1 64 65 66 67 68 69 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 32 33 34 35 36 37 -1 -1 -1
-1 -1 -1 96 97 98 99 100 101 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 64 65 66 67 68 69 -1 -1 -1
-1 -1 -1 128 129 130 131 132 133 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 96 97 98 99 100 101 -1 -1 -1
-1 -1 -1 160 161 162 163 164 165 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 128 129 130 131 132 133 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 160 161 162 163 164 165 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 0 1 2 3 4 5 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 32 33 34 35 36 37 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 0 1 2 3 4 5 -1 -1 -1 -1
-1 -1 -1 -1 64 65 66 67 68 69 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 32 33 34 35 36 37 -1 -1 -1 -1
-1 -1 -1 -1 96 97 98 99 100 101 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 64 65 66 67 68 69 -1 -1 -1 -1
-1 -1 -1 -1 128 129 130 131 132 133 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 96 97 98 99 100 101 -1 -1 -1 -1
-1 -1 -1 -1 160 161 162 163 164 165 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 128 129 130 131 132 133 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 160 161 162 163 164 165 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
```
Finalmente la tercera capa será de los detallitos como objetos en el mundo

**playa_layer3.txt** los detallitos
```
[Atlas_info]
/atlas/playa_atlas.png
2048 768
64 64
30 30
[ID_Matrix]
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 268 269 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 300 301 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 194 195 196 197 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 226 227 228 229 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 334 335 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 366 367 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
```

Con estos archivos ya podemos cargar nuestro mapa, ahora nos vamos a nuestra escena y hacemos uso de las matrices

**Escena_SpriteTiles.cpp** 
```Cpp
//agregamos <Juego/objetos/TileMap.hpp>
void Escena_Sprite::onInit()
{
    CE::GestorCamaras::Get().setCamaraActiva(2);
    CE::GestorCamaras::Get().getCamaraActiva().lockEnObjeto(player);
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

    //cargar mapa 3 layers
    tiles_layers.push_back(TileMap()); //puro mar
    tiles_layers.push_back(TileMap()); // las islas
    tiles_layers.push_back(TileMap()); // los objetos

    if(!tiles_layers[0].loadTileMap(ASSETS "/mapas/playa_layer1.txt"))
        exit(EXIT_FAILURE);
    if(!tiles_layers[1].loadTileMap(ASSETS "/mapas/playa_layer2.txt"))
        exit(EXIT_FAILURE);
    if(!tiles_layers[2].loadTileMap(ASSETS "/mapas/playa_layer3.txt"))
        exit(EXIT_FAILURE);

    //Cargar el sprite
    CE::GestorAssets::Get().agregarTextura(
            "naveb",                                //llave
            ASSETS "/sprites/naves/player_b.png",   //path del sprite
            CE::Vector2D{0.f,0.f},                  //pos dentro de la hoja
            CE::Vector2D{64.f,64.f});               // dimensiones


    auto trans = player->getTransformada();
    trans->velocidad = CE::Vector2D{500.f,500.f};
    player->setPosicion(300.f,300.f);
    
    auto sprite = std::make_shared<CE::ISprite>(
            CE::GestorAssets::Get().getTextura("naveb"), //textura
            64,64,                                      // dim
            1.f);                                       // escala
    
    player->addComponente(sprite);
    player->addComponente(std::make_shared<CE::IControl>());

    inicializar=false;
}

//render
void Escena_Sprite::onRender()
{
    //renderizamos los layers primero para 
    //que se pinten atrás de todo
    for(auto& al: tiles_layers)
        CE::Render::Get().AddToDraw(al);

    for(auto& obj: objetos.getPool())
        CE::Render::Get().AddToDraw(*obj);
    CE::Render::Get().AddToDraw(*player);

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

Al ejecutar tendremos

![](https://external-content.duckduckgo.com/iu/?u=http%3A%2F%2Fdrive.google.com/uc?id=1LQrG0MiCOE90CRj7uziL8z9erhwWOJ4p)
