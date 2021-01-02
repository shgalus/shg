#include <cassert>
#include <iostream>
#include <iomanip>
#include <stdexcept>

using std::cout;
using std::size_t;
using N0 = std::size_t;

template <typename T>
class Field {
public:
     typedef T Element_type;
     
     Field() = default;
     Field(const T& el);

     Field operator+() const;
     Field operator-() const;

     Field& operator++();
     Field operator++(int);
     Field& operator--();
     Field operator--(int);

     Field& operator+=(const Field& rhs);
     Field& operator-=(const Field& rhs);
     Field& operator*=(const Field& rhs);
     Field& operator/=(const Field& rhs);

     template <typename U>
     friend inline bool operator==(const Field<U>& lhs, const Field<U>& rhs);

     template <typename U>
     friend inline std::ostream& operator<<(std::ostream& stream, const Field<U>& rhs);

     inline static Field zero();
     inline static Field one();

     Field additive_inverse() const;
     Field multiplicative_inverse() const;

private:
     T el_ {T(0)};
};

template <typename T>
Field<T>::Field(const T& el) : el_(el) {}

template <typename T>
Field<T> Field<T>::operator+() const {
     return *this;
}

template <typename T>
Field<T> Field<T>::operator-() const {
     Field tmp(*this);
     tmp.el_ = -tmp.el_;
     return tmp;
}

template <typename T>
Field<T>& Field<T>::operator++() {
     el_++;
     return *this;
}

template <typename T>
Field<T> Field<T>::operator++(int) {
     Field tmp(*this);
     operator++();
     return tmp;
}

template <typename T>
Field<T>& Field<T>::operator--() {
     el_--;
     return *this;
}

template <typename T>
Field<T> Field<T>::operator--(int) {
     Field tmp(*this);
     operator--();
     return tmp;
}

template <typename T>
Field<T>& Field<T>::operator+=(const Field& rhs) {
     el_ += rhs.el_;
     return *this;
}

template <typename T>
Field<T>& Field<T>::operator-=(const Field& rhs) {
     el_ -= rhs.el_;
     return *this;
}

template <typename T>
Field<T>& Field<T>::operator*=(const Field& rhs) {
     el_ *= rhs.el_;
     return *this;
}

template <typename T>
Field<T>& Field<T>::operator/=(const Field& rhs) {
     el_ /= rhs.el_;
     return *this;
}

template <typename T>
bool operator==(const Field<T>& lhs, const Field<T>& rhs) {
     return lhs.el_ == rhs.el_;
}

// general template to define != by ==
template <typename T>
bool operator!=(const T& lhs, const T& rhs) {
     return !(lhs == rhs);
}

/*
template <typename T>
bool operator!=(const Field<T>& lhs, const Field<T>& rhs) {
     return !(lhs == rhs);
}
*/
template <typename T>
std::ostream& operator<<(std::ostream& stream, const Field<T>& rhs) {
     stream << rhs.el_;
     return stream;
}

template <typename T>
Field<T> Field<T>::zero() {
     return T(0);
}

template <typename T>
Field<T> Field<T>::one() {
     return T(1);
}

template <typename T>
Field<T> Field<T>::additive_inverse() const {
     return -*this;
}


template <typename T>
T operator+(const T& lhs, const T& rhs) {
     T tmp(lhs);
     tmp += rhs;
     return tmp;
}

template <typename T>
T operator-(const T& lhs, const T& rhs) {
     T tmp(lhs);
     tmp -= rhs;
     return tmp;
}

template <typename T>
T operator*(const T& lhs, const T& rhs) {
     T tmp(lhs);
     tmp *= rhs;
     return tmp;
}

template <typename T>
T operator/(const T& lhs, const T& rhs) {
     T tmp(lhs);
     tmp /= rhs;
     return tmp;
}

template <std::size_t p>
class Fp {
public:
     Fp() = default;
     inline Fp(std::size_t n);
     Fp operator-() const;
     inline operator std::size_t() const;
     Fp& operator++();
     Fp operator++(int);
     Fp& operator+=(const Fp& rhs);
     
