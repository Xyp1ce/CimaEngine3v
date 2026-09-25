#include "Entidad.hpp"
#include "../../Motor/Componentes/IComponentes.hpp"
#include "Motor/Utils/Vector2D.hpp"
#include <memory>
#include <string>

namespace IVJ {

static int num_cpy = 0;
Entidad::Entidad(const Entidad &cpy) : CE::Objeto{cpy} {
  nombre->nombre = cpy.getNombre()->nombre + "_" + std::to_string(++num_cpy);
  for (auto &comp : cpy.componentes) {
    componentes.push_back(comp->clonar());
  }
}

void Entidad::onUpdate(float dt) {
  // CE::Objeto::onUpdate(dt);

  // revisar si tiene ciertos componentes
  // para actualizarlos debidamente
  // o hacer una clase hija y ahí revisar

  if (tieneComponente<ITriangulo>()) {
    auto fig = getComponente<ITriangulo>();
    auto pos = getTransformada()->posicion;
    // dibujamos el triángulo en la posición correcta
    fig->tri_shape.setPosition({pos.x, pos.y});
    // aplicamos rotación dependiendo de la dirección
    auto r = sf::radians(fig->angulo);
    fig->tri_shape.setRotation(r);
  }
  if (tieneComponente<CE::ISprite>()) {
    auto sprite = getComponente<CE::ISprite>();
    auto pos = getTransformada()->posicion;
    sprite->m_sprite.setPosition({pos.x, pos.y});
    auto angulo = sf::radians(getTransformada()->angulo);
    sprite->m_sprite.setRotation(angulo);
  }

  // si tiene un FSM actualizar el estado
  if (tieneComponente<IVJ::IMaquinaEstado>()) {
    auto mq = getComponente<IVJ::IMaquinaEstado>();
    if (mq->fsm)
      mq->fsm->onUpdate(*this, dt);
  }
  transform->pos_prev = transform->posicion;
}
void Entidad::inputFSM() {
  if (!getComponente<IMaquinaEstado>() || !getComponente<IMaquinaEstado>()->fsm)
    return;
  auto mq = getComponente<IMaquinaEstado>();

  {
    if (!getComponente<CE::IControl>())
      return;
    auto control = *getComponente<CE::IControl>();
    auto n_estado = mq->fsm->onInputs(control);
    std::shared_ptr<FSM> estado(n_estado);
    if (estado)
      setFSM(estado);
  }
}

void Entidad::setFSM(const std::shared_ptr<FSM> &mq) {
  // cuando llamamos setFSM con make_shared como parámetro
  // hay que congelar para que no siempre lo cambie
  if (getComponente<IMaquinaEstado>()->congelar)
    return;
  auto &estado_actual = getComponente<IMaquinaEstado>()->fsm;
  // transición de salida
  estado_actual->onSalir(*this);
  // transición de entrar a otro estado
  estado_actual = mq;
  estado_actual->onEntrar(*this);
}

void Entidad::draw(sf::RenderTarget &target, sf::RenderStates state) const {
  state.transform *= getTransform();
#if DEBUG
  if (tieneComponente<CE::IBoundingBox>()) {
    auto escala = 1; // getComponente<CE::ISprite>()->escala;
    auto bound = getComponente<CE::IBoundingBox>();
    sf::RectangleShape box{{bound->tam.x * escala, bound->tam.y * escala}};
    box.setFillColor(sf::Color::Transparent);
    box.setOutlineColor(sf::Color::Red);
    box.setOutlineThickness(1.5f);
    box.setOrigin({bound->mitad.x * escala, bound->mitad.y * escala});
    auto pos = transform->posicion;
    box.setPosition({pos.x, pos.y});
    target.draw(box);
  }
#endif
  // Después del debug pintamos el triángulo
  if (tieneComponente<ITriangulo>()) {
    auto fig = getComponente<ITriangulo>();
    target.draw(fig->tri_shape);
  }
}
} // namespace IVJ
