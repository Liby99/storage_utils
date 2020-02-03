#include <storage_utils/Prelude.h>

using Vector2f = std::tuple<float, float>;

using Matrix2f = std::tuple<float, float, float, float>;

using Particles = VecStorageGroup<float, Vector2f, Vector2f>;

using Deformations = DenseStorageGroup<float, float, Matrix2f>;

using Hardenings = DenseStorageGroup<float>;

using JoinedParDefHar = JoinedStorageGroup<Particles, Deformations, Hardenings>;

using JoinedParDef = JoinedStorageGroup<Particles, Deformations>;

using JoinedParHar = JoinedStorageGroup<Particles, Hardenings>;

int main() {
  Particles particles;
  Deformations deformations;
  Hardenings hardenings;

  for (int i = 0; i < 100; i++) {
    auto id = particles.insert(1.0, Vector2f(1.0, 1.0), Vector2f(1.0, 1.0));
    if (i < 25) { hardenings.insert(id, 1.0); }
    if (i < 50) { deformations.insert(id, 0.0, 1.0, Matrix2f(1.0, 0.0, 0.0, 1.0)); }
  }

  JoinedParDefHar joined_1(particles, deformations, hardenings);

  for (int i = 0; i < 100; i++) {
    if (i < 25) {
      assert(joined_1.contains(i));
    } else {
      assert(!joined_1.contains(i));
    }
  }

  JoinedParDef joined_2(particles, deformations);

  for (int i = 0; i < 100; i++) {
    if (i < 50) {
      assert(joined_2.contains(i));
    } else {
      assert(!joined_2.contains(i));
    }
  }
}