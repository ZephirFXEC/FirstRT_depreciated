#ifndef RTWEEKEND_H
#define RTWEEKEND_H


#include <cmath>
#include <limits>
#include <memory>
#include <random>
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <algorithm>
//Using

using namespace std;
using std::shared_ptr;
using std::make_shared;
using std::sqrt;
std::mt19937 rng;
// const

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

//utility funct

inline double degrees_to_radians(double degrees){
  return degrees*pi/180;
}

//inline double random_double() {
    // Returns a random real in [0,1).
//    return rand() / (RAND_MAX + 1.0);
//}

inline double random_double()
{
    unsigned int tofloat = 0x2f800004u;
    static unsigned long x = 123456789, y = 362436069, z = 521288629;

    unsigned long t;
    x ^= x << 16;
    x ^= x >> 5;
    x ^= x << 1;

    t = x;
    x = y;
    y = z;
    z = t ^ x ^ y;

    float a = static_cast<float>(z) * reinterpret_cast<const float&>(tofloat);

    return a;
}

inline double random_double(double min, double max) {
    //Returns a random real in [min,max).
    return min + (max-min)*random_double();
}

inline double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}


inline int random_int(int min, int max) {
    // Returns a random integer in [min,max].
    return static_cast<int>(random_double(min, max+1));
}


#include "ray.hpp"
#include "vec3.hpp"

#endif
