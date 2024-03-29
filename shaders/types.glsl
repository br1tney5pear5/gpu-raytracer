#module "types"


struct PointLight{
    vec3 position;
    vec3 emission;
};
struct DirectionalLight{
    vec3 direction;
    vec3 emission;
};
struct Material{
    vec3 albedo;
    float roughness;
    float opacity;
    float eta;
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
    int type;
};
struct Sphere{
    vec3 position;
    float radius;
    int material;
};
struct Plane{
    vec3 position;
    vec3 normal;
    vec2 size;
    int material;
};
struct Box {
    vec3 position;
    vec3 size;
    mat4 rotation;
    int material;
};
struct Camera{
    vec3 position;
    vec3 direction;
    float fov;
};
