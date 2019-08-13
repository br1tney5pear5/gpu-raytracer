#pragma once
#include <vector>
#include <optional>
#include <glm/glm.hpp>
#include <stdint.h>

#include "Hitable.h"

struct Hit {
    float t;
    glm::vec3 point;
    glm::vec3 normal;
};

class Ray {
 public:
    Ray();
    Ray(const glm::vec3 origin, const glm::vec3 direction);
    glm::vec3 at(float t) const;
    glm::vec3 origin() const;
    glm::vec3 direction() const;
    glm::vec3 color(int rec = 0) const;
    std::optional<Hit> hit(const std::vector<Hitable * >& record);
 protected:
    float t_max = 100000;
    glm::vec3 _origin, _direction;
};

class RayConductor {
public:
    glm::vec3 trace(const Ray& ray,
                    const std::vector<Hitable*>& hitables,
                    const size_t steps);
};


// template<size_t MAX_STEPS>
// std::array<Ray, MAX_STEPS> RayConductor<MAX_STEPS>::ray_history
// = std::array<Ray, MAX_STEPS>();
