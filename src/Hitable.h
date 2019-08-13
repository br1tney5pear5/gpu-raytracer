#pragma once
#include <vector>
#include <optional>

class Ray;
struct Hit;

class Hitable {
 public:
    static std::vector<Hitable *> record;
    virtual std::optional<Hit> hit(const Ray& ray) const = 0;
 protected:
    Hitable();
};
