#module "scene"
#uses "constants"
#uses "types"

#define DIRECTIONAL_LIGHTS_COUNT 1
DirectionalLight directional_lights[DIRECTIONAL_LIGHTS_COUNT] = 
    DirectionalLight[](DirectionalLight(vec3(1.0,-1.0,0.0), vec3(0.2,0.2,0.2)));

#define POINT_LIGHTS_COUNT 1
PointLight point_lights[POINT_LIGHTS_COUNT] = 
    PointLight[](PointLight(vec3(0.0,4.0,4.0), vec3(0.2,0.2,0.2)));

#define MATERIALS_COUNT 8
Material materials[MATERIALS_COUNT] =
    Material[](Material(vec3(0.3,0.3,0.3), 0.7 , 0.9, 1.0), // 0.FLOOR
               Material(vec3(0.9,0.9,0.9), 0.1 , 1.0, 1.0), // 1.METAL
               Material(vec3(0.8,0.8,0.8), 0.0 , 0.0, 0.2), // 2.GLASS0
               Material(vec3(3.6,3.6,3.5), 1.0 , 1.0, 1.0), // 3.LIGHT
               Material(vec3(1.0,1.0,1.0), 1.1 , 1.5, 2.5), // 4.ROUGH METAL
               Material(vec3(1.0,1.0,1.0), 0.2 , 0.7, 1.9), // 5.
               Material(vec3(1.0,1.0,1.0), 0.1 , 1.0, WORLD_REFRACTIVE_INDEX),  // 6.SKYBOX
               Material(vec3(1.0,1.0,1.0), 0.0 , 0.5, 2.5)  // 7.GLASS/METAL
               );
#define LAMBERTIAN 0
#define METAL 1

#define SPHERES_COUNT 6
Sphere spheres[SPHERES_COUNT] =
    Sphere[](Sphere(vec3( 3.0, -0.9,-0.9), 0.1, 7), //small right
             Sphere(vec3(-2.7, 0.0,-1.0), 0.7, 0), // big right
             Sphere(vec3(-0.2,-0.6,-2.0), 0.4, 5), // medium center
             Sphere(vec3( 3.2, -0.2, 0.2), 0.8, 7),
             Sphere(vec3(-1.0,-0.5,-1.0), 0.5, 1),
             Sphere(vec3( 1.0, 0.0,-0.6), 1.0, 7)
             );

#define PLANES_COUNT 4
Plane planes[PLANES_COUNT] =
    Plane[](Plane(vec3(0.0,-1.0, 0.0), vec3(0.0,1.0, 0.0), 7.0, 0),
            Plane(vec3(0.0, 2.0, 3.0), vec3(0.0, 0.0, -1.0), 3.0, 3),
            Plane(vec3(0.0, 2.0, 3.2), vec3(0.0, 0.0, -1.0), 4.0, 4),
            Plane(vec3(0.0, 2.0, 2.9), vec3(0.0, 0.0, -1.0), 2.7, 4)
            );
