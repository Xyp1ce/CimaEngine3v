#pragma once
#include "../FSM.hpp"
namespace IVJ {
class FSMBoss : public FSM {
public:
  virtual ~FSMBoss() = default;
  // a boss no le interesa este tipo de inputs por eso vacio
  //(mal diseño pero para no romper entidad)
  //(para este tipo de problemas aplicar patron de:
  // puente, facade, estrategia, comando, adaptador )
  virtual FSM *onInputs(const CE::IControl &control) override {
    (void)control;
    return nullptr;
  };
  // el input que usaremos
  virtual FSM *onInputs(Entidad &parent, CE::Vector2D &target) = 0;
  virtual void onEntrar(const Entidad &obj) override = 0;
  virtual void onSalir(const Entidad &obj) override = 0;
  void onUpdate(const Entidad &obj, float dt) override = 0;
};
} // namespace IVJ
