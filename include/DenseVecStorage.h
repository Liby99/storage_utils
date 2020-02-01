#include <cstddef>
#include <utility>
#include <vector>
#include <unordered_set>

#ifndef DENSE_VEC_STORAGE_H
#define DENSE_VEC_STORAGE_H

template <std::size_t _index, typename T>
class _dense_vec_storage_impl {
public:
  _dense_vec_storage_impl() {}

  std::vector<T> &get_data() {
    return data;
  }

  void set(std::size_t i, T elem) {
    this->data[i] = elem;
  }

  void push(T elem) {
    this->data.push_back(elem);
  }

private:
  std::vector<T> data;
};

template <std::size_t _index, typename... types>
class _dense_vec_storage_recurr_base {
public:
  void set_bulk(std::size_t i, types ...args) {}
  void push_bulk(types ...args) {}
  void update_bulk(types ...args) {}
};

template <std::size_t _index, typename L, typename... types>
class _dense_vec_storage_recurr_base<_index, L, types...>
    : public _dense_vec_storage_impl<_index, L>,
      public _dense_vec_storage_recurr_base<_index + 1, types...> {
public:
  static constexpr std::size_t SIZE = sizeof...(types) + 1;

  _dense_vec_storage_recurr_base()
      : _dense_vec_storage_impl<_index, L>(),
        _dense_vec_storage_recurr_base<_index + 1, types...>() {
  }

  void set_bulk(std::size_t i, L elem, types ...args) {
    _dense_vec_storage_impl<_index, L>::set(i, elem);
    _dense_vec_storage_recurr_base<_index + 1, types...>::set_bulk(i, args...);
  }

  void push_bulk(L elem, types ...args) {
    _dense_vec_storage_impl<_index, L>::push(elem);
    _dense_vec_storage_recurr_base<_index + 1, types...>::push_bulk(args...);
  }
};

template <typename... types>
class DenseVecStorage {
public:
  DenseVecStorage() : storage(), max_size(0) {}

  std::size_t insert(types ...args) {
    if (this->removed_indices.empty()) {
      this->storage.push_bulk(args...);
      this->max_size += 1;
      return this->max_size - 1;
    } else {
      std::size_t empty_index = this->removed_indices.extract(0).value();
      this->storage.set_bulk(empty_index, args...);
      return empty_index;
    }
  }

  bool update(std::size_t i, types ...args) {
    if (this->is_valid(i)) {
      this->storage.set_bulk(i, args...);
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

  template <std::size_t index>
  auto &get_component(std::size_t i) {
    auto &cs = this->get_component_storage<index>();
    return cs[i];
  }

  template <std::size_t index, typename T>
  bool update_component(std::size_t i, T elem) {
    if (this->is_valid(i)) {
      auto &cs = this->get_component_storage<index>();
      cs[i] = elem;
      return true;
    }
    return false;
  }

  std::size_t size() {
    return this->max_size - this->removed_indices.size();
  }

  bool is_empty() {
    return this->size() == 0;
  }

private:
  std::size_t max_size;
  std::unordered_set<std::size_t> removed_indices;
  _dense_vec_storage_recurr_base<0, types...> storage;

  template <std::size_t index>
  auto &get_component_storage() {
    typedef _dense_vec_storage_impl<index, typename extract_type_at<index, types...>::type> ext;
    return (static_cast<ext &>(this->storage)).get_data();
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