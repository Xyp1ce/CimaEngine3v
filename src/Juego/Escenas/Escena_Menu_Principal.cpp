#include "Escena_Menu_Principal.hpp"
#include <Motor/Camaras/CamarasGestor.hpp>
#include <Motor/Inputs/Botones.hpp>
#include <Motor/Primitivos/GestorAssets.hpp>
#include <Motor/Primitivos/GestorEscenas.hpp>
#include <Motor/Render/Render.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <cmath>
#include <cstdlib>

namespace IVJ {

Escena_Menu_Principal::Escena_Menu_Principal() : CE::Escena{} {
  nombre = "Escena_Menu_Principal";
}

void Escena_Menu_Principal::onInit() {
  CE::GestorCamaras::Get().setCamaraActiva(0);
  opcion = 0;
  tiempo = 0.f;

  if (!inicializar) {
    return;
  }

  registrarBotones(sf::Keyboard::Scancode::W, "arriba");
  registrarBotones(sf::Keyboard::Scancode::Up, "arriba");
  registrarBotones(sf::Keyboard::Scancode::S, "abajo");
  registrarBotones(sf::Keyboard::Scancode::Down, "abajo");
  registrarBotones(sf::Keyboard::Scancode::Enter, "aceptar");
  registrarBotones(sf::Keyboard::Scancode::Space, "aceptar");
  registrarBotones(sf::Keyboard::Scancode::E, "aceptar");
  registrarBotones(sf::Keyboard::Scancode::Escape, "cerrar");

  inicializar = false;
}

void Escena_Menu_Principal::onFinal() {}

void Escena_Menu_Principal::onUpdate(float dt) {
  tiempo += dt;
}

void Escena_Menu_Principal::onInputs(const CE::Botones &accion) {
  if (accion.getTipo() != CE::Botones::TipoAccion::OnPress)
    return;

  if (accion.getNombre() == "arriba" || accion.getNombre() == "abajo") {
    opcion = (opcion == 0) ? 1 : 0;
  } else if (accion.getNombre() == "aceptar") {
    if (opcion == 0) {
      CE::GestorEscenas::Get().cambiarEscena("Escena_Propia");
      if (CE::GestorEscenas::Get().getEscenaActual().nombre != "Escena_Propia" &&
          CE::GestorEscenas::Get().getEscenaActual().nombre != "Propia") {
        CE::GestorEscenas::Get().cambiarEscena("Propia");
      }
    } else if (opcion == 1) {
      std::exit(EXIT_SUCCESS);
    }
  } else if (accion.getNombre() == "cerrar") {
    std::exit(EXIT_SUCCESS);
  }
}

void Escena_Menu_Principal::onRender() {
  auto &tex = CE::Render::Get().GetTextura();
  sf::View prevView = tex.getView();
  tex.setView(tex.getDefaultView());

  float W = static_cast<float>(tex.getSize().x);
  float H = static_cast<float>(tex.getSize().y);
  const auto &font = CE::GestorAssets::Get().getFont("default_font");

  // Fondo industrial brutalista oscuro
  sf::RectangleShape bg{{W, H}};
  bg.setFillColor(sf::Color(11, 14, 21, 255));
  tex.draw(bg);

  // Grilla sutil arquitectónica
  for (float x = 0.f; x < W; x += 48.f) {
    sf::RectangleShape line{{1.f, H}};
    line.setPosition({x, 0.f});
    line.setFillColor(sf::Color(28, 36, 52, 55));
    tex.draw(line);
  }
  for (float y = 0.f; y < H; y += 48.f) {
    sf::RectangleShape line{{W, 1.f}};
    line.setPosition({0.f, y});
    line.setFillColor(sf::Color(28, 36, 52, 55));
    tex.draw(line);
  }

  // Anillos radar decorativos
  float pulso = 0.5f + 0.5f * std::sin(tiempo * 2.4f);
  sf::CircleShape ring1{H * 0.42f};
  ring1.setOrigin({ring1.getRadius(), ring1.getRadius()});
  ring1.setPosition({W * 0.5f, H * 0.46f});
  ring1.setFillColor(sf::Color::Transparent);
  ring1.setOutlineThickness(2.f);
  ring1.setOutlineColor(sf::Color(255, 115, 25, static_cast<std::uint8_t>(26 + pulso * 24)));
  tex.draw(ring1);

  sf::CircleShape ring2{H * 0.30f};
  ring2.setOrigin({ring2.getRadius(), ring2.getRadius()});
  ring2.setPosition({W * 0.5f, H * 0.46f});
  ring2.setFillColor(sf::Color::Transparent);
  ring2.setOutlineThickness(1.5f);
  ring2.setOutlineColor(sf::Color(0, 220, 255, static_cast<std::uint8_t>(24 + (1.f - pulso) * 24)));
  tex.draw(ring2);

  // Franja superior de lore
  sf::Text txtLore{font, "BASE ESTELAR 312 // TRITON, NEPTUNO  |  ANO 2527  |  DEMO PISO 01", 13};
  txtLore.setFillColor(sf::Color(135, 155, 185, 210));
  auto bLore = txtLore.getLocalBounds();
  txtLore.setPosition({(W - bLore.size.x) * 0.5f, H * 0.06f});
  tex.draw(txtLore);

  // Título principal EXPENDABLE
  sf::Text txtTitulo{font, "EXPENDABLE", 66};
  txtTitulo.setStyle(sf::Text::Style::Bold);
  txtTitulo.setFillColor(sf::Color(242, 246, 255, 255));
  auto bTit = txtTitulo.getLocalBounds();
  float titX = (W - bTit.size.x) * 0.5f;
  float titY = H * 0.11f;

  sf::Text txtShadow = txtTitulo;
  txtShadow.setFillColor(sf::Color(255, 115, 20, 85));
  txtShadow.setPosition({titX + 3.f, titY + 3.f});
  tex.draw(txtShadow);

  txtTitulo.setPosition({titX, titY});
  tex.draw(txtTitulo);

  // Línea separadora neón dual
  float barW = std::min(W * 0.62f, 540.f);
  sf::RectangleShape sepCyan{{barW * 0.5f, 2.5f}};
  sepCyan.setPosition({(W - barW) * 0.5f, titY + 84.f});
  sepCyan.setFillColor(sf::Color(0, 225, 255, 225));
  tex.draw(sepCyan);

  sf::RectangleShape sepOrange{{barW * 0.5f, 2.5f}};
  sepOrange.setPosition({W * 0.5f, titY + 84.f});
  sepOrange.setFillColor(sf::Color(255, 120, 25, 225));
  tex.draw(sepOrange);

  sf::Text txtSub{font, "ROGUELIKE  //  BULLET HELL  //  BOSS RUSH", 16};
  txtSub.setStyle(sf::Text::Style::Bold);
  txtSub.setFillColor(sf::Color(185, 200, 225, 225));
  auto bSub = txtSub.getLocalBounds();
  txtSub.setPosition({(W - bSub.size.x) * 0.5f, titY + 96.f});
  tex.draw(txtSub);

  // Panel central limpio (solo INICIAR PRUEBA y CERRAR)
  float panelW = std::min(W * 0.68f, 520.f);
  float panelH = 185.f;
  float panelX = (W - panelW) * 0.5f;
  float panelY = H * 0.37f;

  sf::RectangleShape panel{{panelW, panelH}};
  panel.setPosition({panelX, panelY});
  panel.setFillColor(sf::Color(16, 20, 30, 225));
  panel.setOutlineThickness(2.f);
  panel.setOutlineColor(sf::Color(55, 72, 102, 220));
  tex.draw(panel);

  float marker_off = std::sin(tiempo * 7.f) * 5.f;

  // Opción 0: INICIAR PRUEBA
  sf::Text txtIniciar{font, "INICIAR PRUEBA", 44};
  txtIniciar.setStyle(sf::Text::Style::Bold);
  txtIniciar.setFillColor(opcion == 0 ? sf::Color(0, 238, 255, 255)
                                      : sf::Color(150, 162, 185, 195));
  auto bIni = txtIniciar.getLocalBounds();
  float iniX = (W - bIni.size.x) * 0.5f;
  float iniY = panelY + 34.f;
  txtIniciar.setPosition({iniX, iniY});
  tex.draw(txtIniciar);

  // Opción 1: CERRAR
  sf::Text txtCerrar{font, "CERRAR", 30};
  txtCerrar.setStyle(sf::Text::Style::Bold);
  txtCerrar.setFillColor(opcion == 1 ? sf::Color(255, 95, 95, 255)
                                     : sf::Color(145, 152, 172, 185));
  auto bCer = txtCerrar.getLocalBounds();
  float cerX = (W - bCer.size.x) * 0.5f;
  float cerY = panelY + 114.f;
  txtCerrar.setPosition({cerX, cerY});
  tex.draw(txtCerrar);

  // Cursor selector ">"
  sf::Text txtCursor{font, ">", opcion == 0 ? 40u : 30u};
  txtCursor.setStyle(sf::Text::Style::Bold);
  txtCursor.setFillColor(sf::Color(255, 215, 65, 255));
  if (opcion == 0) {
    txtCursor.setPosition({iniX - 38.f + marker_off, iniY + 2.f});
  } else {
    txtCursor.setPosition({cerX - 32.f + marker_off, cerY});
  }
  tex.draw(txtCursor);

  // Leyenda Orbes y Controles
  float infoY = panelY + panelH + 34.f;

  sf::CircleShape dotVerde{6.f};
  dotVerde.setFillColor(sf::Color(45, 240, 105, 255));
  dotVerde.setPosition({panelX + 4.f, infoY + 4.f});
  tex.draw(dotVerde);

  sf::Text txtOrbeV{font, "ORBE VERDE: Mejora Permanente", 14};
  txtOrbeV.setFillColor(sf::Color(115, 250, 155, 235));
  txtOrbeV.setPosition({panelX + 22.f, infoY});
  tex.draw(txtOrbeV);

  sf::CircleShape dotMorado{6.f};
  dotMorado.setFillColor(sf::Color(200, 75, 255, 255));
  dotMorado.setPosition({panelX + panelW * 0.52f, infoY + 4.f});
  tex.draw(dotMorado);

  sf::Text txtOrbeM{font, "ORBE MORADO: Sobrecarga Temporal", 14};
  txtOrbeM.setFillColor(sf::Color(220, 130, 255, 235));
  txtOrbeM.setPosition({panelX + panelW * 0.52f + 18.f, infoY});
  tex.draw(txtOrbeM);

  sf::Text txtCtrl1{
      font,
      "[ WASD ] Mover   [ Click Izq ] Disparar   [ Espacio / Click Der ] Dash Inmune   [ R ] Recargar",
      14};
  txtCtrl1.setFillColor(sf::Color(170, 185, 210, 220));
  auto bC1 = txtCtrl1.getLocalBounds();
  txtCtrl1.setPosition({(W - bC1.size.x) * 0.5f, infoY + 34.f});
  tex.draw(txtCtrl1);

  sf::Text txtCtrl2{
      font,
      "[ W / S ] Seleccionar    [ Enter / Espacio ] Confirmar    [ Esc ] Salir",
      13};
  txtCtrl2.setFillColor(sf::Color(125, 140, 168, 195));
  auto bC2 = txtCtrl2.getLocalBounds();
  txtCtrl2.setPosition({(W - bC2.size.x) * 0.5f, infoY + 60.f});
  tex.draw(txtCtrl2);

  tex.setView(prevView);
}

} // namespace IVJ
