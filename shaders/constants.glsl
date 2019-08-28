#module "constants"

#define PI 3.1415926535897932384626433832795
#define EPS 0.00001

#define WORLD_REFRACTIVE_INDEX 1.0

#define UP        vec3( 0.0, 1.0, 0.0)
#define DOWN      vec3( 0.0,-1.0, 0.0)
#define RIGHT     vec3( 1.0, 0.0, 0.0)
#define LEFT      vec3(-1.0, 0.0, 0.0)
#define FORWARD   vec3( 0.0, 0.0, 1.0)
#define BACKWARDS vec3( 0.0, 0.0,-1.0)

#define SKYBOX_MATERIAL 6
#define MATERIAL2 1

#define SPHERE_HIT 0
#define PLANE_HIT 1
#define SKYBOX_HIT 2
#define BOX_HIT 3

#define RAYS_PER_FRAGMENT 30
#define MAX_RAY_STEPS 5
#define MAX_RAY_LENGTH 10.0
#define LIGHT_PROBES 8
#define SCENE 1

#define LIGHT
#define LIGHTBLEED

