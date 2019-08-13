#include "Sphere.h"
#include <glm/glm.hpp>
#include <tgmath.h>

Sphere::Sphere(glm::vec3 origin, float radius)
    : Hitable(), origin(origin), radius(radius)
{}

std::optional<Hit> Sphere::hit(const Ray& ray) const {
    if(opaque) return std::optional<Hit>();

    glm::vec3 C = ray.origin() - origin;
    const float a = pow(glm::length(ray.direction()), 2);
    const float b = 2 * glm::dot(ray.direction(), C);
    const float c = pow(glm::length(C), 2) - pow(radius, 2);
    const float delta = b*b - 4 * a * c;
    if(delta < 0.0) {
        return std::optional<Hit>();
    } else {
        float t1 = (-b + sqrt(delta))/(2 * a);
        float t2 = (-b + sqrt(delta))/(2 * a);
        float t = glm::distance(ray.at(t1), ray.origin()) < glm::distance(ray.at(t2), ray.origin())
                                                            ? t1 : t2;
        auto hitpoint = ray.at(t);
        return std::optional<Hit>({t,
                                   hitpoint,
                                   glm::normalize(hitpoint - origin),
            });
    }
}
