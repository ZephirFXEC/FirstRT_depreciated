#ifndef RTWEEKEND_H
#define RTWEEKEND_H


#include <cmath>
#include <limits>
#include <memory>
#include <random>
#include <cstdlib>
#include <iostream>
#include <chrono>
//Using

using namespace std;
using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// const

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

//utility funct

inline double degrees_to_radians(double degrees){
  return degrees*pi/180;
}

inline double random_double() {
    // Returns a random real in [0,1).
    return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max-min)*random_double();
}

inline double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

inline double unif_distrib(double min, double max) {
  std::mt19937_64 rng;
  uint64_t time_seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
  std::seed_seq ss{uint32_t(time_seed & 0xffffffff), uint32_t(time_seed>>32)};
  rng.seed(ss);
  std::uniform_real_distribution<double> unif(min,max);
  return unif(rng);
}

inline double unif_distrib() {
  std::mt19937_64 rng;
  uint64_t time_seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
  std::seed_seq ss{uint32_t(time_seed & 0xffffffff), uint32_t(time_seed>>32)};
  rng.seed(ss);
  std::uniform_real_distribution<double> unif(0,1);
  return unif(rng);
}

inline int random_int(int min, int max) {
    // Returns a random integer in [min,max].
    return static_cast<int>(random_double(min, max+1));
}

#include "ray.h"
#include "vec3.h"

#endif
