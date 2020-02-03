#include "storage_utils/DenseStorageGroup.h"

using Store = DenseStorageGroup<float, float>;

int main() {
  Store store;

  // First insert 45 elements
  for (int i = 10; i < 100; i += 2) {
    store.insert(i, i, i);
  }

  // Check if it contains 45 elements
  assert(store.size() == 45);

  // Get each element to see if they are good
  for (int i = 10; i < 100; i += 2) {

    // Use bulk get
    auto [a, b] = store.get(i).value();
    assert(a == i);
    assert(b == i);

    // Use individual get
    auto c = store.get_component<0>(i).value();
    assert(c == i);

    auto d = store.get_component<1>(i).value();
    assert(d == i);
  }

  // Get the non existed elements and see if they are not presented
  // 0-9 are not presented
  for (int i = 0; i < 10; i += 1) {
    assert(!store.get(i).has_value());
  }
  // 11, 13, 15, ..., 99 are not presented
  for (int i = 11; i < 100; i += 2) {
    assert(!store.get(i).has_value());
  }

  // Make sure that we cannot update elements that are not existed
  for (int i = 0; i < 10; i += 1) {
    assert(!store.update(i, 0.0, 0.0));
  }

  // Nor can we update individual components
  for (int i = 11; i < 100; i += 2) {
    assert(!store.update_component<0>(i, 0.0));
    assert(!store.update_component<1>(i, 0.0));
  }

  // Make sure that the data is still integrate now
  for (int i = 10; i < 100; i += 2) {
    auto [a, b] = store.get(i).value();
    assert(a == i);
    assert(b == i);
  }

  // Modify the elements using insert
  for (int i = 10; i < 100; i += 2) {
    store.insert(i, i + 10, i + 10);
  }

  // Check if the values are updated
  for (int i = 10; i < 100; i += 2) {
    auto [a, b] = store.get(i).value();
    assert(a == i + 10);
    assert(b == i + 10);
  }

  // Modify the elements using update
  for (int i = 10; i < 100; i += 2) {
    assert(store.update(i, i + 20, i + 20));
  }

  // Check if the values are updated
  for (int i = 10; i < 100; i += 2) {
    auto [a, b] = store.get(i).value();
    assert(a == i + 20);
    assert(b == i + 20);
  }

  // Modify the elements using update_component
  for (int i = 10; i < 100; i += 2) {
    assert(store.update_component<1>(i, i + 100));
  }

  // Check if the values are updated
  for (int i = 10; i < 100; i += 2) {
    auto [a, b] = store.get(i).value();
    assert(a == i + 20);  // The first element remains the same
    assert(b == i + 100); // Only second element gets updated
  }

  // Check the size again before removing
  assert(store.size() == 45);

  // Check if we cannot remove elements that are not existed
  for (int i = 0; i < 10; i++) {
    assert(!store.remove(i));
  }

  // Check nothing gets removed
  assert(store.size() == 45);

  // Check if we can remove elements from 50 to 100 (50, 52, 54, ..., 98)
  for (int i = 50; i < 100; i += 2) {
    assert(store.remove(i));
  }

  // Check the size of the storage (things gets removed)
  // 10, 12, 14, 16, ..., 48. There should be 20 elements in total
  assert(store.size() == 20);

  // The removed items are not there now
  for (int i = 50; i < 100; i += 2) {
    assert(!store.get(i).has_value());
  }

  // We cannot remove them again
  for (int i = 50; i < 100; i += 2) {
    assert(!store.remove(i));
  }

  // We cannot update the removed items (of course)
  for (int i = 50; i < 100; i += 2) {
    assert(!store.update(i, 0.0, 0.0));
  }

  // We can still update the existed items
  for (int i = 10; i < 50; i += 2) {
    assert(store.update(i, i, i));
  }

  // Check if elements get updated
  for (int i = 10; i < 50; i += 2) {
    auto [a, b] = store.get(i).value();
    assert(a == i);
    assert(b == i);
  }

  // Insert new items
  for (int i = 51; i < 100; i += 2) {
    store.insert(i, i, i);
  }

  // Check the size of store
  assert(store.size() == 45);
}