#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"

#include <iostream>
#include <vector>

using namespace std;

void write_color(ostream &out, color pixel_color, int spp) {
    double r = pixel_color.x();
    double g = pixel_color.y();
    double b = pixel_color.z();
    // Divide the color by the number of samples.
    float scale = 1.0f / spp;
    r = sqrt(scale*r);
    g = sqrt(scale*g);
    b = sqrt(scale*b);


  /*
    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(256 * clamp(sqrt(scale*pixel_color.x()), 0.0, 0.999)) << ' '
        << static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
  */
}

#endif
