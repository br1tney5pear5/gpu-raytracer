#include "Ray.h"

Ray::Ray() 
    : _origin(glm::vec3()), _direction(glm::vec3())
{}

Ray::Ray(const glm::vec3 origin, const glm::vec3 direction)
    : _origin(origin), _direction(glm::normalize(direction))
{}

glm::vec3 Ray::at(float t) const { return _origin + t * _direction; }

glm::vec3 Ray::origin() const { return _origin; }

glm::vec3 Ray::direction() const { return _direction; }


std::optional<Hit> Ray::hit(const std::vector<Hitable *>& hitables){
    float t = t_max;
    Hit ret;
    for(auto hitable : hitables){
        auto opt_hit = hitable->hit(*this);
        if(opt_hit.has_value()){
            auto hit = opt_hit.value();
            auto temp_t = glm::distance(_origin, hit.point);
            if(temp_t < t) {
                t = temp_t;
                ret = hit;
            }
        }
    }
    if(t != t_max) return std::optional<Hit>(ret);
    return std::optional<Hit>();
}


static glm::vec3 random_vec_in_unit_circ(){
    glm::vec3 p;
    do {
        p = 2.0f * glm::vec3(drand48(), drand48(), drand48()) - glm::vec3(1.0); 
    }while(glm::length(p) < 1.0);
    return p;
}

glm::vec3 RayConductor::trace(const Ray& ray,
                              const std::vector<Hitable*>& hitables,
                              const size_t steps)
{
    std::vector<Ray> ray_history = std::vector<Ray>(steps);
    ray_history[0] = ray;

    glm::vec3 col(0.0);
    size_t iter = 0;
    while(iter < steps) {
        auto opt_hit = ray_history[iter].hit(hitables);
        // Material properties
        if(opt_hit.has_value()) {
            Hit hit = opt_hit.value();
            auto dir = glm::normalize(hit.normal + random_vec_in_unit_circ());

            ray_history[++iter] = Ray(hit.point + hit.normal * 0.0001f, dir);
        } else break;
    }
    auto pink = glm::vec3(1.0,0.0,1.0);
    col = pink * static_cast<float>(pow(0.9f, iter));
    col = glm::clamp(col, 0.0f, 1.0f);
    return col;
}
