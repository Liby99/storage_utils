#include <cstddef>
#include <utility>

template <std::size_t _index, typename T>
class _tuple_impl {
public:
  _tuple_impl(T const &v) { val = v; }
  _tuple_impl(T &&v) { val = std::move(v); }
  T &get() { return val; }

private:
  T val;
};

template <std::size_t _index, typename... types>
class _tuple_recurr_base {};

template <std::size_t _index, typename L, typename... types>
class _tuple_recurr_base<_index, L, types...>
    : public _tuple_impl<_index, L>,
      public _tuple_recurr_base<_index + 1, types...> {
public:
  _tuple_recurr_base(L &&arg, types &&... args)
      : _tuple_impl<_index, L>(std::forward<L>(arg)),
        _tuple_recurr_base<_index + 1, types...>(std::forward<types>(args)...) {
  }
};

template <std::size_t index, typename L, typename... Args>
struct extract_type_at {
  using type = typename extract_type_at<index - 1, Args...>::type;
};

template <typename L, typename... Args>
struct extract_type_at<0, L, Args...> {
  using type = L;
};

template <typename L, typename... types>
class Tuple {
public:
  Tuple(L &&arg, types &&... args) : _tuple(std::forward<L>(arg), std::forward<types>(args)...) {}

  template <std::size_t index>
  auto &get() {
    return (static_cast<_tuple_impl<index, typename extract_type_at<index, L, types...>::type> &>(this->_tuple)).get();
  }

private:
  _tuple_recurr_base<0, L, types...> _tuple;
};
