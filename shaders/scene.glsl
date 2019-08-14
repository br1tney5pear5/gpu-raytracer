__module "scene"
__uses "constants"
__uses "types"

#define DIRECTIONAL_LIGHTS_COUNT 1
DirectionalLight directional_lights[DIRECTIONAL_LIGHTS_COUNT] = 
    DirectionalLight[](DirectionalLight(vec3(1.0,-1.0,0.0), vec3(0.2,0.2,0.2)));

#define POINT_LIGHTS_COUNT 1
PointLight point_lights[POINT_LIGHTS_COUNT] = 
    PointLight[](PointLight(vec3(0.0,4.0,4.0), vec3(0.2,0.2,0.2)));

#define MATERIALS_COUNT 3
Material materials[MATERIALS_COUNT] =
    Material[](Material(vec3(0.7,0.5,0.3), 0.2),
               Material(vec3(0.5,0.5,0.7), 0.0),
               Material(vec3(0.5,0.5,0.5), 0.01)
               );
#define LAMBERTIAN 0
#define METAL 1

#define SPHERES_COUNT 7
Sphere spheres[SPHERES_COUNT] =
    Sphere[](Sphere(vec3(-2.0, 0.0, 0.0), 1.0, 0),
             Sphere(vec3(-3.0, 0.0, 0.0), 1.0, 1),
             Sphere(vec3( 2.0, 5.0, 0.0), 0.8, 0),
             Sphere(vec3( 3.0, 0.0, 2.0), 2.0, 2),
             Sphere(vec3(-3.0, 3.0,-1.0), 1.0, 2),
             Sphere(vec3( 2.0, 3.0,-0.6), 1.0, 1),
             Sphere(vec3( 0.0,-101.0, 0.0), 100.0, 0)
             );

#define PLANES_COUNT 2
Plane planes[PLANES_COUNT] =
    Plane[](Plane(vec3(-2.0, 2.0, 0.0), UP, 4.0, 0),
            Plane(vec3(-3.0,-2.0, 0.0), UP, 4.0, 1));
