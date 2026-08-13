#pragma once
#include "../../Motor/Componentes/IComponentes.hpp"
#include "../../Motor/Primitivos/Objetos.hpp"
#include "../Maquinas/FSM.hpp"
#include "Motor/Primitivos/CEPool.hpp"
#include "Motor/Utils/Vector2D.hpp"
#include <Juego/Figuras/Figuras.hpp>
namespace IVJ
{
class FSM; // refefencia circular
// Maquina estado componente
class IMaquinaEstado : public CE::IComponentes
{
  public:
    explicit IMaquinaEstado();
    ~IMaquinaEstado() override {};

    std::shared_ptr<IComponentes> clonar() const override
    {
        return std::make_shared<IMaquinaEstado>(*this);
    };

  public:
    std::shared_ptr<FSM> fsm;
    bool congelar{false};
};

// practica de camaras o también para debug
class ITriangulo : public CE::IComponentes
{
  public:
    explicit ITriangulo(float r, float ang);
    ~ITriangulo() override {};
    std::shared_ptr<IComponentes> clonar() const override
    {
        return std::make_shared<ITriangulo>(*this);
    };

  public:
    sf::CircleShape tri_shape;
    float angulo;
};
// componente QUEST con un arreglo de int de 8 bit

class IRayo : public CE::IComponentes
{
  public:
    explicit IRayo(CE::Vector2D &pos, float &sig, const float magnitud = 1.f);
    ~IRayo() override {};
    std::shared_ptr<IComponentes> clonar() const override
    {
        return std::make_shared<IRayo>(*this);
    };

  private:
    float magnitud;
    float &dir;
    CE::Vector2D &lp1;
    CE::Vector2D lp2;

  public:
    CE::Vector2D &getP1() const;
    CE::Vector2D &getP2();
    float getMagnitud() const
    {
        return magnitud;
    }
};

class IInteractuable : public CE::IComponentes
{
  public:
    IInteractuable() = default;
    virtual ~IInteractuable() override {};
    virtual void onInteractuar(CE::Objeto &obj) = 0;

    std::shared_ptr<IComponentes> clonar() const override = 0;

  public:
    bool interactuado{false};
};

class IIndicador : public IInteractuable
{
  public:
    explicit IIndicador(const sf::Texture &ref, float escala, Entidad *p, const sf::Color &c);
    ~IIndicador() override {};
    void onInteractuar(CE::Objeto &obj) override;
    void onRender();
    std::shared_ptr<IComponentes> clonar() const override
    {
        return nullptr;
    };

  public:
    sf::Sprite sprite;
    sf::Shader shader;
    sf::Color color;
    float escala;
    int frame_activo{0};
    bool activo{false};

  private:
    Entidad *parent;
};

class IDialogo : public IInteractuable
{
  public:
    IDialogo();
    ~IDialogo() {};
    void onInteractuar(CE::Objeto &obj) override;
    void onRender();
    std::shared_ptr<IComponentes> clonar() const override
    {
        return nullptr;
    };

  private:
    std::wstring agregarSaltoLinea(const std::wstring &str, size_t max_len);

  public:
    std::wstring texto;
    int id_texto;
    bool activo{false};
};

} // namespace IVJ
