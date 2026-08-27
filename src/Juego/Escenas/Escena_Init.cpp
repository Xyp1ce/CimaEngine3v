#include "Escena_Init.hpp"
#include <Juego/Componentes/IJComponentes.hpp>
#include <Juego/Figuras/ClasePrueba.hpp>
#include <Juego/Figuras/Figuras.hpp>
#include <Juego/Sistemas/Sistemas.hpp>
#include <Juego/objetos/Texto.hpp>
#include <Motor/Inputs/Botones.hpp>
#include <Motor/Primitivos/GestorAssets.hpp>
#include <Motor/Primitivos/GestorEscenas.hpp>
#include <Motor/Render/Render.hpp>
#include <memory>

namespace IVJ {
Escena_Init::Escena_Init() : CE::Escena{} {}
void Escena_Init::onInit() {
  if (!inicializar)
    return;

  std::cout << "Variable apuntador inteligente" << "\n";
  std::shared_ptr<Alumno> al = std::make_shared<Alumno>("Ramon", 2210376, 6);
  std::cout << al->getNombre() << "\n";

  inicializar = false;
}
void Escena_Init::onFinal() {}
void Escena_Init::onUpdate(float dt) {
  for (auto &obj : objetos.getPool()) {
    obj->onUpdate(dt);
  }
}
void Escena_Init::onInputs(const CE::Botones &accion) {
  switch (accion.getTipo()) {
  case CE::Botones::TipoAccion::OnPress: {
    break;
  }
  case CE::Botones::TipoAccion::OnRelease: {
    break;
  }
  case CE::Botones::TipoAccion::None: {
    break;
  }
  }
}
void Escena_Init::onRender() {
  for (auto &obj : objetos.getPool())
    CE::Render::Get().AddToDraw(*obj);
}
} // namespace IVJ
