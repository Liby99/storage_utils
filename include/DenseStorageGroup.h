#include "StorageGroup.h"
#include <unordered_set>

#ifndef DENSE_STORAGE_GROUP_H
#define DENSE_STORAGE_GROUP_H

template <typename... Types>
class DenseStorageGroup {
public:
  template <std::size_t Index>
  using TypeAt = typename extract_type_at<Index, Types...>::Type;

  using Bulk = std::tuple<Types...>;

  DenseStorageGroup() : storage_group(), max_size(0) {}

  std::optional<Bulk> get(std::size_t i) {
    if (this->is_valid(i)) {
      return this->storage_group.get_bulk(i);
    } else {
      return {};
    }
  }

  std::size_t insert(Types... args) {
    auto data = std::make_tuple(args...);
    return this->insert_bulk(data);
  }

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

  bool update(std::size_t i, Types... args) {
    auto data = std::make_tuple(args...);
    return this->update_bulk(i, data);
  }

  bool update_bulk(std::size_t i, Bulk data) {
    if (this->is_valid(i)) {
      this->storage_group.set_bulk(i, data);
      return false;
    }
    return true;
  }

  bool remove(std::size_t i) {
    if (this->is_valid(i)) {
      this->removed_indices.insert(i);
      return true;
    }
    return false;
  }

  template <std::size_t Index>
  std::optional<TypeAt<Index>> get_component(std::size_t i) {
    if (this->is_valid(i)) {
      using S = Storage<Index, TypeAt<Index>>;
      return (static_cast<S &>(this->storage_group)).get(i);
    } else {
      return {};
    }
  }

  template <std::size_t Index>
  bool update_component(std::size_t i, TypeAt<Index> elem) {
    if (this->is_valid(i)) {
      using S = Storage<Index, TypeAt<Index>>;
      (static_cast<S &>(this->storage_group)).set(i, elem);
      return true;
    }
    return false;
  }

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

  std::size_t size() { return this->max_size - this->removed_indices.size(); }

  std::size_t _max_size() { return this->max_size; }

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