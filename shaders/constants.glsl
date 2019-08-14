__module "constants"

#define PI 3.1415926535897932384626433832795

#define WORLD_REFRACTIVE_INDEX 1.0

#define UP        vec3( 0.0, 1.0, 0.0)
#define DOWN      vec3( 0.0,-1.0, 0.0)
#define RIGHT     vec3( 1.0, 0.0, 0.0)
#define LEFT      vec3(-1.0, 0.0, 0.0)
#define FORWARD   vec3( 0.0, 0.0, 1.0)
#define BACKWARDS vec3( 0.0, 0.0,-1.0)

#define MATERIAL1 0
#define MATERIAL2 1

#define SPHERE_HIT 0
#define PLANE_HIT 1
#define SKYBOX_HIT 2

#define RAYS_PER_PIXEL 1
#define MAX_RAY_STEPS 10

