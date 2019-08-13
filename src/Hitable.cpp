#include "Hitable.h"
#include <vector>

std::vector<Hitable *> Hitable::record = std::vector<Hitable*>();

Hitable::Hitable() {
    Hitable::record.push_back(this);
}
