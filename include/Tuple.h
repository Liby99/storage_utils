#include <cstddef>
#include <utility>

#ifndef TUPLE_H
#define TUPLE_H

template <std::size_t Index, typename T>
class _tuple_impl {
public:
  _tuple_impl(T const &v) { val = v; }

  _tuple_impl(T &&v) { val = std::move(v); }

  T &get() { return val; }

private:
  T val;
};

template <std::size_t Index, typename... Types>
class _tuple_recurr_base {};

template <std::size_t Index, typename T, typename... Types>
class _tuple_recurr_base<Index, T, Types...>
    : public _tuple_impl<Index, T>,
      public _tuple_recurr_base<Index + 1, Types...> {
public:
  static constexpr std::size_t SIZE = sizeof...(Types) + 1;

  _tuple_recurr_base(T &&arg, Types &&... args)
      : _tuple_impl<Index, T>(std::forward<T>(arg)),
        _tuple_recurr_base<Index + 1, Types...>(std::forward<Types>(args)...) {}

  _tuple_impl<Index, T> head() {
    return (static_cast<_tuple_impl<Index, T> &>(*this));
  }

  _tuple_recurr_base<Index + 1, Types...> rest() {
    return (static_cast<_tuple_recurr_base<Index + 1, Types...> &>(*this));
  }
};

template <std::size_t Index, typename T, typename... Args>
struct extract_type_at {
  using Type = typename extract_type_at<Index - 1, Args...>::Type;
};

template <typename T, typename... Args>
struct extract_type_at<0, T, Args...> {
  using Type = T;
};

template <typename... Types>
class Tuple {
public:
  Tuple(Types &&... args) : _tuple(std::forward<Types>(args)...) {}

  template <std::size_t Index>
  auto &get() {
    typedef typename extract_type_at<Index, Types...>::Type TypeAtIndex;
    typedef _tuple_impl<Index, TypeAtIndex> ext;
    return (static_cast<ext &>(this->_tuple)).get();
  }

  _tuple_recurr_base<0, Types...> _tuple;
};

// template <typename... Types>
// Tuple(Types... args)->Tuple<Types...>;

template <typename... Types>
struct std::tuple_size<Tuple<Types...>>
    : std::integral_constant<std::size_t, sizeof...(Types)> {};

template <std::size_t Index, typename... Types>
struct std::tuple_element<Index, Tuple<Types...>> {
  using type = typename extract_type_at<Index, Types...>::Type;
};

#endif