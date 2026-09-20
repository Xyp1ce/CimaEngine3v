#pragma once
#include "../../Motor/Componentes/IComponentes.hpp"
#include "../../Motor/Primitivos/Objetos.hpp"
#include "../Maquinas/FSM.hpp"
#include "Motor/Primitivos/CEPool.hpp"
#include "Motor/Utils/Vector2D.hpp"
#include <Juego/Figuras/Figuras.hpp>

namespace IVJ {
class FSM; // refefencia circular
// Maquina estado componente
class IMaquinaEstado : public CE::IComponentes {
public:
  explicit IMaquinaEstado();
  ~IMaquinaEstado() override {};

  std::shared_ptr<IComponentes> clonar() const override {
    return std::make_shared<IMaquinaEstado>(*this);
  };

public:
  std::shared_ptr<FSM> fsm;
  bool congelar{false};
};

// practica de camaras o también para debug
class ITriangulo : public CE::IComponentes {
public:
  explicit ITriangulo(float r, float ang);
  ~ITriangulo() override {};
  std::shared_ptr<IComponentes> clonar() const override {
    return std::make_shared<ITriangulo>(*this);
  };

public:
  sf::CircleShape tri_shape;
  float angulo;
};
// componente QUEST con un arreglo de int de 8 bit

class IRayo : public CE::IComponentes {
public:
  explicit IRayo(CE::Vector2D &pos, float &sig, const float magnitud = 1.f);
  ~IRayo() override {};
  std::shared_ptr<IComponentes> clonar() const override {
    return std::make_shared<IRayo>(*this);
  };

private:
  float magnitud;
  float &dir;
  CE::Vector2D &lp1;
  CE::Vector2D lp2;

public:
  CE::Vector2D &getP1() const;
  CE::Vector2D &getP2();
  float getMagnitud() const { return magnitud; }
};

class IInteractuable : public CE::IComponentes {
public:
  IInteractuable() = default;
  virtual ~IInteractuable() override {};
  virtual void onInteractuar(CE::Objeto &obj) = 0;

  std::shared_ptr<IComponentes> clonar() const override = 0;

public:
  bool interactuado{false};
};

class IIndicador : public IInteractuable {
public:
  explicit IIndicador(const sf::Texture &ref, float escala, Entidad *p,
                      const sf::Color &c);
  ~IIndicador() override {};
  void onInteractuar(CE::Objeto &obj) override;
  void onRender();
  std::shared_ptr<IComponentes> clonar() const override { return nullptr; };

public:
  sf::Sprite sprite;
  sf::Shader shader;
  sf::Color color;
  float escala;
  int frame_activo{0};
  bool activo{false};

private:
  Entidad *parent;
};

class IDialogo : public IInteractuable {
public:
  IDialogo();
  ~IDialogo() {};
  void onInteractuar(CE::Objeto &obj) override;
  void onRender();
  std::shared_ptr<IComponentes> clonar() const override { return nullptr; };

private:
  std::wstring agregarSaltoLinea(const std::wstring &str, size_t max_len);

public:
  std::wstring texto;
  int id_texto;
  bool activo{false};
};

class IGirar : public CE::IComponentes {
public:
  explicit IGirar(const float ang, const float r);

  virtual ~IGirar() override {};
  // esto permite copiar el componente sin crear referencia
  // se tiene que implementar afueras (error de diseño mio)
  std::shared_ptr<IComponentes> clonar() const override {
    return std::make_shared<IGirar>(*this);
  };

public:
  float angulo;
  float radio;
};

class IVertical : public CE::IComponentes {
public:
  explicit IVertical(const float ang, const float r);

  virtual ~IVertical() override {};
  // esto permite copiar el componente sin crear referencia
  // se tiene que implementar afueras (error de diseño mio)
  std::shared_ptr<IComponentes> clonar() const override {
    return std::make_shared<IVertical>(*this);
  };

public:
  float angulo;
  float radio;
};

class IOnda : public CE::IComponentes {
public:
  explicit IOnda(const float ang, const float r, const int dir);

  virtual ~IOnda() override {};
  std::shared_ptr<IComponentes> clonar() const override {
    return std::make_shared<IOnda>(*this);
  };

public:
  float angulo;
  float radio;
  int direccion;
};
// --- 2. Componente 1: ICTimer ---
class ICTimer : public CE::IComponentes {
public:
  int curr_frame;
  int max_frame;

  explicit ICTimer(int max) : curr_frame(0), max_frame(max) {}
  ~ICTimer() override = default;

  std::shared_ptr<IComponentes> clonar() const override {
    return std::make_shared<ICTimer>(*this);
  }
};

class Figuras;
class ICFigura : public CE::IComponentes {
public:
  std::shared_ptr<Figuras> figura;

  explicit ICFigura(std::shared_ptr<Figuras> fig) : figura(fig) {}
  ~ICFigura() override = default;

