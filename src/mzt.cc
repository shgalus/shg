/**
 * \file src/mzt.cc
 * Marsaglia, Zaman and Tsang random number generator.
 * \date Created on 2 June 2009.
 */

#include <shg/mzt.h>
#include <stdexcept>

namespace SHG {

MZT::MZT() : ip_(97), jp_(33), cc_(cc0_) {
     restart(12, 34, 56, 78);
}

MZT::MZT(int i, int j, int k, int l) : ip_(97), jp_(33), cc_(cc0_) {
     restart(i, j, k, l);
}

void MZT::restart(int i, int j, int k, int l) {
     if (i < 1 || i > 178 || j < 1 || j > 178 || k < 1 || k > 178 ||
         l < 0 || l > 168 || (i == 1 && j == 1 && k == 1))
          throw std::invalid_argument(__func__);
     for (int ii = 0; ii < 97; ii++) {
          double s = 0.0, t = 0.5;
          for (int jj = 0; jj < 24; jj++) {
               int m = (((i * j) % 179) * k) % 179;
               i = j;
               j = k;
               k = m;
               l = (53 * l + 1) % 169;
               if ((l * m) % 64 >= 32)
                    s += t;
               t *= 0.5;
          }
          uu_[ii] = s;
     }
}

double MZT::operator()() {
     double pom = uu_[--ip_] - uu_[--jp_];
     if (pom < 0.0)
          pom++;
     uu_[ip_] = pom;
     if (ip_ == 0)
          ip_ = 97;
     if (jp_ == 0)
          jp_ = 97;
     if ((cc_ -= cd_) < 0.0)
          cc_ += cm_;
     if ((pom -= cc_) < 0.0)
          pom++;
     return pom;
}

void MZT::write(std::ostream& f) const {
     f.write(reinterpret_cast<char const*>(uu_), sizeof uu_);
     f.write(reinterpret_cast<char const*>(&ip_), sizeof ip_);
     f.write(reinterpret_cast<char const*>(&jp_), sizeof jp_);
     f.write(reinterpret_cast<char const*>(&cc_), sizeof cc_);
}

void MZT::read(std::istream& f) {
     double uu[97];
     int ip, jp;
     double cc;
     f.read(reinterpret_cast<char*>(uu), sizeof uu);
     if (f.fail())
          return;
     f.read(reinterpret_cast<char*>(&ip), sizeof ip);
     if (f.fail())
          return;
     f.read(reinterpret_cast<char*>(&jp), sizeof jp);
     if (f.fail())
          return;
     f.read(reinterpret_cast<char*>(&cc), sizeof cc);
     if (f.fail())
          return;
     for (int i = 0; i < 97; i++)
          uu_[i] = uu[i];
     ip_ = ip;
     jp_ = jp;
     cc_ = cc;
}

double const MZT::cc0_ = 362436.0 / 16777216.0;
double const MZT::cd_ = 7654321.0 / 16777216.0;
double const MZT::cm_ = 16777213.0 / 16777216.0;

}  // namespace SHG
