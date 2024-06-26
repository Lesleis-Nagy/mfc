//
// Created by L. Nagy on 28/06/2023.
//

#ifndef MFC_INCLUDE_FRACTION_HPP_
#define MFC_INCLUDE_FRACTION_HPP_

#include <iostream>
#include <string>
#include <sstream>

#include "utilities.hpp"

class Fraction {

 public:

  Fraction() : _n(0), _d(1) {}

  explicit Fraction(long long n) : _n(n), _d(1) {}

  explicit Fraction(long n) : _n(n), _d(1) {}

  explicit Fraction(int n) : _n(n), _d(1) {}

  Fraction(long long n, long long d) : _n(n), _d(d) {}

  Fraction(long n, long d) : _n(n), _d(d) {}

  Fraction(int n, int d) : _n(n), _d(d) {}

  explicit Fraction(double v, size_t max_precision = 4) : _n(0), _d(1) {
    set(v, max_precision);
  }

  explicit Fraction(const char *v, size_t max_precision = 4) : _n(0), _d(1) {
    set(std::stod(v), max_precision);
  }

  explicit Fraction(const std::string &v, size_t max_precision = 4)
      : _n(0), _d(1) {
    set(std::stod(v), max_precision);
  }

  [[nodiscard]] long long numerator() const {
    return _n;
  }

  [[nodiscard]] long long denominator() const {
    return _d;
  }

  [[nodiscard]] Fraction inverse() const {
    return {_d, _n};
  }

  [[nodiscard]] Fraction operator+(const Fraction &rhs) const {
    auto n = _n * rhs.denominator() + _d * rhs.numerator();
    if (n == 0) {
      return {0, 1};
    }
    auto d = _d * rhs.denominator();
    auto g = gcd(n, d);
    return {n / g, d / g};
  }

  [[nodiscard]] Fraction operator-(const Fraction &rhs) const {
    auto n = _n * rhs.denominator() - _d * rhs.numerator();
    if (n == 0) {
      return {0, 1};
    }
    auto d = _d * rhs.denominator();
    auto g = gcd(n, d);
    return {n / g, d / g};
  }

  [[nodiscard]] Fraction operator*(const Fraction &rhs) const {
    auto n = _n * rhs.numerator();
    if (n == 0) {
      return {0, 1};
    }
    auto d = _d * rhs.denominator();
    auto g = gcd(n, d);
    return {n / g, d / g};
  }

  [[nodiscard]] Fraction operator/(const Fraction &rhs) const {
    auto n = _n * rhs.denominator();
    if (n == 0) {
      return {0, 1};
    }
    auto d = _d * rhs.numerator();
    auto g = gcd(n, d);
    return {n / g, d / g};
  }

  [[nodiscard]] bool operator==(const Fraction &rhs) const {
    return _n * rhs.denominator() == _d * rhs.numerator();
  }

  [[nodiscard]] double as_double() const {
    return (double) _n / (double) _d;
  }

 private:

  [[nodiscard]] long long gcd(long long n1, long long n2) const {

    n1 = (n1 > 0) ? n1 : -n1;
    n2 = (n2 > 0) ? n2 : -n2;

    while (n1 != n2) {
      if (n1 > n2)
        n1 -= n2;
      else
        n2 -= n1;
    }

    return n1;
  }

  void set(double v, size_t max_precision) {

    // The sign of the number.
    long long sign = sgn(v);

    // Make sure the number is positive.
    double vpos = v < 0.0 ? -1.0 * v : v;

    // Isolate the fraction part of the number.
    double frac_part = vpos - (double) ((long long) vpos);

    // Isolate the integer part of the number.
    long long int_part = (long long) vpos;

    // used to 'pop' digits off the frac_part.
    double dbl_power = 10.0;

    // denominator of the fraction part.
    long long denominator = 10;
    for (size_t i = 1; i <= max_precision; ++i) {
      denominator *= 10;
    }

    // used to rebuild digits of the frac_part in to numerator.
    long long int_power = denominator / 10;

    // numerator of the fraction part (accumulated in the for loop).
    long long numerator = 0;

    for (size_t i = 1; i <= max_precision; ++i) {

      // Shift frac_part up by a digit and grab that digit.
      auto digit = (long) (frac_part * dbl_power);

      // Remove the digit from frac_part.
      frac_part = frac_part * dbl_power - (double) digit;

      // Accumulate the digit in to numerator.
      numerator += digit * int_power;

      // Shift to the right by a digit.
      int_power /= 10;

    }

    if (numerator == 0) {

      _n = int_part;
      _d = 1;

    } else {

      auto g1 = gcd(numerator, denominator);

      numerator /= g1;
      denominator /= g1;

      numerator = numerator + int_part * denominator;

      auto g2 = gcd(numerator, denominator);

      numerator /= g2;
      denominator /= g2;

      _n = sign * numerator;
      _d = denominator;

    }
  }

  long long _n;
  long long _d;

};

typedef std::pair<Fraction, Fraction> FractionPair;

template<>
struct std::hash<Fraction> {
  std::size_t operator()(Fraction const &v) const noexcept {
    std::size_t n = std::hash<long long>{}(v.numerator());
    std::size_t d = std::hash<long long>{}(v.denominator());

    size_t seed = 0;

    hash_combine(seed, n);
    hash_combine(seed, d);

    return seed;
  }
};

template<>
struct std::hash<FractionPair> {
  std::size_t operator()(FractionPair const &v) const noexcept {
    std::size_t n = std::hash<Fraction>{}(v.first);
    std::size_t d = std::hash<Fraction>{}(v.second);

    size_t seed = 0;

    hash_combine(seed, n);
    hash_combine(seed, d);

    return seed;
  }
};

std::ostream &operator<<(std::ostream &out, const Fraction &fp) {
  out << fp.numerator() << "/" << fp.denominator();
  return out;
}

std::ostream &operator<<(std::ostream &out, const FractionPair &fp) {
  out << "(" << fp.first << ", " << fp.second << ")";
  return out;
}

#endif // MFC_INCLUDE_FRACTION_HPP_