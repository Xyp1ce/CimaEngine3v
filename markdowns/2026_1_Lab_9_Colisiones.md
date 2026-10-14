# Laboratorio 9 $\rightarrow$ Colisiones

# Formas de agregar colisiones 

Existen dos formar para agregar colisiones en el motor

* La Primera es por medio de un componente individual en la entidad
* La Segunda es a través del TileMap, si le pasamos el pool a la función de loadTileMap, asume que la matriz logica tenga la colisión 

**Para ambos casos ocupa un componente CE::IBoundingBox**, ya que con eso hace los calculos para saber si dos objetos estan colisionando.

## Primera forma

Esta forma es cuando ocupas ciertas entidades que tengan un área de colisión.

**Limitantes**: Solo se puede tener un solo componente de cada tipo, pero en la realidad, se recomienda que se tengan diferentes áreas de colisiones, como por ejemplo.

* una para las colisiones de la fisica como plataformas, piso etc
* una para las colisiones de daño, cuando un enemigo te golpea o tu a el
* una para las colisiones de interacción, como cuando te cuelgas de una pared

Sin embargo el CimaEngine solo acepta 1 componente por cada tipo, si se ponen dos del mismo, regresa el primero. **Se puede arreglar creando un método que busque por nombre del componente o manualmente iterando la lista de componentes**


teniendo cuenta eso, vamos agregar el componente de IBoundingBox, a nuestro jugador 

**Escena del lab anterior.cpp**
```cpp
void Escena_Sprite::onInit()
{
//código anterior
        player->addComponente(std::make_shared<CE::IBoundingBox>(
            CE::Vector2D{64.f,64.f} //dimensiones del sprite
        ));

//más código después
}
```
![](https://external-content.duckduckgo.com/iu/?u=http%3A%2F%2Fdrive.google.com/uc?id=1YL7RR1xNGk5pno1fVuI2QpNcgjzo7Mr0)

La posición el IBoundingBox se **ajusta dependiendo del punto de ancha del sprite**

## Segunda forma

Esta es cuando a la mejor al cargar el mapa queremos que ciertos tiles tengan colisión como una pared.

Este es a través del TileMap, pero sin embargo se necesita que en la matriz lógica contanga una **c si hay colisión** en el tile o cualquier otra letra de que no hay, en este caso usaré la **n**

### un layer con colisiones incluidas

Como por ejemplo el layer 0 del mapa prueba, contiene practicamente todos los tiles, por lo que en las esquinas vamos a poner que tengan áreas de colisión.

Antes que nada hay que modificar la matriz lógica agregando una c o una n entre cada tile

**ASSETS /atlas/mapas/playa_col_layer0.txt**

![](https://external-content.duckduckgo.com/iu/?u=http%3A%2F%2Fdrive.google.com/uc?id=1F7xYk27PUcHPR32u0QuGzyUVQaUVYlBg)


En la escena cargamos este nuevo layer y le pasamos el pool para que se le agregue el BoundingBox y se guarde en el pool

**Escena laboratorio anterior.cpp**

```cpp
void Escena_Sprite::onInit()
{
//código anterior
    // usar la función que tiene un pool
    if(!tiles_layers[0].loadTileMap(ASSETS "/mapas/playa_col_layer1.txt",objetos))
        exit(EXIT_FAILURE);
    if(!tiles_layers[1].loadTileMap(ASSETS "/mapas/playa_layer2.txt"))
        exit(EXIT_FAILURE);
    if(!tiles_layers[2].loadTileMap(ASSETS "/mapas/playa_layer3.txt"))
        exit(EXIT_FAILURE);

//más código después
}

```
![](https://external-content.duckduckgo.com/iu/?u=http%3A%2F%2Fdrive.google.com/uc?id=1UoD8D28stpEEFm9nMWpv0K_EdK_Rs3-t)


### Atlas para objetos individuales

Podemos usar el atlas para cargar objetos individuales sin necesidad de cargarlo por TileMap.

Para esto se hace igual que cuando hicimos las animaciones, pero directo en el sprite.

**También modificamos el pivote (ancla) para que se centrará el bounding box bien al sprite ya que puede salir un poco más de un lado que de otro**

**Escena Laboratorio Anterior.cpp**

```cpp
void Escena_Sprite::onInit()
{
//código anterior

    auto techo = std::make_shared<Entidad>();
    techo->getStats()->hp_max=100;
    techo->getStats()->hp=100;
    techo->getStats()->def=150;

    //cargar todo el atlas para poder poner cosas individuales
    CE::GestorAssets::Get().agregarTextura("atlas_playa",
            ASSETS "/atlas/playa_atlas.png",
            CE::Vector2D{0.f,0.f},
            CE::Vector2D{2048,768});

    auto techo_sprite = std::make_shared<CE::ISprite>(
        CE::GestorAssets::Get().getTextura("atlas_playa"),
        128,256, //dim
        1.f);
    //acomodar el sprite en el area deseada del atlas
    techo_sprite->m_sprite.setTextureRect(
            sf::IntRect{
            {64*28,64*6}, //220 indice
            {128,256} //dos tiles, 4 tiles en dimensiones
            });
    //acomodar el pivote para centrar bien el bounding box en el centro
    //128/2 = 64, 256/2 = 128
    techo_sprite->m_sprite.setOrigin({64,128});
    techo->addComponente(techo_sprite)
        .addComponente(std::make_shared<CE::IBoundingBox>(CE::Vector2D{128,256}));

    techo->setPosicion(1300,175);

    objetos.agregarPool(techo);
//más código después
}

```

![](https://external-content.duckduckgo.com/iu/?u=http%3A%2F%2Fdrive.google.com/uc?id=1rBLXSyknUVMzRC0TGmfoA5TwrAe6DJ-K)


# Colisiones

Para las colisiones usamos el SistemaAABBMid el cual ejecuta la verificación de AABB Mid, **Tiene un parámetro de resolución** y practicamente lo que hace es poner a la entidad en la posición anterior a la actual antes de la colisión.o

En este caso, vamos a revisar todos los objetos con respecto al player y si hay colisión que los objetos funcionen como paredes, Es una operación lenta por lo que lo más recomendable es que el pool solo se use para update, y para las colisiones hacer una lista de entidades que queremos comparar más pequeña que el pool.

**Escena laboratorio anterior.cpp**

```cpp
void Escena_Sprite::onUpdate(float dt)
{
    player->inputFSM();
    player->onUpdate(dt);
    SistemaMover(player,dt);
    for(auto& obj: objetos.getPool())
    {
        obj->inputFSM();
        obj->onUpdate(dt);
        if(obj->tieneComponente<ITarget>())
        {
            auto& tpos = *obj->getComponente<ITarget>()->pos;
            SistemaNPCLookTarget(*obj,tpos);
        }
        //lab9
        SistemaColAABBMid(*player, *obj, true);
    }
}
```


![](https://external-content.duckduckgo.com/iu/?u=http%3A%2F%2Fdrive.google.com/uc?id=1uW5q_2yWN4tyahno7aSBuLQcym7bTCjS)
