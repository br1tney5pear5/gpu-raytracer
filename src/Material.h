#include "Ray.h"

class Material {
public:
    Ray scatter(const Hit& ray);
}

class Lambertian : public Material {
    Ray scatter(const Hit& ray) override {

    }
}
