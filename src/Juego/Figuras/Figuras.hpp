#pragma once
#include "../../Motor/Primitivos/Objetos.hpp"
#include <SFML/Graphics.hpp>

namespace IVJ {
class Figuras : public CE::Objeto {
public:
  explicit Figuras(int l, const sf::Color &relleno, const sf::Color &contorno);
  ~Figuras() override {};
  friend std::ostream &operator<<(std::ostream &os,
                                  const std::shared_ptr<Figuras> &fig) {
    os << "Lados: " << fig->f_lados
       << "\nRelleno: " << fig->f_crelleno.toInteger()
       << "\nContorno:" << fig->f_ccontorno.toInteger() << "\n";
    return os;
  }

protected:
  int f_lados;
  sf::Color f_crelleno;
  sf::Color f_ccontorno;
};

class Rectangulo : public Figuras {
public:
  explicit Rectangulo(float ancho, float largo, const sf::Color &relleno,
                      const sf::Color &contorno);
  ~Rectangulo() override {};
  void draw(sf::RenderTarget &target, sf::RenderStates state) const override;
  void onUpdate(float dt) override;

private:
  sf::RectangleShape rect_img;

public:
  float w;
  float h;
};

class Circulo : public Figuras {
public:
  explicit Circulo(float radio, const sf::Color &relleno,
                   const sf::Color &contorno);
  ~Circulo() override {};
  void draw(sf::RenderTarget &target, sf::RenderStates state) const override;
  void onUpdate(float dt) override;

private:
  sf::CircleShape circ_img;

public:
  float radio;
};

class Pentagono : public Figuras {
public:
  explicit Pentagono(float radio, const sf::Color &relleno,
                     const sf::Color &contorno);
  ~Pentagono() override {};
  void draw(sf::RenderTarget &target, sf::RenderStates state) const override;
  void onUpdate(float dt) override;

private:
  sf::CircleShape m_img;

public:
  float radio;
};

class Triangulo : public Figuras {
public:
  explicit Triangulo(float radio, const sf::Color &relleno,
                     const sf::Color &contorno);
  ~Triangulo() override {};
  void draw(sf::RenderTarget &target, sf::RenderStates state) const override;
  void onUpdate(float dt) override;

private:
  sf::CircleShape t_img;

public:
  float radio;
};

class Hexagono : public Figuras {
public:
  explicit Hexagono(float radio, const sf::Color &relleno,
                    const sf::Color &contorno);
  ~Hexagono() override {};
  void draw(sf::RenderTarget &target, sf::RenderStates state) const override;
  void onUpdate(float dt) override;

private:
  sf::CircleShape h_img;

public:
  float radio;
};

class Octagono : public Figuras {
public:
  explicit Octagono(float radio, const sf::Color &relleno,
                    const sf::Color &contorno);
  ~Octagono() override {};
  void draw(sf::RenderTarget &target, sf::RenderStates state) const override;
  void onUpdate(float dt) override;

private:
  sf::CircleShape o_img;

public:
  float radio;
};

class FileLoader {
public:
  FileLoader(const std::string &pathfile);
  ~FileLoader();
  std::vector<std::shared_ptr<Figuras>> LoadFile();

private:
  std::vector<std::shared_ptr<Figuras>> lista;
  std::string archivo;
};
} // namespace IVJ
