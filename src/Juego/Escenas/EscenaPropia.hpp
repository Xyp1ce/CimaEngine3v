#pragma once
#include <Juego/objetos/TileMap.hpp>
#include <Motor/Primitivos/Escena.hpp>
#include <vector>
namespace IVJ {
class Escena_Propia : public CE::Escena {
public:
  explicit Escena_Propia(std::shared_ptr<Entidad> &pref);
  virtual ~Escena_Propia() {};
  void onInit() override;
  void onFinal() override;
  void onUpdate(float dt) override;
  void onInputs(const CE::Botones &accion) override;
  void onRender() override;
  std::shared_ptr<Entidad> getJugador() override { return jugador_soldier; }

private:
  int inicializar{1};
  std::shared_ptr<Entidad> &jugador_soldier;
  std::vector<TileMap> tiles_industrial;
};
} // namespace IVJ
