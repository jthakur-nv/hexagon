#ifndef JKT_CORE_HXX
#define JKT_CORE_HXX

#include "common.hxx"

namespace JKT {
namespace core {

using namespace JKT::common;

template <class T>
struct compile_time_type {
  using type = T;
};

template <bool V>
struct compile_time_value {
  static const bool value = V;
};

using true_value = compile_time_value<true>;
using false_value = compile_time_value<false>;

template <bool, class T = void>
struct enable_if {};

template <class T>
struct enable_if<true, T> {
  using type = T;
};

template <bool, class T, class F>
struct conditional {
  using type = T;
};

template <class T, class F>
struct conditional<false, T, F> {
  using type = F;
};

template <class A, class B>
struct is_same : false_value {};

template <class A>
struct is_same<A, A> : true_value {};

// implement is_any_of using is_same and logical or
template <class T, class... Candidates>
struct is_any_of : false_value {};

template <class T, class First, class... Rest>
struct is_any_of<T, First, Rest...> : conditional<is_same<T, First>::value, true_value, is_any_of<T, Rest...>>::type {};

template <bool Boolean, class T>
using enable_if_t = typename enable_if<Boolean, T>::type;

template <class T>
struct is_signed : conditional<is_any_of<T, i8, i16, i32, i64>::value, true_value, false_value>::type {};

template <class T>
struct is_unsigned : conditional<is_any_of<T, u8, u16, u32, u64>::value, true_value, false_value>::type {};

template <class T>
struct is_float : conditional<is_any_of<T, f32, f64>::value, true_value, false_value>::type {};

template <class T>
struct is_integer : conditional<is_any_of<T, i8, i16, i32, i64, u8, u16, u32, u64>::value, true_value, false_value>::type {};

template <class T>
struct to_unsigned {
  using type = T;
};

template <> struct to_unsigned<i8>  : compile_time_type<u8>  {};
template <> struct to_unsigned<i16> : compile_time_type<u16> {};
template <> struct to_unsigned<i32> : compile_time_type<u32> {};
template <> struct to_unsigned<i64> : compile_time_type<u64> {};


template <usize bits>
struct big_int {
  using small_int = big_int<bits / 2>;
  // big_int = limbs[1] << (bits / 2) + limbs[0]
  small_int limbs[2];
  bool is_overflow;

  big_int(u64 value = 0) : limbs{value, 0}, is_overflow(false) {}

  big_int &operator+=(const big_int& other) {
    limbs[0] += other.limbs[0];
    limbs[1] += other.limbs[1] + limbs[0].is_overflow;
    is_overflow = limbs[1].is_overflow;
    return *this;
  }

  big_int &operator-=(const big_int& other) {
    limbs[0] -= other.limbs[0];
    limbs[1] -= other.limbs[1] + limbs[0].is_overflow;
    is_overflow = limbs[1].is_overflow;
    return *this;
  }
};

template <>
struct big_int<64> {
  u64 value;
  bool is_overflow;

  big_int(u64 value = 0) : value(value), is_overflow(false) {}
  big_int(const big_int& other) = default;
  big_int &operator=(const big_int& other) { value = other.value; is_overflow = other.is_overflow; return *this; }

  friend big_int operator+(big_int a, const big_int& b) { a += b; return a; }
  friend big_int operator-(big_int a, const big_int& b) { a -= b; return a; }
  friend big_int operator*(big_int a, const big_int& b) { a *= b; return a; }
  friend big_int operator/(big_int a, const big_int& b) { a /= b; return a; }
  friend big_int operator%(big_int a, const big_int& b) { a %= b; return a; }

  big_int &operator+=(const big_int& other) {
    u64 previous = value;
    value += other.value;
    is_overflow = (value < previous);
    return *this;
  }

  big_int &operator-=(const big_int& other) {
    is_overflow = (value < other.value);
    value -= other.value;
    return *this;
  }

  big_int &operator*=(const big_int& other) {
    is_overflow = ((other.value != 0) && (value > ((~u64(0)) / other.value)));
    value *= other.value;
    return *this;
  }

  // Precondition: other.value != 0.
  big_int &operator/=(const big_int& other) {
    value /= other.value;
    is_overflow = false;
    return *this;
  }

  // Precondition: other.value != 0.
  big_int &operator%=(const big_int& other) {
    value %= other.value;
    is_overflow = false;
    return *this;
  }

  friend bool operator==(const big_int& a, const big_int& b) { return a.value == b.value; }
  friend bool operator!=(const big_int& a, const big_int& b) { return a.value != b.value; }
  friend bool operator<=(const big_int& a, const big_int& b) { return a.value <= b.value; }
  friend bool operator>=(const big_int& a, const big_int& b) { return a.value >= b.value; }
  friend bool operator<(const big_int& a, const big_int& b) { return a.value < b.value; }
  friend bool operator>(const big_int& a, const big_int& b) { return a.value > b.value; }

  friend big_int operator<<(big_int a, usize shift) { a <<= shift; return a; }
  friend big_int operator>>(big_int a, usize shift) { a >>= shift; return a; }

  big_int &operator<<=(usize shift) {
    is_overflow = shift >= 64
      ? value != 0
      : shift != 0 && (value >> (64 - shift)) != 0;

    value = shift >= 64 ? u64(0) : value << shift;
    return *this;
  }

  big_int &operator>>=(usize shift) {
    value = shift >= 64 ? u64(0) : value >> shift;
    is_overflow = false;
    return *this;
  }

  friend big_int operator&(big_int a, const big_int& b) { a &= b; return a; }
  friend big_int operator|(big_int a, const big_int& b) { a |= b; return a; }
  friend big_int operator^(big_int a, const big_int& b) { a ^= b; return a; }

  big_int &operator&=(const big_int& other) {
    value &= other.value;
    is_overflow = false;
    return *this;
  }

  big_int &operator|=(const big_int& other) {
    value |= other.value;
    is_overflow = false;
    return *this;
  }

  big_int &operator^=(const big_int& other) {
    value ^= other.value;
    is_overflow = false;
    return *this;
  }

  big_int operator~() const { return big_int(~value); }
  bool operator!() const { return !value; }
  explicit operator bool() const { return value != 0; }

  big_int &operator++() { return *this += big_int(1); }
  big_int &operator--() { return *this -= big_int(1); }

  big_int operator++(int) {
    big_int previous(*this);
    ++*this;
    return previous;
  }

  big_int operator--(int) {
    big_int previous(*this);
    --*this;
    return previous;
  }
};



}  // namespace core
}  // namespace JKT

#endif  // JKT_CORE_HXX