  std::shared_ptr<IComponentes> clonar() const override {
    return std::make_shared<ICFigura>(*this);
  }
};

struct MiColaPosiciones {
private:
  static const int CAPACIDAD = 1000; // Suficiente para guardar un rastro
  CE::Vector2D datos[CAPACIDAD];
  int frente = 0;
  int final = 0;
  int tamano = 0;

public:
  void push(const CE::Vector2D &val) {
    if (tamano < CAPACIDAD) {
      datos[final] = val;
      final = (final + 1) % CAPACIDAD;
      tamano++;
    }
  }
  CE::Vector2D pop() {
    if (tamano > 0) {
      CE::Vector2D val = datos[frente];
      frente = (frente + 1) % CAPACIDAD;
      tamano--;
      return val;
    }
    return CE::Vector2D(0, 0); // O manejar el error
  }
  bool vacia() const { return tamano == 0; }
  int size() const { return tamano; }
};

// --- 2. Componente 2: ICParte ---
class ICParte : public CE::IComponentes {
public:
  MiColaPosiciones posiciones;     // 1) Cola de posiciones
  std::shared_ptr<ICFigura> parte; // 2) Componente figura
  std::shared_ptr<CE::ITransform>
      pos; // 3) Componente transform. Usando la clase de CimaEngine
  std::shared_ptr<ICTimer> timer; // 4) Componente timer
  bool hacerAccion;               // 5) Booleano

  // 3. Inicialización en el constructor
  explicit ICParte() : hacerAccion(false) {
    // Inicializamos los componentes atributos
    // Nota: La figura específica se asignará desde fuera
    parte = std::make_shared<ICFigura>(nullptr);
    pos = std::make_shared<CE::ITransform>();
    timer = std::make_shared<ICTimer>(10); // Tiempo de retraso (ej. 10 frames)
  }
  ~ICParte() override = default;

  std::shared_ptr<IComponentes> clonar() const override {
    // Clonar este componente requiere cuidado con los punteros,
    // pero por simplicidad haremos una copia básica.
    return std::make_shared<ICParte>(*this);
  }
};

// --- 2. Componente 3: ICPartesCuerpo ---
class ICPartesCuerpo : public CE::IComponentes {
public:
  std::vector<std::shared_ptr<ICParte>> partes; // 1) Vector de partes
  int width;                                    // 2) Ancho
  int height;                                   // 3) Alto

  explicit ICPartesCuerpo(int w, int h) : width(w), height(h) {}
  ~ICPartesCuerpo() override = default;

  std::shared_ptr<IComponentes> clonar() const override {
    return std::make_shared<ICPartesCuerpo>(*this);
  }
};

class IEstadoInterno : public CE::IComponentes {
public:
  enum class Estados {
    BUSCAR,
    ENMOVIMIENTOCOMIDA,
    ENMOVIMIENTOCASA,
    CONSUMIR,
    REPRODUCIR // se agrego para reproducir
  };

public:
  explicit IEstadoInterno(const IEstadoInterno::Estados &estado_inicial);
  virtual ~IEstadoInterno() override {};
  std::shared_ptr<IComponentes> clonar() const override {
    return std::make_shared<IEstadoInterno>(*this);
  };

  void setEstadoInterno(const IEstadoInterno::Estados &ne);
  IEstadoInterno::Estados getEstadoInterno() const { return estado; };

private:
  IEstadoInterno::Estados estado;
};

class IInventarioComida : public CE::IComponentes {
public:
  virtual ~IInventarioComida() override {};
  void guardarComida(const std::shared_ptr<Circulo> &c);
  void sacarComida();
  std::weak_ptr<Circulo> getComidaGuardada() const { return comida; };
  std::shared_ptr<IComponentes> clonar() const override {
    return std::make_shared<IInventarioComida>(*this);
  };

private:
  std::weak_ptr<Circulo> comida;
};
class ITargetComida : public CE::IComponentes {
public:
  virtual ~ITargetComida() override {};
  void setTargetComida(const std::shared_ptr<Circulo> &c);
  void quitarTarget();
  std::weak_ptr<Circulo> getTargetComida() const { return target; };
  std::shared_ptr<IComponentes> clonar() const override {
    return std::make_shared<ITargetComida>(*this);
  };

private:
  std::weak_ptr<Circulo> target;
};
class IPosicionInicial : public CE::IComponentes {
public:
  explicit IPosicionInicial(float x, float y);
  virtual ~IPosicionInicial() override {};
  std::shared_ptr<IComponentes> clonar() const override {
    return std::make_shared<IPosicionInicial>(*this);
  };

public:
  CE::Vector2D pos_init;
};
class ITieneDueño : public CE::IComponentes {
public:
  virtual ~ITieneDueño() override {};
  std::shared_ptr<IComponentes> clonar() const override {
    return std::make_shared<ITieneDueño>(*this);
  };

public:
  bool tiene{false};
};
class IScore : public CE::IComponentes {
public:
  virtual ~IScore() override {};
  std::shared_ptr<IComponentes> clonar() const override {
    return std::make_shared<IScore>(*this);
  };

public:
  int score{0};
};
} // namespace IVJ
