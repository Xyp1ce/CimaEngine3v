#pragma once
#include <Juego/Figuras/Figuras.hpp>
#include <Motor/Primitivos/Escena.hpp>

namespace IVJ {
class Escena_Menu : public CE::Escena {
public:
  explicit Escena_Menu();
  virtual ~Escena_Menu() {};
  void onInit() override;
  void onFinal() override;
  void onUpdate(float dt) override;
  void onInputs(const CE::Botones &accion) override;
  void onRender() override;

private:
  int inicializar{1};
  float salto;
  unsigned int opcion;
};
} // namespace IVJ
