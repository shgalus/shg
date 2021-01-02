#include <cassert>
#include <iostream>
#include <iomanip>

template <class T>
class A {
public:
     A& operator++() {el_++; return *this;}
     void show() const {
          std::cout << "A::el_ = " << el_ << '\n';
     }
protected:
     T el_{T(0)};
};

template <class T>
class B : public A<T> {
public:
     B& operator++() {A<T>::operator++(); return *this;}
     void show() const {
          std::cout << "B::el_ = " << A<T>::el_ << '\n';
     }
};

using namespace std;

int main() {
     B<int> b;
     b.show();
     ++b;
     b.show();     
}
