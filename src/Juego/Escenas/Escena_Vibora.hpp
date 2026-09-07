#pragma once
#include <Juego/Figuras/Figuras.hpp>
#include <Motor/Primitivos/Escena.hpp>
#include <memory>

namespace IVJ {

class EnteVibora; // Declaración adelantada de la clase

class Escena_Vibora : public CE::Escena {
public:
  explicit Escena_Vibora();
  virtual ~Escena_Vibora() {};
  void onInit() override;
  void onFinal() override;
  void onUpdate(float dt) override;
  void onInputs(const CE::Botones &accion) override;
  void onRender() override;

private:
  int inicializar{1};
  
  // Guardamos una referencia directa a la víbora para facilitar el control 
  // en onInputs sin tener que buscarla en el pool en cada frame.
  std::shared_ptr<EnteVibora> vibora; 
};
} // namespace IVJ
