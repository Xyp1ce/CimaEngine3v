#include "Sistemas.hpp"
#include "../../Motor/Componentes/IComponentes.hpp"
#include "../../Motor/Primitivos/GestorAssets.hpp"
#include "../../Motor/Render/Render.hpp"
#include "../../Motor/Utils/Lerp.hpp"
#include "../objetos/Entidad.hpp"
#include "Motor/GUI/GLogger.hpp"
#include "Motor/Utils/Vector2D.hpp"
#include <cmath>
#include <codecvt>
#include <memory>

#include <Juego/Componentes/IJComponentes.hpp>
#include <Juego/Figuras/Figuras.hpp>
#include <string>

namespace IVJ {
void SistemaControl(CE::Objeto &ente, float dt) {
  // no hay necesidad verificar si tiene, se asume que tiene
  // control y transformada
  auto p = ente.getTransformada();
  auto c = ente.getComponente<CE::IControl>();
  (void)dt;
  if (c->arr)
    p->velocidad.y = -200;
  if (c->abj)
    p->velocidad.y = 200;
  if (c->der)
    p->velocidad.x = 200;
  if (c->izq)
    p->velocidad.x = -200;
  if (c->run)
    p->velocidad.escala(2.f);
}

void SistemaMover(const std::vector<std::shared_ptr<CE::Objeto>> &entes,
                  float dt) {

  for (auto &ente : entes) {
    // todo ente tiene ITransform por lo que no requiere verificación
    auto trans = ente->getTransformada();
    trans->posicion.suma(trans->velocidad.escala(dt));
  }
}

void SistemaMoverBalas(const std::shared_ptr<CE::Objeto> &ente, float dt) {
  auto trans = ente->getTransformada();
  auto vel = ente->getTransformada()->velocidad;
  trans->posicion.suma(vel.escala(dt));
}

void SistemaMover(const std::shared_ptr<CE::Objeto> &objeto, float dt) {
  auto trans = objeto->getTransformada();

  auto control = objeto->getComponente<CE::IControl>();
  if (!control || !control->isActivo())
    return;

  auto vel = CE::Vector2D{0.f, 0.f};
  if (control->arr)
    vel.y = -trans->velocidad.y;
  if (control->abj)
    vel.y = trans->velocidad.y;
  if (control->der)
    vel.x = trans->velocidad.x;
  if (control->izq)
    vel.x = -trans->velocidad.x;

  if (objeto->getComponente<ITriangulo>() ||
      objeto->getComponente<CE::ISprite>()) {
    auto n = vel;
    n.normalizacion();
    auto trian = objeto->getComponente<ITriangulo>(); // nulo
    if (vel.x != 0 || vel.y != 0) {
      if (trian)
        trian->angulo = std::atan2(n.x, -n.y);
      trans->angulo = std::atan2(n.x, -n.y);
    }
  }

  trans->posicion.suma(vel.escala(dt));
}

bool SistemaColAABB(CE::Objeto &A, CE::Objeto &B, bool resolucion) {
  if (!A.tieneComponente<CE::IBoundingBox>() ||
      !B.tieneComponente<CE::IBoundingBox>())
    return false;
  auto bA = A.getComponente<CE::IBoundingBox>()->tam;
  auto mA = A.getComponente<CE::IBoundingBox>()->mitad;
  auto *pa = &A.getTransformada()->posicion;
  auto prevA = A.getTransformada()->pos_prev;
  auto bB = B.getComponente<CE::IBoundingBox>()->tam;
  auto mB = B.getComponente<CE::IBoundingBox>()->mitad;
  auto pb = B.getTransformada()->posicion;
  // calculos
  bool H =
      pa->y - mA.y < pb.y + bB.y - mB.y && pb.y - mB.y < pa->y + bA.y - mA.y;
  bool V =
      pa->x - mA.x < pb.x + bB.x - mB.x && pb.x - mB.x < pa->x + bA.x - mA.x;
  bool hay_colision = H && V;
  if (hay_colision && resolucion)
    *pa = prevA;
  return hay_colision;
}

bool SistemaColAABBMid(CE::Objeto &A, CE::Objeto &B, bool resolucion) {
  if (!A.tieneComponente<CE::IBoundingBox>() ||
      !B.tieneComponente<CE::IBoundingBox>())
    return false;
  auto midA = A.getComponente<CE::IBoundingBox>()->mitad;
  auto *pa = &A.getTransformada()->posicion;
  auto prevA = A.getTransformada()->pos_prev;
  auto midB = B.getComponente<CE::IBoundingBox>()->mitad;
  auto *pb = &B.getTransformada()->posicion;
  // calculos
  float dX = std::abs(pb->x - pa->x);
  float dY = std::abs(pb->y - pa->y);
  float sumMidX = midA.x + midB.x;
  float sumMidY = midA.y + midB.y;
  bool V = sumMidX - dX > 0;
  bool H = sumMidY - dY > 0;
  bool hay_colision = V && H;
  if (resolucion && hay_colision)
    *pa = prevA;
  return hay_colision;
}

bool checkRayoHit(CE::Objeto &npc, CE::Vector2D &p1, CE::Vector2D &p2) {
  if (!npc.tieneComponente<CE::IBoundingBox>())
    return false;

  auto midnpc = npc.getComponente<CE::IBoundingBox>()->mitad;
  auto posnpc = npc.getTransformada()->posicion;
  auto linea = (p2 - p1); //.escala(1.f);
  // std::cout<<"Mid linea: "<<midlinea<<"\n";
  float dx = std::abs(posnpc.x - p1.x);
  float dy = std::abs(posnpc.y - p1.y);
  float sumMidX = midnpc.x + std::abs(linea.x);
  float sumMidY = midnpc.y + std::abs(linea.y);

  return (sumMidX - dx > 0) && (sumMidY - dy > 0);
}

void SistemaSpawn(CE::Pool &spawns) {
  for (auto &s : spawns.getPool()) {
    auto respawndata = s->getComponente<CE::IRespawn>();
    if (!respawndata || respawndata->curr_spawn >= respawndata->max_spawn)
      continue;
    // respawn
    if (respawndata->timer_actual >= respawndata->timer_maximo) {
      // crear objeto
      auto malillo = std::make_shared<Entidad>(); //(*(respawndata->prefab));
      // auto comp = respawndata->prefab->getListaComponentes();
      // auto comp = respawndata->prefab->getComponente<CE::ISprite>();
      // for(auto &c : comp)
      //{
      // malillo->copyComponente<CE::ISprite>(comp);
      // malillo->copyComponente<CE::IPaths>(respawndata->prefab->getComponente<CE::IPaths>());
      //}

      malillo->addComponente(std::make_shared<CE::IPaths>(60))
          .addComponente(std::make_shared<CE::ISprite>(
              CE::GestorAssets::Get().getTextura("hoja_blue"), 68, 91, 1.f))
          .addComponente(std::make_shared<CE::IShader>(
              "", ASSETS "/shaders/contorno.frag"));
      malillo->getComponente<CE::IPaths>()->addCurva(
          CE::Vector2D{500.f, 300.f}, CE::Vector2D{600.f, 500.f},
          CE::Vector2D{800.f, 300.f});
      malillo->getComponente<CE::IPaths>()->addCurva(
          CE::Vector2D{800.f, 300.f}, CE::Vector2D{600.f, 100.f},
          CE::Vector2D{500.f, 300.f});

      // mandar datos al shader
      auto shader = malillo->getComponente<CE::IShader>();
      static sf::Texture textura =
          malillo->getComponente<CE::ISprite>()->m_sprite.getTexture();

      static sf::Glsl::Vec4 uOutlineColor{1.f, 0.f, 1.f, 1.f};
      // static  float uEdgeThreshold= 0.15f; //0.15-0.35
      // static  float uEdgeSoftness = 0.02f;
      static float uThickness = 0.15f;
      auto w = malillo->getComponente<CE::ISprite>()->width;
      auto h = malillo->getComponente<CE::ISprite>()->height;
      static sf::Glsl::Vec2 uTexSize{(float)w, (float)h};

      shader->setTextura("uTex", &textura);
      shader->setVector4("uOutlineColor", &uOutlineColor);
      // shader->setEscalar("uEdgeThreshold",&uEdgeThreshold);
      // shader->setEscalar("uEdgeSoftness",&uEdgeSoftness);
      shader->setEscalar("uThickness", &uThickness);
      shader->setVector2("uTexSize", &uTexSize);

      // posiciones
      malillo->getStats()->hp = 100;
      // asumimos que spwan tine un cuadro , tomamos el cuadro como el area
      int size_x = respawndata->width;
      int size_y = respawndata->height;
      float x0 = s->getTransformada()->posicion.x - size_x / 2.f;
      float y0 = s->getTransformada()->posicion.y - size_y / 2.f;
      float x = x0 + (rand() % size_x);
      float y = y0 + (rand() % size_y);
      malillo->setPosicion(x, y);
      // spawns.push_back(malillo);
      spawns.agregarPool(malillo);
      respawndata->timer_actual = 0;
      respawndata->curr_spawn++;
    }
    respawndata->timer_actual++;
  }
}
void SistemaPaths(std::vector<std::shared_ptr<CE::Objeto>> &obj) {
  for (auto &o : obj) {
    if (o->tieneComponente<CE::IPaths>()) {
      auto path = o->getComponente<CE::IPaths>();
      int num_curvas = path->puntos.size() / 3;
      if (path->puntos.size() == 0)
        continue;
      if (path->id_curva >= num_curvas)
        continue;
      float t = (path->frame_actual_curva % (path->frame_total_curva + 1)) /
                (float)path->frame_total_curva;
      // std::cout<<o<<" t: "<<t<<"\n";
      CE::Vector2D P0 = path->puntos[0 + path->id_curva * (path->offset - 0)];
      CE::Vector2D P1 = path->puntos[1 + path->id_curva * (path->offset - 0)];
      CE::Vector2D P2 = path->puntos[2 + path->id_curva * (path->offset - 0)];
      auto posiciones_actual = CE::lerp2(P0, P1, P2, t);
      path->frame_actual_curva++;
      o->setPosicion(posiciones_actual.x, posiciones_actual.y);
      if (t == 1.f)
        path->id_curva++;
      // path->frame_actual_curva=0;
      // std::cout<<"[ "<<o<<" ] "<<path->frame_actual_curva<<"\n";
      if (path->id_curva == 2 && o->tieneComponente<CE::IShader>()) {
        // cambiar shader
        auto shader = o->getComponente<CE::IShader>();
        shader->cambiarShader("", ASSETS "/shaders/color.frag");
        static sf::Glsl::Vec4 color{0.f, 1.f, 0.f, 1.f};
        shader->m_vars.clear();
        shader->setVector4("color", &color);
      } else {
        // cambiar shader
        auto shader = o->getComponente<CE::IShader>();
        shader->cambiarShader("", ASSETS "/shaders/contorno.frag");
        static sf::Glsl::Vec4 outlinecolor{0.f, 1.f, 0.f, 1.f};
        static float thickness = 0.1;
        static sf::Glsl::Vec2 size{63, 92};
        shader->m_vars.clear();
        shader->setVector4("uOutlineColor", &outlinecolor);
        shader->setEscalar("uThickness", &thickness);
        shader->setVector2("uTexSize", &size);
      }
    }
  }
}

// Dialogos
void SistemaDialogos(IVJ::IDialogo *dialogo, CE::Objeto &obj) {
  if (!dialogo)
    return;
  dialogo->activo = true;
  dialogo->onInteractuar(obj);
}

bool revisarDistanciaInteraccion(CE::Objeto &jugador, CE::Objeto &npc,
                                 float distancia_maxima) {
  auto pos_jugador = jugador.getTransformada()->posicion;
  auto pos_npc = npc.getTransformada()->posicion;
  float distancia = pos_jugador.distancia(pos_npc);
  return distancia <= distancia_maxima;
}

// DEBUG
void pintarLinea(CE::Vector2D &p1, CE::Vector2D &p2, const sf::Color &color) {
  sf::CircleShape cp1{2.f};
  cp1.setFillColor({0, 0, 0, 255});
  sf::CircleShape cp2{2.f};
  cp2.setFillColor({0, 0, 0, 255});
  cp1.setPosition({p1.x, p1.y});
  cp2.setPosition({p2.x, p2.y});
  // pintar lineas de guía
  for (float t = 0; t <= 1; t += 0.0015) {
    sf::CircleShape pixel{1};
    pixel.setFillColor(color);
    auto puntopos = CE::lerp(p1, p2, t);
    pixel.setPosition({puntopos.x, puntopos.y});
    CE::Render::Get().AddToDraw(pixel);
  }
  CE::Render::Get().AddToDraw(cp1);
  CE::Render::Get().AddToDraw(cp2);
}

void SistemaGirar(CE::Objeto &ente, float dt) {
  if (!ente.getComponente<IGirar>())
    return;
  auto componente = ente.getComponente<IGirar>();
  auto pos = ente.getTransformada()->posicion;
  float radio = componente->radio;
  float x = radio * cos(componente->angulo);
  float y = radio * sin(componente->angulo);
  ente.setPosicion(pos.x + x, pos.y + y);
  componente->angulo += 3.146 * dt;
}

void SistemaVertical(CE::Objeto &ente, float dt) {
  if (!ente.getComponente<IVertical>())
    return;
  auto componente = ente.getComponente<IVertical>();
  auto pos = ente.getTransformada()->posicion;
  float radio = componente->radio;
  float y = radio * sin(componente->angulo);
  ente.setPosicion(pos.x, pos.y + y);
  componente->angulo += 3.146f * dt;
}

void SistemaOnda(CE::Objeto &ente, float dt) {
  if (!ente.getComponente<IOnda>())
    return;
  auto componente = ente.getComponente<IOnda>();
  auto pos = ente.getTransformada()->posicion;
  auto dim = CE::Render::Get().GetVentana().getSize();
  // direccion = 1 -> derecha
  // direccion = -1 -> izquierda
  float avance_x = 150.f * dt;
  if (pos.x > dim.x)
    componente->direccion = -1;
  if (pos.x < 0)
    componente->direccion = 1;
  int dir = componente->direccion;
  if (dir == -1)
    avance_x = -150.f * dt;
  float radio = componente->radio;
  float y = radio * sin(componente->angulo);
  float nueva_x = pos.x + avance_x;
  float nueva_y = pos.y + y;
  ente.setPosicion(nueva_x, nueva_y);
  componente->angulo += 3.146f * dt;
}

void SistemaBuscarComida(
    CE::Objeto &ente, const std::vector<std::shared_ptr<CE::Objeto>> &objetos,
    float dt) {

  if (!ente.tieneComponente<IEstadoInterno>() &&
      !ente.tieneComponente<IInventarioComida>())
    return;
  if (ente.getComponente<ITargetComida>()->getTargetComida().lock() != nullptr)
    return;
  if (ente.getComponente<IEstadoInterno>()->getEstadoInterno() !=
      IEstadoInterno::Estados::BUSCAR)
    return;

  std::shared_ptr<Circulo> comida_mas_cerca = nullptr;
  float dist_min = 9999999999999.f;
  float rango_vision = ente.getStats()->agi; // Usar AGI como rango

  for (auto &objeto : objetos) {
    std::shared_ptr<Circulo> comida =
        std::dynamic_pointer_cast<Circulo>(objeto);
    if (!comida)
      continue;
    if (comida->getComponente<ITieneDueño>()->tiene)
      continue;

    auto mi_pos = ente.getTransformada()->posicion;
    float dist = mi_pos.distancia(comida->getTransformada()->posicion);

    // Condición: debe estar más cerca Y dentro del rango de visión
    if (dist < dist_min && dist <= rango_vision) {
      dist_min = dist;
      comida_mas_cerca = comida;
    }
  }

  if (comida_mas_cerca != nullptr) {
    ente.getComponente<ITargetComida>()->setTargetComida(comida_mas_cerca);
    ente.getComponente<IEstadoInterno>()->setEstadoInterno(
        IEstadoInterno::Estados::ENMOVIMIENTOCOMIDA);
  } else {
    // Si no encontró comida en su rango, caminar hacia enfrente con
    // comportamiento Wander
    auto po = ente.getTransformada()->posicion;
    auto vo = ente.getTransformada()->velocidad;

    // 1. Variación aleatoria al ángulo (entre -15 y 15 grados) para que
    // exploren en 2D
    float variacion_angulo = (std::rand() % 31) - 15.f;
    ente.getTransformada()->angulo += variacion_angulo;

    // 2. Calcular la nueva posición con el ángulo actualizado
    float angulo_rad = ente.getTransformada()->angulo * 3.14159265f / 180.f;
    float dx = po.x + (std::cos(angulo_rad) * vo.x * dt);
    float dy = po.y + (std::sin(angulo_rad) * vo.y * dt);

    // 3. Restricciones del mundo (Rectángulo blanco: x de 50 a 1000, y de 50 a
    // 650) Rebote horizontal
    if (dx < 50.f) {
      dx = 50.f;
      ente.getTransformada()->angulo = 180.f - ente.getTransformada()->angulo;
    } else if (dx > 1000.f) {
      dx = 1000.f;
      ente.getTransformada()->angulo = 180.f - ente.getTransformada()->angulo;
    }

    // Rebote vertical
    if (dy < 50.f) {
      dy = 50.f;
      ente.getTransformada()->angulo = 360.f - ente.getTransformada()->angulo;
    } else if (dy > 650.f) {
      dy = 650.f;
      ente.getTransformada()->angulo = 360.f - ente.getTransformada()->angulo;
    }

    ente.setPosicion(dx, dy);

    // 4. Actualizar la rotación visual para que el pentágono "mire" a donde
    // camina
    auto pentagono = dynamic_cast<IVJ::Pentagono *>(&ente);
    if (pentagono) {
      pentagono->getShape().setRotation(
          sf::degrees(ente.getTransformada()->angulo));
    }
  }
}
void SistemaMoveraComidaoCasa(CE::Objeto &ente, float dt) {
  // comida y mundo no tienen IEstadoInterno
  if (!ente.tieneComponente<IEstadoInterno>())
    return;

  // aún esta buscando
  if (ente.getComponente<IEstadoInterno>()->getEstadoInterno() !=
          IEstadoInterno::Estados::ENMOVIMIENTOCASA &&
      ente.getComponente<IEstadoInterno>()->getEstadoInterno() !=
          IEstadoInterno::Estados::ENMOVIMIENTOCOMIDA)
    return;

  // desplazarse hacia casa o comida
  auto estado = ente.getComponente<IEstadoInterno>()->getEstadoInterno();
  switch (estado) {
  case IEstadoInterno::Estados::ENMOVIMIENTOCOMIDA: {
    auto po = ente.getTransformada()->posicion;
    auto vo = ente.getTransformada()->velocidad;
    std::shared_ptr<Circulo> target =
        ente.getComponente<ITargetComida>()->getTargetComida().lock();
    // no tiene target
    if (!target) {
      ente.getComponente<IEstadoInterno>()->setEstadoInterno(
          IEstadoInterno::Estados::BUSCAR);
      ente.getComponente<ITargetComida>()->quitarTarget();
      return;
    }
    auto pt = target->getTransformada()->posicion;
    auto dir = pt - po;
    float dx = po.x + (dir.x * vo.x * dt);
    float dy = po.y + (dir.y * vo.y * dt);
    ente.setPosicion(dx, dy);
    float dist = po.distancia(pt);
    if (target->getComponente<ITieneDueño>()->tiene) {
      ente.getComponente<IEstadoInterno>()->setEstadoInterno(
          IEstadoInterno::Estados::BUSCAR);
      ente.getComponente<ITargetComida>()->quitarTarget();
    } else if (dist < 0.15) {
      // si target no tiene dueño, ser el dueño del target y guardar en
      // inventario
      ente.getComponente<IEstadoInterno>()->setEstadoInterno(
          IEstadoInterno::Estados::ENMOVIMIENTOCASA);
      ente.getComponente<IInventarioComida>()->guardarComida(target);
      target->getComponente<ITieneDueño>()->tiene = true;
    }
  } break;
  case IEstadoInterno::Estados::ENMOVIMIENTOCASA: {
    auto po = ente.getTransformada()->posicion;
    auto vo = ente.getTransformada()->velocidad;
    auto pt = ente.getComponente<IPosicionInicial>()->pos_init;
    auto dir = pt - po;
    float dx = po.x + (dir.x * vo.x * dt);
    float dy = po.y + (dir.y * vo.y * dt);
    ente.setPosicion(dx, dy);
    float dist = po.distancia(pt);
    // mover comida con el ente
    auto comida =
        ente.getComponente<IInventarioComida>()->getComidaGuardada().lock();
    comida->setPosicion(dx, dy);
    if (dist < 0.15)
      ente.getComponente<IEstadoInterno>()->setEstadoInterno(
          IEstadoInterno::Estados::CONSUMIR);
  } break;
  default:
    break;
  }
}
void SistemaConsumirComida(CE::Objeto &ente) {
  // comida y mundo no tiene IEstadoInterno ni ITargetComida
  if (!ente.tieneComponente<IEstadoInterno>() &&
      !ente.tieneComponente<ITargetComida>())
    return;
  // si no tiene target
  if (ente.getComponente<ITargetComida>()->getTargetComida().lock() == nullptr)
    return;
  // si no esta en estado consumir
  if (ente.getComponente<IEstadoInterno>()->getEstadoInterno() !=
      IEstadoInterno::Estados::CONSUMIR)
    return;

  // si llega aquí es que esta en el estado consumir y solo puede estar en este
  // estado si ya llego a la posición inicial. quitar comida del inventario
  ente.getComponente<IInventarioComida>()->sacarComida();
  // quitar comida del target del ente
  ente.getComponente<ITargetComida>()->quitarTarget();
  // no quitar propetario de comida, para que nadie lo busque, y por defecto ya
  // esta marcado para borrar incrementar score
  ente.getComponente<IScore>()->score++;
  // cambiar estado a buscar nueva comida
  ente.getComponente<IEstadoInterno>()->setEstadoInterno(
      IEstadoInterno::Estados::REPRODUCIR);
}
void SistemaReproducirEnte(CE::Objeto &ente, CE::Pool &pool) {

  // no es ente
  if (!ente.tieneComponente<IScore>() &&
      !ente.tieneComponente<IEstadoInterno>())
    return;

  // es ente pero aún no esta en la fase de reproducirse
  if (ente.getComponente<IEstadoInterno>()->getEstadoInterno() !=
      IEstadoInterno::Estados::REPRODUCIR)
    return;

  // cambiar estado a buscar nueva comida
  ente.getComponente<IEstadoInterno>()->setEstadoInterno(
      IEstadoInterno::Estados::BUSCAR);

  // no tiene suficiente score
  if (ente.getComponente<IScore>()->score < 2)
    return;

  // tiene suficiente score pero no es multiplo de 2
  if (ente.getComponente<IScore>()->score % 2 != 0)
    return;
  // si tiene un score mayor a 2 y es multiplo de 2 y es un ente, entonces lo
  // reproducimos con los mismos stats y guardamos en el pool de la escena
  auto nuevo =
      std::make_shared<Pentagono>(15.f, sf::Color::Blue, sf::Color::Black);
  nuevo->getStats()->hp_max = ente.getStats()->hp_max;
  nuevo->getStats()->hp = ente.getStats()->hp;
  nuevo->getStats()->agi = ente.getStats()->agi;
  auto pos_init = ente.getComponente<IPosicionInicial>()->pos_init;
  nuevo->getTransformada()->velocidad = ente.getTransformada()->velocidad;
  nuevo->getTransformada()->angulo = ente.getTransformada()->angulo;
  nuevo->getShape().setRotation(sf::degrees(nuevo->getTransformada()->angulo));
  nuevo->setPosicion(pos_init.x, pos_init.y);
  // componentes
  nuevo
      ->addComponente(
          std::make_shared<IEstadoInterno>(IEstadoInterno::Estados::BUSCAR))
      // agregar inventario
      .addComponente(std::make_shared<IInventarioComida>())
      // agregar target nulo
      .addComponente(std::make_shared<ITargetComida>())
      // agregar la posicion inicial
      .addComponente(std::make_shared<IPosicionInicial>(pos_init.x, pos_init.y))
      // agregar score
      .addComponente(std::make_shared<IScore>());
  pool.agregarPool(nuevo);
}
} // namespace IVJ
