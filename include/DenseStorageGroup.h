#include "StorageGroup.h"
#include <unordered_set>

#ifndef DENSE_STORAGE_GROUP_H
#define DENSE_STORAGE_GROUP_H

template <typename... Types>
class DenseStorageGroup {
public:
  // The helper type `TypeAt<Index>`
  template <std::size_t Index>
  using TypeAt = typename extract_type_at<Index, Types...>::Type;

  // The helper type `Bulk` for the tuple of all types
  using Bulk = std::tuple<Types...>;

  /**
   * Default constructor
   */
  DenseStorageGroup() : storage_group(), max_size(0) {}

  /**
   * Get an optional Bulk of data from the storage at index `i`;
   * If the given index `i` is invalid, then return `None`
   */
  std::optional<Bulk> get(std::size_t i) {
    if (this->is_valid(i)) {
      return this->storage_group.get_bulk(i);
    } else {
      return {};
    }
  }

  /**
   * Insert all the data (as function arguments) to the storage group.
   * Will return the index where the item get insert to.
   */
  std::size_t insert(Types... args) {
    auto data = std::make_tuple(args...);
    return this->insert_bulk(data);
  }

  /**
   * Insert the data wrapped in a `Bulk` to the storage group.
   * Will fill in the empty slots within the storage when available.
   * Will return the index where the item get insert to.
   */
  std::size_t insert_bulk(Bulk data) {
    std::size_t index;
    if (this->removed_indices.empty()) {
      index = this->max_size++;
      this->storage_group.push_bulk(data);
    } else {
      auto first_index_it = this->removed_indices.begin();
      index = *first_index_it;
      this->removed_indices.erase(first_index_it);
      this->storage_group.set_bulk(index, data);
    }
    return index;
  }

  /**
   * Update all the data (provided as function arguments) at position `i`
   * Will return `true` if update is successful (index is valid)
   * Will return `false` when index is not valid
   */
  bool update(std::size_t i, Types... args) {
    auto data = std::make_tuple(args...);
    return this->update_bulk(i, data);
  }

  /**
   * Update the data at index `i` using a `Bulk`.
   * Will return `true` if update is successful (index is valid)
   * Will return `false` when index is not valid
   */
  bool update_bulk(std::size_t i, Bulk data) {
    if (this->is_valid(i)) {
      this->storage_group.set_bulk(i, data);
      return false;
    }
    return true;
  }

  /**
   * Remove all the data at index `i`. Return `true` when `i` is valid
   * and the removal is successful. Return `false` if not.
   */
  bool remove(std::size_t i) {
    if (this->is_valid(i)) {
      this->removed_indices.insert(i);
      return true;
    }
    return false;
  }

  /**
   * Get an optional specific component at index `i`.
   * If there's no such element at index `i`, then return `None`.
   *
   * Sample usage:
   *
   * ``` c++
   * DenseStorageGroup<float, int> storage;
   * storage.get_component<1>(10); // is an integer
   * ```
   */
  template <std::size_t Index>
  std::optional<TypeAt<Index>> get_component(std::size_t i) {
    if (this->is_valid(i)) {
      using S = Storage<Index, TypeAt<Index>>;
      return (static_cast<S &>(this->storage_group)).get(i);
    } else {
      return {};
    }
  }

  /**
   * Update the component at index `i` with the given data.
   * If the index is valid, return `true`. Otherwise return `false`.
   *
   * Sample usage:
   *
   * ``` c++
   * DenseStorageGroup<float, int> storage;
   * storage.insert(3.14, 10); // `10` is stored at `0`
   * storage.update_component<1>(0, 50); // `50` is stored at `0`
   * ```
   */
  template <std::size_t Index>
  bool update_component(std::size_t i, TypeAt<Index> elem) {
    if (this->is_valid(i)) {
      using S = Storage<Index, TypeAt<Index>>;
      (static_cast<S &>(this->storage_group)).set(i, elem);
      return true;
    }
    return false;
  }

  /**
   * Extract all the valid data (as a `std::vector`)of a given component.
   *
   * Sample Usage:
   *
   * ``` c++
   * DenseStorageGroup<float, int> storage;
   * std::vector<int> int_store = storage.extract<1>();
   * ```
   */
  template <std::size_t Index>
  std::vector<TypeAt<Index>> extract() {
    std::vector<TypeAt<Index>> result;
    auto removed_index_end = this->removed_indices.end();
    for (std::size_t i = 0; i < this->max_size; i++) {
      if (this->removed_indices.find(i) == removed_index_end) {
        result.push_back(this->get_component<Index>(i).value());
      }
    }
    return result;
  }

  /**
   * Get the size of this storage. Only valid elements will be considered.
   */
  std::size_t size() { return this->max_size - this->removed_indices.size(); }

  /**
   * [Experimental] Get the maximum size of this storage
   */
  std::size_t _max_size() { return this->max_size; }

  /**
   * Check if the storage is empty (basically if size is 0)
   */
  bool is_empty() { return this->size() == 0; }

private:
  std::size_t max_size;
  std::unordered_set<std::size_t> removed_indices;
  StorageGroup<Types...> storage_group;

  bool is_valid(std::size_t i) {
    return i < this->max_size && !this->is_removed(i);
  }

  bool is_removed(std::size_t i) {
    auto it = this->removed_indices.find(i);
    return it != this->removed_indices.end();
  }
};

#endif