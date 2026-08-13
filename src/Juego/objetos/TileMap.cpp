#include "../objetos/Entidad.hpp"
#include "TileMap.hpp"
#include <fstream>
#include <iostream>
namespace IVJ
{
bool TileMap::loadTileMap(const std::string &atlas_path, CE::Pool &objetos)
{
    std::ifstream archivo{atlas_path};
    std::string info;
    archivo >> info; // tag [info]
    archivo >> atlas_p >> atlas_w >> atlas_h >> tile_w >> tile_h >> map_r >> map_c >> info;
    tiles_vertex.setPrimitiveType(sf::PrimitiveType::Triangles);
    tiles_vertex.resize(map_r * map_c * 6);
    if (!atlas_texture.loadFromFile(ASSETS + atlas_p))
    {
        std::cerr << "[TileMap] No se pudo cargar la imagen " << atlas_p << "\n";
        return false;
    }

    int id = 0;
    int i = 0;
    int j = 0;
    int max_col = atlas_w / tile_w;
    // colisión
    char col;
    while (archivo >> id)
    {
        archivo >> col;
        if (id != -1) // no ignorar tile
        {
            // referencia al triangulo a dibujar
            sf::Vertex *trian = &tiles_vertex[(i + j * map_c) * 6];
            // posición del tile en el atlas
            int tU = id % max_col;
            int tV = id / max_col;

            // hay boundingbox
            if (col == 'c')
            {
                float hw = tile_w / 2.f;
                float hh = tile_h / 2.f;
                auto bloque = std::make_shared<Entidad>();
                bloque->getStats()->hp = 100;
                bloque->setPosicion(i * tile_w + hw, j * tile_h + hh);
                bloque->addComponente(std::make_shared<CE::IBoundingBox>(CE::Vector2D{(float)tile_w, (float)tile_h}));
                objetos.agregarPool(bloque);
            }

            // vertices del triángulo
            trian[0].position = sf::Vector2f(i * tile_w, j * tile_h);
            trian[1].position = sf::Vector2f((i + 1) * tile_w, j * tile_h);
            trian[2].position = sf::Vector2f(i * tile_w, (j + 1) * tile_h);
            trian[3].position = sf::Vector2f(i * tile_w, (j + 1) * tile_h);
            trian[4].position = sf::Vector2f((i + 1) * tile_w, j * tile_h);
            trian[5].position = sf::Vector2f((i + 1) * tile_w, (j + 1) * tile_h);
            // textura del triángulo
            trian[0].texCoords = sf::Vector2f(tU * tile_w, tV * tile_h);
            trian[1].texCoords = sf::Vector2f((tU + 1) * tile_w, tV * tile_h);
            trian[2].texCoords = sf::Vector2f(tU * tile_w, (tV + 1) * tile_h);
            trian[3].texCoords = sf::Vector2f(tU * tile_w, (tV + 1) * tile_h);
            trian[4].texCoords = sf::Vector2f((tU + 1) * tile_w, tV * tile_h);
            trian[5].texCoords = sf::Vector2f((tU + 1) * tile_w, (tV + 1) * tile_h);
        }
        if ((i + 1) % map_c == 0)
            ++j;
        ++i %= map_c;
    }
    return true;
}
// Lab shaders
bool TileMap::loadTileMap(const std::string &atlas_path, CE::Pool &objetos, const CE::Vector2D pos_init)
{
    std::ifstream archivo{atlas_path};
    std::string info;
    archivo >> info; // tag [info]
    archivo >> atlas_p >> atlas_w >> atlas_h >> tile_w >> tile_h >> map_r >> map_c >> info;
    tiles_vertex.setPrimitiveType(sf::PrimitiveType::Triangles);
    tiles_vertex.resize(map_r * map_c * 6);
    if (!atlas_texture.loadFromFile(ASSETS + atlas_p))
    {
        std::cerr << "[TileMap] No se pudo cargar la imagen " << atlas_p << "\n";
        return false;
    }

    int id = 0;
    int i = 0;
    int j = 0;
    int max_col = atlas_w / tile_w;
    // colisión
    char col;
    while (archivo >> id)
    {
        archivo >> col;
        if (id != -1) // no ignorar tile
        {
            // referencia al triangulo a dibujar
            sf::Vertex *trian = &tiles_vertex[(i + j * map_c) * 6];
            // posición del tile en el atlas
            int tU = id % max_col;
            int tV = id / max_col;

            // hay boundingbox
            if (col == 'c')
            {
                float hw = tile_w / 2.f;
                float hh = tile_h / 2.f;
                auto bloque = std::make_shared<Entidad>();
                bloque->getStats()->hp = 100;
                bloque->setPosicion((pos_init.x + i * tile_w + hw), (pos_init.y + j * tile_h + hh));
                bloque->addComponente(std::make_shared<CE::IBoundingBox>(CE::Vector2D{(float)tile_w, (float)tile_h}));
                objetos.agregarPool(bloque);
            }

            // vertices del triángulo
            trian[0].position = sf::Vector2f(i * tile_w, j * tile_h);
            trian[1].position = sf::Vector2f((i + 1) * tile_w, j * tile_h);
            trian[2].position = sf::Vector2f(i * tile_w, (j + 1) * tile_h);
            trian[3].position = sf::Vector2f(i * tile_w, (j + 1) * tile_h);
            trian[4].position = sf::Vector2f((i + 1) * tile_w, j * tile_h);
            trian[5].position = sf::Vector2f((i + 1) * tile_w, (j + 1) * tile_h);
            // textura del triángulo
            trian[0].texCoords = sf::Vector2f(tU * tile_w, tV * tile_h);
            trian[1].texCoords = sf::Vector2f((tU + 1) * tile_w, tV * tile_h);
            trian[2].texCoords = sf::Vector2f(tU * tile_w, (tV + 1) * tile_h);
            trian[3].texCoords = sf::Vector2f(tU * tile_w, (tV + 1) * tile_h);
            trian[4].texCoords = sf::Vector2f((tU + 1) * tile_w, tV * tile_h);
            trian[5].texCoords = sf::Vector2f((tU + 1) * tile_w, (tV + 1) * tile_h);
        }
        if ((i + 1) % map_c == 0)
            ++j;
        ++i %= map_c;
    }
    return true;
}
bool TileMap::loadTileMap(const std::string &atlas_path)
{
    std::ifstream archivo{atlas_path};
    std::string info;
    archivo >> info; // tag [info]
    archivo >> atlas_p >> atlas_w >> atlas_h >> tile_w >> tile_h >> map_r >> map_c >> info;
    tiles_vertex.setPrimitiveType(sf::PrimitiveType::Triangles);
    tiles_vertex.resize(map_r * map_c * 6);
    if (!atlas_texture.loadFromFile(ASSETS + atlas_p))
    {
        std::cerr << "[TileMap] No se pudo cargar la imagen " << atlas_p << "\n";
        return false;
    }
    int id = 0;
    int i = 0;
    int j = 0;
    int max_col = atlas_w / tile_w;
    while (archivo >> id)
    {
        if (id != -1) // no ignorar tile
        {
            // referencia al triangulo a dibujar
            sf::Vertex *trian = &tiles_vertex[(i + j * map_c) * 6];
            // posición del tile en el atlas
            int tU = id % max_col;
            int tV = id / max_col;

            // vertices del triángulo
            trian[0].position = sf::Vector2f(i * tile_w, j * tile_h);
            trian[1].position = sf::Vector2f((i + 1) * tile_w, j * tile_h);
            trian[2].position = sf::Vector2f(i * tile_w, (j + 1) * tile_h);
            trian[3].position = sf::Vector2f(i * tile_w, (j + 1) * tile_h);
            trian[4].position = sf::Vector2f((i + 1) * tile_w, j * tile_h);
            trian[5].position = sf::Vector2f((i + 1) * tile_w, (j + 1) * tile_h);
            // textura del triángulo

            trian[0].texCoords = sf::Vector2f(tU * tile_w, tV * tile_h);
            trian[1].texCoords = sf::Vector2f((tU + 1) * tile_w, tV * tile_h);
            trian[2].texCoords = sf::Vector2f(tU * tile_w, (tV + 1) * tile_h);
            trian[3].texCoords = sf::Vector2f(tU * tile_w, (tV + 1) * tile_h);
            trian[4].texCoords = sf::Vector2f((tU + 1) * tile_w, tV * tile_h);
            trian[5].texCoords = sf::Vector2f((tU + 1) * tile_w, (tV + 1) * tile_h);
        }
        if ((i + 1) % map_c == 0)
            ++j;
        ++i %= map_c;
    }
    return true;
}

void TileMap::draw(sf::RenderTarget &target, sf::RenderStates state) const
{
    state.transform *= getTransform();
    state.texture = &atlas_texture;
    // state.shader = &tilemap_shader->m_shader;
    if (tilemap_shader)
        target.draw(tiles_vertex, &tilemap_shader->m_shader);
    else
        target.draw(tiles_vertex, state);
}

bool TileMap::loadTileMapShaders(const std::string &vert, const std::string &frag)
{
    tilemap_shader = std::make_unique<CE::IShader>(vert, frag);
    if (!tilemap_shader)
        return false;
    return true;
}
} // namespace IVJ
