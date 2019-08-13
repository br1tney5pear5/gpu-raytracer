#pragma once
#include <optional>
#include <glm/glm.hpp>
#include "Ray.h"

class Sphere : public Hitable {
 public:
    bool opaque = false;
    Sphere(glm::vec3 origin, float radius);
    std::optional<Hit> hit(const Ray& ray) const override;
protected:
    glm::vec3 origin;
    float radius;
};

