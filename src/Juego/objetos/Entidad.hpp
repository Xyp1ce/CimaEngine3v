#pragma once
#include "../../Motor/Primitivos/Objetos.hpp"
#include "../Maquinas/FSM.hpp"
#include <memory>

namespace IVJ
{
class FSM; // referencia circular
class Entidad : public CE::Objeto
{
  public:
    Entidad() = default;
    Entidad(const Entidad &cpy);
    void onUpdate(float dt) override;
    void draw(sf::RenderTarget &target, sf::RenderStates state) const override;

    // fsm
    void setFSM(const std::shared_ptr<FSM> &mq);
    void inputFSM() override;

    // versiones const
    const std::shared_ptr<CE::INombre> getNombre() const
    {
        return nombre;
    }
    const std::shared_ptr<CE::IStats> getStats() const
    {
        return stats;
    };
    const std::vector<std::shared_ptr<CE::IComponentes>> &getListaComponentes() const
    {
        return componentes;
    }
};
} // namespace IVJ
