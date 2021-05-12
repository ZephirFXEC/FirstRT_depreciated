#include "rtweekend.h"
#include "color.h"
#include "hittable_list.h"
#include "camera.h"
#include "sphere.h"
#include "material.h"
#include <assert.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#define _CRT_SECURE_NO_WARNINGS
using namespace std;

color ray_color(const ray& r, const hittable& world, int depth){
  hit_record rec;
  if(depth <= 0)
  {
    return color(0,0,0);
  }

  if(world.hit(r, 0.001, infinity, rec))
  {
    ray scattered;
    color attenuation;
    if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        return attenuation * ray_color(scattered, world, depth-1);
    return color(0,0,0);
  }

  vec3 unit_dir = unit_vector(r.direction());
  auto t = 0.5f * (unit_dir.y() + 1.0f);
  return (1.0f - t) * color(1.0f, 1.0f, 1.0f) + t*color(0.5f, 0.7f, 1.0f);
}

hittable_list random_scene() {
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    for (int a = 0; a < 4; a++) {
        for (int b = 0; b < 4; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    auto albedo2 = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo2);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.33);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}

class framebuffer {
public:
  static constexpr size_t kBytesPerPixel = 3;

  framebuffer(size_t width, size_t height ):
  width_(width),
  height_(height),
  data_ ((uint8_t*)malloc(width* height* kBytesPerPixel)) {}

  ~framebuffer() { free(data_); }

  void set_pixel(size_t row,
                 size_t col,
                 uint8_t r,
                 uint8_t g,
                 uint8_t b) {
   const size_t idx = kBytesPerPixel * (row * width_ + col);
   data_[idx + 0] = b;
   data_[idx + 1] = g;
   data_[idx + 2] = r;
   }

   void save(const char* file_path) const {
     FILE* fptr = fopen(file_path, "wb");
     assert(fptr);
     putc(0, fptr);
     putc(0, fptr);
     putc(2, fptr);
     putc(0, fptr); putc(0, fptr);
     putc(0, fptr); putc(0, fptr);
     putc(0, fptr);
     putc(0, fptr); putc(0, fptr);
     putc(0, fptr); putc(0, fptr);
     putc((width_ & 0x00FF), fptr);
     putc((width_ & 0xFF00) / 256, fptr);
     putc((height_ & 0x00FF), fptr);
     putc((height_ & 0xFF00) / 256, fptr);
     putc(24, fptr);
     putc(0, fptr);
     fwrite(data_, kBytesPerPixel, width_ * height_, fptr);
     fclose(fptr);
   }

   size_t width() const { return width_; }
   size_t height() const { return height_; }
private:
   uint8_t* data_;
   size_t width_;
   size_t height_;

};

int main() {

    // Image
    double aspect_ratio = 16.0f/9.0f;
    size_t width = 720u;
    size_t height = static_cast<int>(width / aspect_ratio);
    int spp = 100u;
    int max_depth = 4u;

    //world

    auto world = random_scene();

    //Camera

    point3 lookfrom(13,2,3);
    point3 lookat(0,0,0);
    vec3 vup(0,1,0);
    float dist_to_focus = 10.0;
    float aperture = 0.1;

    camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

    // Render
    framebuffer fb {width , height};
    for (size_t j = 0u; j < fb.height(); j++) {
      cerr << "\rScanlines remaining: " << fb.height() - j - 1 << ' ' << flush;
        for (size_t i = 0u; i < fb.width(); i++) {
          color pixel_color(0, 0, 0);
          for (size_t s = 0; s < spp; ++s) {
              const float u = (i + random_double()) / fb.width();
              const float v = (j + random_double()) / fb.height();
              ray r = cam.get_ray(u, v);
              pixel_color += ray_color(r, world, max_depth);
          }
          float scale = 1.0f/spp;
          int8_t r = static_cast<int>(256 * clamp(sqrt(scale*pixel_color.x()), 0.0, 0.999));
          int8_t g = static_cast<int>(256 * clamp(sqrt(scale*pixel_color.y()), 0.0, 0.999));
          int8_t b = static_cast<int>(256 * clamp(sqrt(scale*pixel_color.z()), 0.0, 0.999));
          fb.set_pixel(j, i, r, g, b);
        }
    }

    fb.save("d:\\render.tga");

    cerr << "\nDone.\n";
}
