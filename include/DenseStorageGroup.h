#include "StorageGroup.h"
#include <unordered_set>

#ifndef DENSE_STORAGE_GROUP_H
#define DENSE_STORAGE_GROUP_H

template <typename... Types>
class DenseStorageGroup {
public:
  DenseStorageGroup() : storage_group(), max_size(0) {}

  std::tuple<Types...> get(std::size_t i) { return this->storage_group.get_bulk(i); }

  std::size_t insert(Types... args) {
    auto data = std::make_tuple(args...);
    return this->insert_bulk(data);
  }

  std::size_t insert_bulk(std::tuple<Types...> data) {
    std::size_t result;
    if (this->removed_indices.empty()) {
      result = this->max_size;
      this->storage_group.push_bulk(data);
      this->max_size += 1;
    } else {
      result = this->removed_indices.extract(0).value();
      this->storage_group.set_bulk(result, data);
    }
    return result;
  }

  bool update(std::size_t i, Types... args) {
    auto data = std::make_tuple(args...);
    return this->update_bulk(i, data);
  }

  bool update_bulk(std::size_t i, std::tuple<Types...> data) {
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
  auto &get_component(std::size_t i) {
    auto &cs = this->get_component_storage<Index>();
    return cs[i];
  }

  template <std::size_t Index, typename T>
  bool update_component(std::size_t i, T elem) {
    if (this->is_valid(i)) {
      auto &cs = this->get_component_storage<Index>();
      cs[i] = elem;
      return true;
    }
    return false;
  }

  std::size_t size() { return this->max_size - this->removed_indices.size(); }

  bool is_empty() { return this->size() == 0; }

private:
  std::size_t max_size;
  std::unordered_set<std::size_t> removed_indices;
  StorageGroup<0, Types...> storage_group;

  template <std::size_t Index>
  auto &get_component_storage() {
    typedef typename extract_type_at<Index, Types...>::Type Type;
    return (static_cast<Storage<Index, Type> &>(this->storage_group)).get_data();
  }

  bool is_valid(std::size_t i) {
    return i < this->max_size && !this->is_removed(i);
  }

  bool is_removed(std::size_t i) {
    auto it = this->removed_indices.find(i);
    return it != this->removed_indices.end();
  }
};

#endif