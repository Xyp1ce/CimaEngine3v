#pragma once
#include "FSMBoss.hpp"
#include "Motor/Utils/Vector2D.hpp"
namespace IVJ {
class AttackBoss : public FSMBoss {
public:
  explicit AttackBoss(int max_frames, float frame_rate);
  FSM *onInputs(Entidad &parent, CE::Vector2D &target) override;
  void onEntrar(const Entidad &obj) override;
  void onSalir(const Entidad &obj) override;
  void onUpdate(const Entidad &obj, float dt) override;

private:
  int max_frames;
  int id_frame;
  float max_frame_time;
  float cur_frame_time;
  int t_w;
  int t_h;
};
} // namespace IVJ
