#include "../include/Prelude.h"
#include <assert.h>

typedef std::tuple<float, float, float> Vector3f;

typedef std::tuple<float, Vector3f, Vector3f> Particle;

// Mass, Position, Velocity
typedef DenseVecStorage<float, Vector3f, Vector3f> Particles;

int main() {
  Particles particles;

  // Insert a particle
  auto init_pos = std::make_tuple(1.0f, 2.0f, 3.0f);
  auto init_vel = std::make_tuple(4.0f, 5.0f, 6.0f);
  auto idx = particles.insert(1.0, init_pos, init_vel);

  // Assert the data inside that particle
  auto [mass, position, velocity] = particles.get(idx);
  assert(mass == 1.0);
  assert(std::get<1>(position) == 2.0);
  assert(std::get<2>(velocity) == 6.0);

  // Check if we can remove a particle from random index (NO)
  assert(!particles.remove(100));

  // Check if the particle array is empty (IT IS NOT)
  assert(!particles.is_empty());

  // Try to update a component
  particles.update_component<0>(idx, 3.0);
  auto the_mass_now = particles.get_component<0>(idx);
  assert(the_mass_now == 3.0);

  // Try to update the whole particle
  auto new_pos = std::make_tuple(100.0f, 200.0f, 300.0f);
  particles.update(idx, 2.0, new_pos, init_vel);
  auto the_mass_now_again = particles.get_component<0>(idx);
  assert(the_mass_now_again == 2.0);

  // CHeck if we can remove the particle we just inserted
  assert(particles.remove(idx));

  // Check if the particle storage is empty (IT IS)
  assert(particles.is_empty());
}