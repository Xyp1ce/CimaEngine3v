#pragma once
#include <Motor/Primitivos/Escena.hpp>
#include <Juego/objetos/Texto.hpp>
#include <memory>

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
  void actualizarSeleccion();

private:
  int inicializar{1};
  unsigned int opcion{0}; // 0 = Iniciar (Jugar), 1 = Cerrar
  float tiempo{0.f};

  std::shared_ptr<Texto> strTitulo;
  std::shared_ptr<Texto> strIniciar;
  std::shared_ptr<Texto> strCerrar;
  std::shared_ptr<Texto> strMarcador;
  std::shared_ptr<Texto> strInstrucciones;
};
} // namespace IVJ
