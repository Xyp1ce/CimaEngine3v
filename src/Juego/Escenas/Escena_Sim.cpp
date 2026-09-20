#include "Escena_Sim.hpp"
#include <Juego/Componentes/IJComponentes.hpp>
#include <Juego/Figuras/Figuras.hpp>
#include <Juego/Sistemas/Sistemas.hpp>
#include <Juego/objetos/Texto.hpp>
#include <Motor/GUI/GLogger.hpp>
#include <Motor/Primitivos/GestorAssets.hpp>
#include <Motor/Primitivos/GestorEscenas.hpp>
#include <Motor/Render/Render.hpp>
#include <memory>
#include <random>
namespace IVJ {
Escena_Sim::Escena_Sim()
    : CE::Escena{}, timer_generacion{60 * 30},
      mensaje{CE::GestorAssets::Get().getFont("default_font"),
              "Presiona Enter para continuar"} {
  punto_inferior.x = 50.f;
  punto_inferior.y = 50.f;
  punto_superior.x = 1000.f;
  punto_superior.y = 650.f;
  entes_iniciales = 100;
  comida_iniciales = 150;
  generacion = 0;
  generacion_termino = false;

  float ancho = punto_superior.x - punto_inferior.x;
  float largo = punto_superior.y - punto_inferior.y;

  mundo = std::make_shared<Rectangulo>(ancho, largo, sf::Color::White,
                                       sf::Color::Black);

  float mx = punto_inferior.x + ancho / 2.f;
  float my = punto_inferior.y + largo / 2.f;

  mundo->setPosicion(mx, my);

  mensaje.m_texto.setPosition({mx, my});
  mensaje.m_texto.setFillColor(sf::Color::Yellow);
}
void Escena_Sim::onInit() {
  std::random_device rd;
  std::mt19937 rand(rd());
  float x;
  float y;
  float angulo;
  // comida
  // deben estar dentro del mundo
  for (int i = 0; i < comida_iniciales; i++) {
    x = std::uniform_real_distribution(100.f + punto_inferior.x,
                                       punto_superior.x - 100.f)(rand);
    y = std::uniform_real_distribution(100.f + punto_inferior.y,
                                       punto_superior.y - 100.f)(rand);
    auto comida =
        std::make_shared<Circulo>(5.f, sf::Color::Green, sf::Color::Green);
    comida->setPosicion(x, y);
    objetos.agregarPoolEnfrete(comida);
  }
  objetos.agregarPoolEnfrete(mundo);

  if (!inicializar)
    return;

  registrarBotones(sf::Keyboard::Scancode::Enter, "Ok");

  // entes iniciales
  // donde respawn la idea es sobre la linea del mundo
  for (int i = 0; i < entes_iniciales; i++) {
    const float num = std::uniform_real_distribution(0.0f, 1.0f)(rand);
    if (num <= 0.5) // abajo
    {
      y = punto_superior.y;
      angulo = 0.f;
    } else // arriba
    {
      y = punto_inferior.y;
      angulo = 180.f;
    }
    x = std::uniform_real_distribution(punto_inferior.x,
                                       punto_superior.x)(rand);

    auto ente =
        std::make_shared<Pentagono>(15.f, sf::Color::Red, sf::Color::Black);
    auto &stats = ente->getStats();
    auto &vel = ente->getTransformada()->velocidad;
    vel.x = std::uniform_real_distribution(1.f, 10.f)(rand);
    vel.y = std::uniform_real_distribution(1.f, 10.f)(rand);
    stats->hp_max = 100;
    stats->hp = 100;
    stats->agi = std::uniform_int_distribution(10, 255)(rand);
    ente->setPosicion(x, y);
    ente->getShape().setRotation(sf::degrees(angulo));

    objetos.agregarPool(ente);
  }

  inicializar = false;
}

void Escena_Sim::onFinal() {}

void Escena_Sim::onUpdate(float dt) {
  if (generacion_termino) {
    // borrar los entes que ya murieron
    // boorarPool itera todos los objetos y va eliminando
    // a los objetos que stats->hp <= 0, en este caso
    // hp puede ser la energia.
    objetos.borrarPool();
  } else {
    for (auto &obj : objetos.getPool()) {
      obj->onUpdate(dt);
      // prueba al azar de entes muriendo
      std::random_device rd;
      std::mt19937 rand(rd());
      float prob = std::uniform_real_distribution(0.f, 1.f)(rand);
      if (prob < 0.0002) // matamos ente
      {
        int id = std::uniform_int_distribution(
            0, (int)objetos.getPool().size() - 1)(rand);
        auto &ente = objetos.getPool()[id];
        auto &stats = ente->getStats();
        stats->hp = 0;
        // se puede repetir el id pero solo al final de la simulación lo borra
        std::cout << prob << " murio ente id ->" << id << "\n";
      }
    }

    timer_generacion.frame_actual++;
    if (timer_generacion.frame_actual >= timer_generacion.frame_maximo) {
      generacion_termino = true;
      timer_generacion.frame_actual = 0;
    }
  }

  CE::GLogger::Get().agregarLog(
      std::to_string(timer_generacion.frame_actual) + "/" +
          std::to_string(timer_generacion.frame_maximo),
      CE::GLogger::Niveles::LOG_SEVERO);
}
void Escena_Sim::onInputs(const CE::Botones &accion) {
  switch (accion.getTipo()) {
  case CE::Botones::TipoAccion::OnPress: {

    break;
  }
  case CE::Botones::TipoAccion::OnRelease: {

    if (accion.getNombre() == "Ok" && generacion_termino == true) {
      generacion_termino = false;
      this->onInit();
    }
    break;
  }
  case CE::Botones::TipoAccion::None: {
    break;
  }
  }
}
void Escena_Sim::onRender() {
  if (generacion_termino) {
    // render texto
    CE::Render::Get().AddToDraw(mensaje.m_texto);
  } else {
    for (auto &obj : objetos.getPool())
      CE::Render::Get().AddToDraw(*obj);
  }
}
} // namespace IVJ
