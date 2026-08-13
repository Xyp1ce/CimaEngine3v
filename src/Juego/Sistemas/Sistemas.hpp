#pragma once
#include "../../Motor/Componentes/IComponentes.hpp"
#include "../../Motor/Primitivos/CEPool.hpp"
#include "../../Motor/Primitivos/Objetos.hpp"
#include "../Componentes/IJComponentes.hpp"
#

namespace IVJ
{
[[maybe_unused]] void SistemaControl(CE::Objeto &ente, float dt);
[[maybe_unused]] void SistemaMover(const std::vector<std::shared_ptr<CE::Objeto>> &entes, float dt);
[[maybe_unused]] void SistemaMover(const std::shared_ptr<CE::Objeto> &objeto, float dt);
[[maybe_unused]] bool SistemaColAABB(CE::Objeto &A, CE::Objeto &B, bool resolucion = false);
[[maybe_unused]] bool SistemaColAABBMid(CE::Objeto &A, CE::Objeto &B, bool resolucion = false);

void SistemaMoverBalas(const std::shared_ptr<CE::Objeto> &ente, float dt);

[[maybe_unused]] void SistemaSpawn(CE::Pool &spwans);

[[maybe_unused]] void SistemaPaths(std::vector<std::shared_ptr<CE::Objeto>> &obj);

[[maybe_unused]] bool checkRayoHit(CE::Objeto &npc, CE::Vector2D &p1, CE::Vector2D &p2);

// void SistemaDialogos(CE::Objeto& npc);
[[maybe_unused]] void SistemaDialogos(IVJ::IDialogo *dialogo, CE::Objeto &obj);

bool revisarDistanciaInteraccion(CE::Objeto &jugador, CE::Objeto &npc, float distancia_maxima);

// DEBUG
[[maybe_unused]] void pintarLinea(CE::Vector2D &p1, CE::Vector2D &p2, const sf::Color &color = sf::Color::Red);


} // namespace IVJ
