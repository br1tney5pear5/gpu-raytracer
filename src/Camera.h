#pragma once
#include "Ray.h"
#include <tgmath.h>

class Camera {
public:
    Camera(float vfov, float aspect_ratio) {
        origin = glm::vec3(0.0f,0.0f,0.1f);
        float theta = vfov * M_PI / 180;

        float half_height = tan(theta/2);
        float half_width = aspect_ratio * half_height;
        plane_rect = glm::vec3(2 * half_width, 2 * half_height, 1.0f);
        plane_origin = glm::vec3(-half_width, -half_height, 1.0f);
    }
    Ray ray_at(float u, float v) {
        return Ray(origin, plane_origin + plane_rect * glm::vec3(u,v,0) - origin);
    }
protected:
    glm::vec3 plane_rect;
    glm::vec3 plane_origin;
    glm::vec3 origin;
    glm::vec3 vertical;
    glm::vec3 horizontal;
};
