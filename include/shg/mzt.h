/**
 * \file include/shg/mzt.h
 * Marsaglia, Zaman and Tsang random number generator.
 * \date Created on 2 June 2009.
 */

#ifndef SHG_MZT_H
#define SHG_MZT_H

#include <iostream>
#include <shg/rng.h>

namespace SHG {

/**
 * Marsaglia, Zaman and Tsang random number generator. The generator
 * can primarily be used to generate random numbers uniformly
 * distributed on [0, 1]. The implementation follows that of
 * \cite wieczorkowski-zielinski-1997, p. 39-40.
 *
 * \ingroup random_number_generator
 */
class MZT : public RNG {
public:
     /**
      * Constructs a generator with i = 12, j = 34, k = 56, l = 78.
      */
     MZT();
     /**
      * The constructor requires four integer numbers: i, j, k from
      * [1, 178] and l from [0, 168], with the case i = j = k = 1
      * excluded.
      *
      * \exception std::invalid_argument if the arguments do not
      * fulfill the above conditions
      */
     MZT(int i, int j, int k, int l);
     /**
      * Re-initializes the generator. The method requires four integer
      * numbers: i, j, k from [1, 178] and l from [0, 168], with the
      * case i = j = k = 1 excluded.
      *
      * \exception std::invalid_argument if the arguments do not
      * fulfill the above conditions
      */
     void restart(int i, int j, int k, int l);
     /**
      * Returns the next random number uniformly distributed on [0,
      * 1].
      */
     virtual double operator()();
     virtual void write(std::ostream& f) const;
     virtual void read(std::istream& f);

private:
     double uu_[97];
     int ip_, jp_;
     double cc_;
     static double const cc0_, cd_, cm_;
};

}  // namespace SHG

#endif
