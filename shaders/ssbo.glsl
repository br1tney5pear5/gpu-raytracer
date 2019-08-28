#module "ssbo"
layout(std430, binding = 3) buffer teapot {
    float mesh[];
};

