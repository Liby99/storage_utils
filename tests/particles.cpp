#include "../include/Prelude.h"
#include <assert.h>

typedef Tuple<float, float, float> Vector3f;

typedef Tuple<float, Vector3f, Vector3f> Particle;

// Mass, Position, Velocity
typedef DenseVecStorage<float, Vector3f, Vector3f> Particles;

int main() {
  Particles particles;

  // Insert a particle
  auto idx =
      particles.insert(1.0, Tuple(1.0f, 2.0f, 3.0f), Tuple(4.0f, 5.0f, 6.0f));

  // Assert the data inside that particle
  auto mass = particles.get_component<0>(idx);
  auto position = particles.get_component<1>(idx);
  auto velocity = particles.get_component<2>(idx);
  assert(mass == 1.0);
  assert(position.get<1>() == 2.0);
  assert(velocity.get<2>() == 6.0);

  // Check if we can remove a particle from random index (NO)
  assert(!particles.remove(100));

  // Check if the particle array is empty (IT IS NOT)
  assert(!particles.is_empty());

  // Try to update a component
  particles.update_component<0>(idx, 3.0);
  auto the_mass_now = particles.get_component<0>(idx);
  assert(the_mass_now == 3.0);

  // Try to update the whole particle
  particles.update(idx, 2.0, Tuple(100.0f, 200.0f, 300.0f),
                   Tuple(4.0f, 5.0f, 6.0f));
  auto the_mass_now_again = particles.get_component<0>(idx);
  assert(the_mass_now_again == 2.0);

  // CHeck if we can remove the particle we just inserted
  assert(particles.remove(idx));

  // Check if the particle storage is empty (IT IS)
  assert(particles.is_empty());
}