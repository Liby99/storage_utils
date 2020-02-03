#include "storage_utils/Prelude.h"

using Vector2f = std::tuple<float, float>;

using Particles = VecStorageGroup<Vector2f, Vector2f>;

using Accelerations = DenseStorageGroup<Vector2f>;

int main() {
  Particles particles;
  Accelerations accelerations;

  assert(accelerations.size() == 0);

  // Add 10 pure particles
  for (int i = 0; i < 10; i++) {
    particles.insert(Vector2f(i, i), Vector2f(1.0, -1.0));
  }

  // Add another 10 particles with acceleration stored in accelerations
  for (int i = 10; i < 20; i++) {
    auto p = particles.insert(Vector2f(i, i), Vector2f(1.0, -1.0));
    accelerations.insert(p, Vector2f(-1.0, -1.0));
  }

  // Add another 10 particles
  for (int i = 20; i < 30; i++) {
    particles.insert(Vector2f(i, i), Vector2f(1.0, -1.0));
  }

  assert(accelerations.size() == 10);

  // Count the amount of accelerations being removed, should be 10
  int counter_1 = 0;
  for (auto [index, _pos, _vel] : particles) {
    if (accelerations.remove(index)) {
      counter_1 += 1;
    }
  }
  printf("%d\n", counter_1);
  assert(counter_1 == 10);
}