     Fp& operator--();
     Fp operator--(int);
     Fp& operator-=(const Fp& rhs);     

     inline Fp additive_inverse() const;
     Fp multiplicative_inverse() const;
     
private:
     std::size_t v_ {0};
};

template <std::size_t p>
Fp<p>::Fp(std::size_t n) : v_(n % p) {}

template <std::size_t p>
Fp<p> Fp<p>::operator-() const {
     Fp tmp(*this);
     if (tmp.v_ != 0)
          tmp.v_ = p - tmp.v_;
     return tmp;
}

// Needed by operator <<.
template <std::size_t p>
Fp<p>::operator std::size_t() const {return v_;}

template <std::size_t p>
Fp<p>& Fp<p>::operator++() {
     if (++v_ == p)
          v_ = 0;
     return *this;
}

template <std::size_t p>
Fp<p> Fp<p>::operator++(int) {
     Fp tmp(*this);
     operator++();
     return tmp;
}

template <std::size_t p>
Fp<p>& Fp<p>::operator+=(const Fp& rhs) {
     v_ += rhs.v_;
     if (v_ >= p)
          v_ -= p;
     // v_ = (v_ + rhs.v_) % p;
     return *this;
}

template <std::size_t p>
Fp<p>& Fp<p>::operator--() {
     if (v_ == 0)
          v_ = p;
     --v_;
     return *this;
}

template <std::size_t p>
Fp<p> Fp<p>::operator--(int) {
     Fp tmp(*this);
     operator--();
     return tmp;
}

template <std::size_t p>
Fp<p>& Fp<p>::operator-=(const Fp& rhs) {
     // std::cout << "@@@: " << *this << "  " << rhs << " = ";
     operator+=(-rhs);
     // std::cout << *this << '\n';
     return *this;
}

template <std::size_t p>
Fp<p> Fp<p>::additive_inverse() const {
     return -*this;
}

template <std::size_t p>
Fp<p> Fp<p>::multiplicative_inverse() const {
     
}

class F2 {
public:
     F2() = default;
     F2(size_t p) {if (p == 0) v_ = 0; else v_ = 1;}
     F2& operator++() {
          if (v_ == 0)
               v_ = 1;
          else
               v_ = 0;
          return *this;
     }
     F2 operator++(int) {
          F2 tmp(*this);
          operator++();
          return tmp;
     }
     
     F2& operator+=(const F2& rhs) {
          v_ += rhs.v_;
          v_ %= 2;
          return *this;
     }
     size_t v() const {return v_;}
     friend inline bool operator==(const F2& lhs, const F2& rhs);
private:

     size_t v_ {0};
};

std::ostream& operator<<(std::ostream& stream, const F2& f) {
     stream.width(0);
     stream << f.v();
     return stream;
}

bool operator==(const F2& lhs, const F2& rhs) {
     return lhs.v_ == rhs.v_;
}

int main() {

     Field<double> x;
     cout << x << '\n';
     x++;
     cout << x << '\n';
     ++x;
     cout << x << '\n';
     Field<double> y;
     y++;
     cout << y << '\n';
     Field<double> z = x + y;
     cout << z << '\n';

     Field<double> w(z);
     cout << w << '\n';

     Field<F2> f1, f2;
     f1++;
     ++f2;
     Field<F2> f3 = f1 + f2;
     cout << f3 << '\n';

     Field<Fp<3>> a;
     std::cout << a << '\n';
     for (std::size_t i = 0; i < 3; i++) {
          for (std::size_t j = 0; j < 3; j++) {
               Field<Fp<3>> f(i);               
               Field<Fp<3>> g(j);
               f -= g;
               std::cout << f << " "; 
          }
          std::cout << '\n';
     }

     std::cout << (f1 == f2) << '\n';
     for (std::size_t i = 0; i < 3; i++) {
          Field<Fp<3>> f(i);
          std::cout << "inv(" << f << ") = " << f.additive_inverse() << '\n';
     }
}
