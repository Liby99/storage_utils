#include <cstddef>
#include <unordered_set>
#include <utility>
#include <vector>

#ifndef DENSE_VEC_STORAGE_H
#define DENSE_VEC_STORAGE_H

template <std::size_t Index, typename T>
class _dense_vec_storage_impl {
public:
  _dense_vec_storage_impl() {}

  std::vector<T> &get_data() { return data; }

  void set(std::size_t i, T elem) { this->data[i] = elem; }

  void push(T elem) { this->data.push_back(elem); }

private:
  std::vector<T> data;
};

template <std::size_t Index, typename... Types>
class _dense_vec_storage_recurr_base {
public:
  void set_bulk(std::size_t i, Types... args) {}
  void push_bulk(Types... args) {}
  void update_bulk(Types... args) {}
};

template <std::size_t Index, typename T, typename... Types>
class _dense_vec_storage_recurr_base<Index, T, Types...>
    : public _dense_vec_storage_impl<Index, T>,
      public _dense_vec_storage_recurr_base<Index + 1, Types...> {
public:
  static constexpr std::size_t SIZE = sizeof...(Types) + 1;

  _dense_vec_storage_recurr_base()
      : _dense_vec_storage_impl<Index, T>(),
        _dense_vec_storage_recurr_base<Index + 1, Types...>() {}

  void set_bulk(std::size_t i, T elem, Types... args) {
    _dense_vec_storage_impl<Index, T>::set(i, elem);
    _dense_vec_storage_recurr_base<Index + 1, Types...>::set_bulk(i, args...);
  }

  void push_bulk(T elem, Types... args) {
    _dense_vec_storage_impl<Index, T>::push(elem);
    _dense_vec_storage_recurr_base<Index + 1, Types...>::push_bulk(args...);
  }
};

template <typename... Types>
class DenseVecStorage {
public:
  DenseVecStorage() : storage(), max_size(0) {}

  std::size_t insert(Types... args) {
    std::size_t result;
    if (this->removed_indices.empty()) {
      result = this->max_size;
      this->storage.push_bulk(args...);
      this->max_size += 1;
    } else {
      result = this->removed_indices.extract(0).value();
      this->storage.set_bulk(result, args...);
    }
    return result;
  }

  bool update(std::size_t i, Types... args) {
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
  _dense_vec_storage_recurr_base<0, Types...> storage;

  template <std::size_t Index>
  auto &get_component_storage() {
    typedef _dense_vec_storage_impl<
        Index, typename extract_type_at<Index, Types...>::Type>
        ext;
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