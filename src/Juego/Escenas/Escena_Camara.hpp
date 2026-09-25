#pragma once
#include <Motor/Primitivos/Escena.hpp>
namespace IVJ {
class Escena_Camara : public CE::Escena {
public:
  explicit Escena_Camara(std::shared_ptr<Entidad> &pref);
  virtual ~Escena_Camara() {};
  void onInit() override;
  void onFinal() override;
  void onUpdate(float dt) override;
  void onInputs(const CE::Botones &accion) override;
  void onRender() override;
  std::shared_ptr<Entidad> getJugador() override { return jugador_ref; }

private:
  int inicializar{1};
  std::shared_ptr<Entidad> &jugador_ref;
};
} // namespace IVJ
