#include <storage_utils/Prelude.h>

using Vector2f = std::tuple<float, float>;

using Matrix2f = std::tuple<float, float, float, float>;

// mass (m), position (x), velocity (v)
using Particles = VecStorageGroup<float, Vector2f, Vector2f>;

// theta_c (tc), theta_s (ts), deformation_gradient (F)
using Deformations = DenseStorageGroup<float, float, Matrix2f>;

// hardening (h)
using Hardenings = DenseStorageGroup<float>;

int main() {
  Particles particles;
  Deformations deformations;
  Hardenings hardenings;

  for (int i = 0; i < 100; i++) {
    auto id = particles.insert(1.0, Vector2f(1.0, 1.0), Vector2f(1.0, 1.0));
    if (i < 25) {
      hardenings.insert(id, 1.0);
    }
    if (i < 50) {
      deformations.insert(id, 0.0, 1.0, Matrix2f(1.0, 0.0, 0.0, 1.0));
    }
  }

  for (auto [id, m, x, v, tc, ts, F] : particles.join(deformations)) {
    // Do things with particle & deformations
  }

  for (auto [id, m, x, v, h] : particles.join(hardenings)) {
    // Do things with particle & hardenings
  }

  for (auto [id, m, x, v, tc, ts, F, h] :
       particles.join(deformations, hardenings)) {
    // Do things with particle & deformation & hardenings
  }
}