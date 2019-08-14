__module "types"


struct Material{
    float roughness;
};
struct Ray{
    vec3 origin;
    vec3 direction;
};
struct Hit{
    float t;
    vec3 point;
    vec3 normal;
    int material;
};
struct Sphere{
    vec3 position;
    float radius;
};
struct Plane{
    vec3 normal;
    float size;
};
struct Camera{
    vec3 position;
    vec3 direction;
    float fov;
};
