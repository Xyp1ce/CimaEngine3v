#pragma once
#include "../GUI/GLayer.hpp"
#include "../Primitivos/Objetos.hpp"
#include "../Utils/Utils.hpp"
#include <vector>

namespace CE {
class GHistograma : public GLayer {
public:
  struct Histograma {
    std::vector<float> bins; // conteo por bin
    float minVal = 0.0f;
    float maxVal = 1.0f;
    float binWidth = 0.0f;
    float peak = 0.0f;
  };

public:
  ~GHistograma() override {};

  void OnInit(const MotorConfig &des) override;
  void OnUpdate(float dt) override;
  void OnRender(void) override;

private:
  GHistograma::Histograma
  crearHistograma(const std::vector<std::shared_ptr<CE::Objeto>> &objetos,
                  int bins);
  GHistograma::Histograma histograma;
  size_t total{0};
  size_t getEntesSim(const std::vector<std::shared_ptr<CE::Objeto>> &objetos);
};
} // namespace CE
