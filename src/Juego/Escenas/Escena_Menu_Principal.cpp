#include "Escena_Menu_Principal.hpp"
#include <Motor/Camaras/CamarasGestor.hpp>
#include <Motor/Inputs/Botones.hpp>
#include <Motor/Primitivos/GestorAssets.hpp>
#include <Motor/Primitivos/GestorEscenas.hpp>
#include <Motor/Render/Render.hpp>
#include <cmath>
#include <cstdlib>

namespace IVJ {

Escena_Menu_Principal::Escena_Menu_Principal() : CE::Escena{} {}

void Escena_Menu_Principal::onInit() {
  // Aseguramos que la cámara activa para el menú sea la cámara fija por defecto (0)
  CE::GestorCamaras::Get().setCamaraActiva(0);
  opcion = 0;
  tiempo = 0.f;

  if (!inicializar) {
    actualizarSeleccion();
    return;
  }

  // Mapeo de botones de control para navegación en el menú
  registrarBotones(sf::Keyboard::Scancode::W, "arriba");
  registrarBotones(sf::Keyboard::Scancode::Up, "arriba");
  registrarBotones(sf::Keyboard::Scancode::S, "abajo");
  registrarBotones(sf::Keyboard::Scancode::Down, "abajo");
  registrarBotones(sf::Keyboard::Scancode::Enter, "aceptar");
  registrarBotones(sf::Keyboard::Scancode::Space, "aceptar");
  registrarBotones(sf::Keyboard::Scancode::E, "aceptar");
  registrarBotones(sf::Keyboard::Scancode::Escape, "cerrar");

  const auto &font = CE::GestorAssets::Get().getFont("default_font");

  // Título del juego
  strTitulo = std::make_shared<Texto>(font, "Expendable");
  strTitulo->setFontSize(54);
  strTitulo->setColor(sf::Color(240, 240, 240, 250));
  strTitulo->setPosicion(385.f, 110.f);

  // Opción 1: Iniciar (Opción de Jugar) - Tamaño considerablemente mayor, color blanco
  strIniciar = std::make_shared<Texto>(font, "Iniciar");
  strIniciar->setFontSize(76);
  strIniciar->setColor(sf::Color::White);
  strIniciar->setPosicion(430.f, 250.f);

  // Opción 2: Cerrar - Tamaño menor que la de jugar, color blanco
  strCerrar = std::make_shared<Texto>(font, "Cerrar");
  strCerrar->setFontSize(48);
  strCerrar->setColor(sf::Color(180, 180, 180, 200));
  strCerrar->setPosicion(430.f, 370.f);

  // Marcador selector (flecha indicadora)
  strMarcador = std::make_shared<Texto>(font, ">");
  strMarcador->setFontSize(60);
  strMarcador->setColor(sf::Color::White);
  strMarcador->setPosicion(365.f, 260.f);

  // Instrucciones de uso en la parte inferior
  strInstrucciones = std::make_shared<Texto>(
      font, "[ W / S / Flechas ] Seleccionar    [ Enter / Espacio ] Confirmar");
  strInstrucciones->setFontSize(22);
  strInstrucciones->setColor(sf::Color(160, 160, 160, 180));
  strInstrucciones->setPosicion(250.f, 620.f);

  objetos.agregarPool(strTitulo);
  objetos.agregarPool(strMarcador);
  objetos.agregarPool(strIniciar);
  objetos.agregarPool(strCerrar);
  objetos.agregarPool(strInstrucciones);

  actualizarSeleccion();
  inicializar = false;
}

void Escena_Menu_Principal::onFinal() {}

void Escena_Menu_Principal::actualizarSeleccion() {
  if (opcion == 0) {
    // Opción Iniciar (Jugar) activa
    if (strIniciar)
      strIniciar->setColor(sf::Color::White);
    if (strCerrar)
      strCerrar->setColor(sf::Color(170, 170, 170, 180));
  } else {
    // Opción Cerrar activa
    if (strIniciar)
      strIniciar->setColor(sf::Color(170, 170, 170, 180));
    if (strCerrar)
      strCerrar->setColor(sf::Color::White);
  }
}

void Escena_Menu_Principal::onUpdate(float dt) {
  tiempo += dt;
  float marker_offset_x = std::sin(tiempo * 6.f) * 6.f;

  if (strMarcador) {
    if (opcion == 0) {
      strMarcador->setPosicion(365.f + marker_offset_x, 260.f);
    } else {
      strMarcador->setPosicion(365.f + marker_offset_x, 365.f);
    }
  }

  for (auto &obj : objetos.getPool()) {
    obj->onUpdate(dt);
  }
}

void Escena_Menu_Principal::onInputs(const CE::Botones &accion) {
  if (accion.getTipo() == CE::Botones::TipoAccion::OnPress) {
    if (accion.getNombre() == "arriba") {
      opcion = (opcion == 0) ? 1 : 0;
      actualizarSeleccion();
    } else if (accion.getNombre() == "abajo") {
      opcion = (opcion == 1) ? 0 : 1;
      actualizarSeleccion();
    } else if (accion.getNombre() == "aceptar") {
      if (opcion == 0) {
        // Opción de Jugar seleccionada -> Transiciona a Escena_Propia
        CE::GestorEscenas::Get().cambiarEscena("Escena_Propia");
        // Fallback por si la escena está registrada como "Propia"
        if (CE::GestorEscenas::Get().getEscenaActual().nombre != "Escena_Propia" &&
            CE::GestorEscenas::Get().getEscenaActual().nombre != "Propia") {
          CE::GestorEscenas::Get().cambiarEscena("Propia");
        }
      } else if (opcion == 1) {
        // Opción Cerrar seleccionada -> Termina la aplicación
        std::exit(EXIT_SUCCESS);
      }
    } else if (accion.getNombre() == "cerrar") {
      std::exit(EXIT_SUCCESS);
    }
  }
}

void Escena_Menu_Principal::onRender() {
  for (auto &obj : objetos.getPool()) {
    CE::Render::Get().AddToDraw(*obj);
  }
}

} // namespace IVJ
