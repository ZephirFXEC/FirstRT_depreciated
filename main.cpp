#include "rtweekend.hpp"
#include "color.hpp"
#include "hittable_list.hpp"
#include "camera.hpp"
#include "sphere.hpp"
#include "material.hpp"
#include <assert.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include "framebuffer.hpp"
#include "moving_sphere.hpp"
#include "aarect.hpp"
#include "box.hpp"
#define _CRT_SECURE_NO_WARNINGS
using namespace std;

color ray_color(const ray& r, const color& background, const hittable& world, int depth){
  hit_record rec;
  if(depth <= 0)
  {
    return color(0,0,0);
  }

    if(!world.hit(r, 0.001, infinity, rec))
    {
        return background;
    }

    ray scattered;
    color attenuation;
    color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

    if(!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        return emitted;
    
    return emitted + attenuation * ray_color(scattered, background, world, depth-1);
}

hittable_list random_scene() {
    hittable_list world;

    auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));

    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(checker)));
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
                    auto center2 = center + vec3(0, random_double(0,.5), 0);
                    world.add(make_shared<moving_sphere>(
                      center, center2, 0.0, 1.0, 0.2, sphere_material));
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

hittable_list two_spheres() {
    hittable_list objects;

    auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));

    objects.add(make_shared<sphere>(point3(0,-10, 0), 10, make_shared<lambertian>(checker)));
    objects.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker)));

    return objects;
}

hittable_list two_perlin_spheres(){
  hittable_list objects;

  auto pertext = make_shared<noise_texture>(4);
  objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
  objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

  return objects;
}

hittable_list earth() {
    auto earth_texture = make_shared<image_texture>("tex/earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<sphere>(point3(0,0,0), 2, earth_surface);

    return hittable_list(globe);
}

hittable_list simple_light() {
    hittable_list objects;

    auto pertext = make_shared<noise_texture>(4);
    objects.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(point3(0,2,0), 2, make_shared<lambertian>(pertext)));

    auto difflight = make_shared<diffuse_light>(color(4,4,4));
    objects.add(make_shared<xy_rect>(3, 5, 1, 3, -2, difflight));

    return objects;
}

hittable_list cornell_box() {
    hittable_list objects;

    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(213, 343, 227, 332, 554, light));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));
    shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265,0,295));
    objects.add(box1);

    shared_ptr<hittable> box2 = make_shared<box>(point3(0,0,0), point3(165,165,165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130,0,65));
    objects.add(box2);

    return objects;
}

int main() {

    // Image
    float aspect_ratio = 16.0f/9.0f;
    size_t width = 400u;
    size_t spp = 100u;
    const int max_depth = 50;

    // World

    hittable_list world;

    point3 lookfrom;
    point3 lookat;
    float vfov = 40.0;
    float aperture = 0.0;
    color background(0,0,0);

    switch (0) {
        case 1:
            world = random_scene();
            background = color(0.70, 0.80, 1.00);
            lookfrom = point3(13,2,3);
            lookat = point3(0,0,0);
            vfov = 20.0;
            aperture = 0.1;
            break;

        case 2:
            world = two_spheres();
            background = color(0.70, 0.80, 1.00);
            lookfrom = point3(13,2,3);
            lookat = point3(0,0,0);
            vfov = 20.0;
            break;

        case 3:
          world = two_perlin_spheres();
          background = color(0.70, 0.80, 1.00);
          lookfrom = point3(13,2,3);
          lookat = point3(0,0,0);
          vfov = 20.0;
          break;

        case 4:
            world = earth();
            lookfrom = point3(13,2,3);
            background = color(0.70, 0.80, 1.00);
            lookat = point3(0,0,0);
            vfov = 20.0;
            break;
        
        case 5:
            world = simple_light();
            spp = 400;
            background = color(0,0,0);
            lookfrom = point3(26,3,6);
            lookat = point3(0,2,0);
            vfov = 20.0;
            break;

        default:
        case 6:
            world = cornell_box();
            aspect_ratio = 1.0;
            width = 600u;
            spp = 100u;
            background = color(0,0,0);
            lookfrom = point3(278, 278, -800);
            lookat = point3(278, 278, 0);
            vfov = 40.0;
            break;
        }

    // Camera

    vec3 vup(0,1,0);
    auto dist_to_focus = 10.0;
    size_t height = static_cast<int>(width / aspect_ratio);

    camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

    // Render
    framebuffer fb {width , height};
    for (size_t j = 0u; j < fb.height(); j++) {
      cerr << "\rScanlines remaining: " << fb.height() - j - 1 << ' ' << flush;
        for (size_t i = 0u; i < fb.width(); i++) {
          color pixel_color(0, 0, 0);
          for (size_t s = 0; s < spp; s++) {
              const float u = (i + random_double()) / fb.width();
              const float v = (j + random_double()) / fb.height();
              ray r = cam.get_ray(u, v);
              pixel_color += ray_color(r, background, world, max_depth);
          }
          float scale = 1.0f/spp;
          int8_t r = static_cast<int>(256 * clamp(sqrt(scale*pixel_color.x()), 0.0, 0.999));
          int8_t g = static_cast<int>(256 * clamp(sqrt(scale*pixel_color.y()), 0.0, 0.999));
          int8_t b = static_cast<int>(256 * clamp(sqrt(scale*pixel_color.z()), 0.0, 0.999));
          fb.set_pixel(j, i, r, g, b);
        }
    }

    fb.save("c:/Users/enzoc/OneDrive/Bureau/raytracing/render.tga");

    cerr << "\nDone.\n";
    cerr << "\nConverting TGA to PNG ... \n";
    std::system("ffmpeg -i ./render.tga -pix_fmt rgba out.png");
    cerr << "\nFinish !\n";
}
