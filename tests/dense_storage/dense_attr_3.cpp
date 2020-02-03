#include "storage_utils/Prelude.h"

using Vector2f = std::tuple<float, float>;

using Particles = VecStorageGroup<Vector2f, Vector2f>;

using Accelerations = DenseStorageGroup<Vector2f>;

int main() {
  Accelerations accelerations;

  for (int i = 10; i < 20; i++) {
    accelerations.insert(i, Vector2f(i, i + 1));
  }

  assert(accelerations.size() == 10);

  // Counter
  int counter = 0;

  for (auto [index, acc] : accelerations) {
    assert(std::get<0>(acc) == index);
    assert(std::get<1>(acc) == index + 1);
    counter += 1;
  }

  assert(counter == 10);
}