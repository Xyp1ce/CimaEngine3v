#pragma once
#include "../../Motor/Componentes/IComponentes.hpp"
#include "../../Motor/Primitivos/CEPool.hpp"
#include "Motor/Utils/Vector2D.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
/*TODO: Mover a Motor/Mapas para pasarlo a un dll el motor*/
namespace IVJ
{
class TileMap : public sf::Transformable, public sf::Drawable
{
  public:
    explicit TileMap() = default;

  public:
    bool loadTileMap(const std::string &atlas_path);
    bool loadTileMap(const std::string &atlas_path, CE::Pool &objetos);
    bool loadTileMap(const std::string &atlas_path, CE::Pool &objetos, const CE::Vector2D pos_init);
    virtual void draw(sf::RenderTarget &target, sf::RenderStates state) const;
    // TODO: shaders en tilemap
    bool loadTileMapShaders(const std::string &vert, const std::string &frag);
    CE::IShader *getTilemapShader()
    {
        return tilemap_shader.get();
    };

  private:
    int atlas_w{};
    int atlas_h{};
    int tile_w{};
    int tile_h{};
    int map_r{};
    int map_c{};
    std::string atlas_p{};

  public:
    sf::VertexArray tiles_vertex;
    sf::Texture atlas_texture;
    std::unique_ptr<CE::IShader> tilemap_shader;
};
} // namespace IVJ
