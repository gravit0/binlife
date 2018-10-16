#include <boost/random.hpp>
#include "random.h"
#include "stats.h"
boost::mt19937_64 rng(time(NULL));
boost::uniform_int<> six(1, 16);
boost::variate_generator<boost::mt19937_64, boost::uniform_int<> > die(rng, six);
Random::Random() {
        pool = rand64();
        pos = 0;
}
unsigned long long int Random::rand64() {
        stats.random++;
        return rng();
}
