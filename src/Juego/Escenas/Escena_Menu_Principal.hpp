#pragma once
#include <Juego/Sistemas/ConfigExpendable.hpp>
#include <Motor/Primitivos/Escena.hpp>

namespace IVJ {
class Escena_Menu_Principal : public CE::Escena {
public:
  explicit Escena_Menu_Principal();
  virtual ~Escena_Menu_Principal() {};

  void onInit() override;
  void onFinal() override;
  void onUpdate(float dt) override;
  void onInputs(const CE::Botones &accion) override;
  void onRender() override;

private:
  int inicializar{1};
  int opcion{0}; // 0 = Iniciar, 1 = Cerrar
  float tiempo{0.f};
};
} // namespace IVJ
