#include "Figuras.hpp"
#include <Juego/Componentes/IJComponentes.hpp>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <typeinfo>

namespace IVJ {
Figuras::Figuras(int l, const sf::Color &relleno, const sf::Color &contorno)
    : CE::Objeto{}, f_lados{l}, f_crelleno{relleno}, f_ccontorno{contorno} {}

Rectangulo::Rectangulo(float ancho, float largo, const sf::Color &relleno,
                       const sf::Color &contorno)
    : Figuras{4, relleno, contorno},
      rect_img{sf::RectangleShape({ancho, largo})}, w{ancho}, h{largo} {
  rect_img.setFillColor(f_crelleno);
  rect_img.setOutlineColor(f_ccontorno);
  rect_img.setOutlineThickness(3);
  rect_img.setOrigin({ancho / 2.f, largo / 2.f});
}

void Rectangulo::draw(sf::RenderTarget &target, sf::RenderStates state) const {
  state.transform *= getTransform();
  target.draw(rect_img);
  sf::CircleShape pivote{10.f};
  pivote.setFillColor(f_ccontorno);
  pivote.setOrigin({10.f, 10.f});
  pivote.setPosition(rect_img.getPosition());

  target.draw(pivote);
}
void Rectangulo::onUpdate(float dt) {
  rect_img.setPosition({transform->posicion.x, transform->posicion.y});
  (void)dt;
}

// CIRCULO
Circulo::Circulo(float radio, const sf::Color &relleno,
                 const sf::Color &contorno)
    : Figuras{32, relleno, contorno}, circ_img{radio, 32}, radio{radio} {
  circ_img.setFillColor(relleno);
  circ_img.setOutlineColor(contorno);
  circ_img.setOutlineThickness(3);
  // anchor
  circ_img.setOrigin({radio, radio});
}
void Circulo::onUpdate(float dt) {
  circ_img.setPosition({transform->posicion.x, transform->posicion.y});
  (void)dt;
}
void Circulo::draw(sf::RenderTarget &target, sf::RenderStates state) const {
  state.transform *= getTransform();
  target.draw(circ_img);
  sf::CircleShape pivote{10.f};
  pivote.setFillColor(f_ccontorno);
  pivote.setOrigin({10.f, 10.f});
  pivote.setPosition(circ_img.getPosition());
  target.draw(pivote);
}

Pentagono::Pentagono(float radio, const sf::Color &relleno,
                     const sf::Color &contorno)
    : Figuras{5, relleno, contorno}, m_img{radio, 5}, radio{radio} {
  m_img.setFillColor(relleno);
  m_img.setOutlineColor(contorno);
  m_img.setOutlineThickness(3);
  // anchor
  m_img.setOrigin({radio, radio});
}
void Pentagono::onUpdate(float dt) {
  m_img.setPosition({transform->posicion.x, transform->posicion.y});
  (void)dt;
}
void Pentagono::draw(sf::RenderTarget &target, sf::RenderStates state) const {
  state.transform *= getTransform();
  target.draw(m_img);
  sf::CircleShape pivote{10.f};
  pivote.setFillColor(f_ccontorno);
  pivote.setOrigin({10.f, 10.f});
  pivote.setPosition(m_img.getPosition());
  target.draw(pivote);
}

// Clase Triangulo

Triangulo::Triangulo(float radio, const sf::Color &relleno,
                     const sf::Color &contorno)
    : Figuras{5, relleno, contorno}, t_img{radio, 3}, radio{radio} {
  t_img.setFillColor(relleno);
  t_img.setOutlineColor(contorno);
  t_img.setOutlineThickness(3);
  // anchor
  t_img.setOrigin({radio, radio});
}
void Triangulo::onUpdate(float dt) {
  t_img.setPosition({transform->posicion.x, transform->posicion.y});
  (void)dt;
}
void Triangulo::draw(sf::RenderTarget &target, sf::RenderStates state) const {
  state.transform *= getTransform();
  target.draw(t_img);
  sf::CircleShape pivote{10.f};
  pivote.setFillColor(f_ccontorno);
  pivote.setOrigin({10.f, 10.f});
  pivote.setPosition(t_img.getPosition());
  target.draw(pivote);
}
// Clase Hexagono
Hexagono::Hexagono(float radio, const sf::Color &relleno,
                   const sf::Color &contorno)
    : Figuras{5, relleno, contorno}, h_img{radio, 6}, radio{radio} {
  h_img.setFillColor(relleno);
  h_img.setOutlineColor(contorno);
  h_img.setOutlineThickness(3);
  // anchor
  h_img.setOrigin({radio, radio});
}
void Hexagono::onUpdate(float dt) {
  h_img.setPosition({transform->posicion.x, transform->posicion.y});
  (void)dt;
}
void Hexagono::draw(sf::RenderTarget &target, sf::RenderStates state) const {
  state.transform *= getTransform();
  target.draw(h_img);
  sf::CircleShape pivote{10.f};
  pivote.setFillColor(f_ccontorno);
  pivote.setOrigin({10.f, 10.f});
  pivote.setPosition(h_img.getPosition());
  target.draw(pivote);
}
// Clase Octagono
Octagono::Octagono(float radio, const sf::Color &relleno,
                   const sf::Color &contorno)
    : Figuras{5, relleno, contorno}, o_img{radio, 6}, radio{radio} {
  o_img.setFillColor(relleno);
  o_img.setOutlineColor(contorno);
  o_img.setOutlineThickness(3);
  // anchor
  o_img.setOrigin({radio, radio});
}
void Octagono::onUpdate(float dt) {
  o_img.setPosition({transform->posicion.x, transform->posicion.y});
  (void)dt;
}
void Octagono::draw(sf::RenderTarget &target, sf::RenderStates state) const {
  state.transform *= getTransform();
  target.draw(o_img);
  sf::CircleShape pivote{10.f};
  pivote.setFillColor(f_ccontorno);
  pivote.setOrigin({10.f, 10.f});
  pivote.setPosition(o_img.getPosition());
  target.draw(pivote);
}
CargadorFiguras::CargadorFiguras(const std::string &ruta) : m_ruta(ruta) {}

std::vector<std::shared_ptr<Figuras>> CargadorFiguras::cargar() {
  std::vector<std::shared_ptr<Figuras>> lista;
  std::ifstream archivo(m_ruta);
  std::string tipo;

  if (!archivo.is_open()) {
    std::cerr << "Error al abrir: " << m_ruta << "\n";
    return lista;
  }

  while (archivo >> tipo) {
    float x, y;
    archivo >> x >> y;

    if (tipo == "Rectangulo") {
      int w, h;
      int r1, g1, b1, r2, g2, b2;
      archivo >> w >> h >> r1 >> g1 >> b1 >> r2 >> g2 >> b2;
      auto fig = std::make_shared<Rectangulo>(w, h, sf::Color(r1, g1, b1),
                                              sf::Color(r2, g2, b2));
      fig->setPosicion(x, y);
      lista.push_back(fig);
    } else {
      float radio;
      int r1, g1, b1, r2, g2, b2;
      archivo >> radio >> r1 >> g1 >> b1 >> r2 >> g2 >> b2;

      std::shared_ptr<Figuras> fig = nullptr;
      if (tipo == "Circulo") {
        fig = std::make_shared<Circulo>(radio, sf::Color(r1, g1, b1),
                                        sf::Color(r2, g2, b2));
      } else if (tipo == "Triangulo") {
        fig = std::make_shared<Triangulo>(radio, sf::Color(r1, g1, b1),
                                          sf::Color(r2, g2, b2));
      } else if (tipo == "Pentagono") {
        fig = std::make_shared<Pentagono>(radio, sf::Color(r1, g1, b1),
                                          sf::Color(r2, g2, b2));
      } else if (tipo == "Hexagono") {
        fig = std::make_shared<Hexagono>(radio, sf::Color(r1, g1, b1),
                                         sf::Color(r2, g2, b2));
      } else if (tipo == "Octagono") {
        fig = std::make_shared<Octagono>(radio, sf::Color(r1, g1, b1),
                                         sf::Color(r2, g2, b2));
      }
      if (fig) {
        fig->setPosicion(x, y);
        lista.push_back(fig);
      }
    }
  }
  return lista;
}

EnteVibora::EnteVibora(float radio, const sf::Color &relleno,
                       const sf::Color &contorno)
    : Circulo{radio, relleno, contorno}, direccion{1, 0} {
  generarNuevoObjetivo();
}

void EnteVibora::generarNuevoObjetivo() {
  objetivo.x = 50.f + static_cast<float>(rand() % 700);
  objetivo.y = 50.f + static_cast<float>(rand() % 500);
}

void EnteVibora::setDireccion(float x, float y) {
  direccion.x = x;
  direccion.y = y;
}

void EnteVibora::onUpdate(float dt) {
  auto miTransform = getTransformada();

  // Calculamos las diferencias entre el objetivo y nuestra posición actual
  float dx = objetivo.x - miTransform->posicion.x;
  float dy = objetivo.y - miTransform->posicion.y;

  // Calculamos la distancia para saber si ya llegamos al punto
  float distancia = std::sqrt(dx * dx + dy * dy);

  if (distancia < 5.0f) {
    // Si ya llegamos (o estamos muy cerca), buscamos un nuevo punto
    generarNuevoObjetivo();
  } else {
    // Obtenemos el ángulo en radianes hacia el objetivo usando atan2
    float angulo = std::atan2(dy, dx);

    // Convertimos el ángulo en un vector de dirección normalizado (x, y)
    direccion.x = std::cos(angulo);
    direccion.y = std::sin(angulo);
  }

  // Movemos la cabeza de la víbora
  miTransform->posicion.x += direccion.x * velocidad * dt;
  miTransform->posicion.y += direccion.y * velocidad * dt;

  Circulo::onUpdate(dt);

  auto cuerpo = getComponente<ICPartesCuerpo>();
  if (!cuerpo)
    return;

  if (!cuerpo->partes.empty()) {
    cuerpo->partes[0]->posiciones.push(miTransform->posicion);
  }

  for (size_t i = 0; i < cuerpo->partes.size(); ++i) {
    auto &parteActual = cuerpo->partes[i];

    if (!parteActual->hacerAccion) {
      parteActual->timer->curr_frame++;
      if (parteActual->timer->curr_frame >= parteActual->timer->max_frame) {
        parteActual->hacerAccion = true;
      }
    }

    if (parteActual->hacerAccion) {
      if (!parteActual->posiciones.vacia()) {
        CE::Vector2D nuevaPos = parteActual->posiciones.pop();
        parteActual->pos->posicion = nuevaPos;

        if (i + 1 < cuerpo->partes.size()) {
          cuerpo->partes[i + 1]->posiciones.push(nuevaPos);
        }
      }
    }

    if (parteActual->parte->figura) {
      parteActual->parte->figura->setPosicion(parteActual->pos->posicion.x,
                                              parteActual->pos->posicion.y);
      parteActual->parte->figura->onUpdate(dt);
    }
  }

  tiempoAcumulado += dt;
  if (tiempoAcumulado >= 3.0f) {
    tiempoAcumulado = 0.f;
    agregarNuevaParte(cuerpo);
  }
}
void EnteVibora::agregarNuevaParte(ICPartesCuerpo *cuerpo) {
  auto nuevaParte = std::make_shared<ICParte>();

  // Retraso para que las partes no se encimen
  nuevaParte->timer->max_frame = 15;

  auto fig = std::make_shared<Triangulo>(cuerpo->width / 2.f, sf::Color::Red,
                                         sf::Color::White);

  if (cuerpo->partes.empty()) {
    fig->setPosicion(getTransformada()->posicion.x,
                     getTransformada()->posicion.y);
  } else {
    auto &ultimaParte = cuerpo->partes.back();
    fig->setPosicion(ultimaParte->pos->posicion.x,
                     ultimaParte->pos->posicion.y);
  }

  nuevaParte->parte->figura = fig;
  cuerpo->partes.push_back(nuevaParte);
}

void EnteVibora::draw(sf::RenderTarget &target, sf::RenderStates state) const {
  auto cuerpo = getComponente<ICPartesCuerpo>();
  if (cuerpo) {
    for (auto &p : cuerpo->partes) {
      if (p->parte->figura) {
        target.draw(*p->parte->figura, state);
      }
    }
  }
  Circulo::draw(target, state);
}
} // namespace IVJ
