#ifndef GLSLVIEWER
  #version 330
#endif

//=============================== types
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
    float size;
    int material;
};
struct Camera{
    vec3 position;
    vec3 direction;
    float fov;
};

//=============================== constants
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

#define RAYS_PER_FRAGMENT 10
#define MAX_RAY_STEPS 6
#define MAX_RAY_LENGTH 1000.0
#define LIGHT_PROBES 10

// #define LIGHT
#define LIGHTBLEED


//=============================== quaternion
// Prefix Legend
// n - normalized input
vec4 quat_mult(vec4 q1, vec4 q2) { // TODO: OPTIMIZE
    vec4 qret;
    qret.w = q1.x*q2.x + q1.x*q2.x + q1.x*q2.x + q1.x*q2.x;
    qret.x = q1.x*q2.w + q1.w*q2.x + q1.y*q2.z - q1.z*q2.y;
    qret.y = q1.y*q2.w + q1.w*q2.y + q1.z*q2.x - q1.x*q2.z;
    qret.z = q1.z*q2.w + q1.w*q2.z + q1.x*q2.y - q1.y*q2.x;
    return qret;
}

vec4 quat_conj(vec4 q) {
    return q * vec4(-1.0,-1.0,-1.0,1.0);
}

vec4 quat(vec3 axis, float angle) {
    angle *= PI/360.0; // additional division by two
    return vec4(normalize(axis) * sin(angle), cos(angle));
}

vec3 quat_rot(vec3 v, vec4 q) {
    return quat_mult(quat_mult(q, vec4(v, 0.0)), quat_conj(q)).xyz;
}

vec4 nvec2nvec_trans_quat(vec3 a, vec3 b) { 
    vec3 axis = cross(a,b);
    float sine = length(axis);
    float cosine = dot(a,b);
    return vec4(axis * sine, cosine);
}

//=============================== uniforms
uniform float u_time;
uniform float u_resolution;
varying vec4 v_position;

//=============================== random
// needs u_time

// Prefixes legend
// p - pseudo (pseudo random number generator)
// u - unit (unit vector)
// bi - bipolar (between -1 and 1)
// t - time based (depends on u_time uniform)

#define _RTABLE_SZ 30
float _rtable[_RTABLE_SZ] = float[](
  0.1423379664, 0.9167794718, 0.4464730226,
  0.3356152508, 0.6605720105, 0.5035941418,
  0.0867193540, 0.8604511685, 0.2954168330,
  0.2845960435, 0.8537994198, 0.7203180557,
  0.5788900111, 0.8457761476, 0.3552139777,
  0.1052490854, 0.4952968815, 0.7208292198,
  0.1162629763, 0.3902010835, 0.1313408282,
  0.8585971515, 0.8015519997, 0.7313578742,
  0.5810265761, 0.8502936394, 0.9322189191,
  0.7558817130, 0.4575346629, 0.4304639553
);


float prand(vec2 seed, float t) {
    float x = fract(0.58333 * t) + fract(1.89333 * t + 12.99);
    return fract(sin(dot(seed.xy, vec2(12.9898,78.233))
                     ) * 43758.5453123 * x);
}

float prand(vec2 seed, int n) {
    return prand(seed,_rtable[n %_RTABLE_SZ]); }
float prand(vec2 seed) {
    return prand(seed,0.0); }

float trand(vec2 seed, int n) {
    return prand(seed,_rtable[n %_RTABLE_SZ] + u_time); }
float trand(vec2 seed) {
    return prand(seed,u_time); }

float biprand(vec2 seed, int n) {
    return prand(seed,_rtable[n %_RTABLE_SZ]) * 2.0 - 1.0; }
float biprand(vec2 seed) {
    return prand(seed,0.0) * 2.0 - 1.0; }

float bitrand(vec2 seed, int n) {
    return prand(seed,_rtable[n %_RTABLE_SZ] + u_time) * 2.0 - 1.0; }
float bitrand(vec2 seed) {
    return prand(seed,u_time) * 2.0 - 1.0; }


vec3 trand_uvec3(vec2 seed){
    return vec3(bitrand(seed,2), bitrand(seed,9), bitrand(seed,7));}

vec3 trand_uvec3(vec2 seed, int n){
    return vec3(bitrand(seed,2 + n), bitrand(seed,9 + n), bitrand(seed,7 + n));}


vec3 prand_uvec3(vec2 seed){
    return vec3(biprand(seed,2), biprand(seed,9), biprand(seed,7));}

vec3 prand_uvec3(vec2 seed, int n){
    return vec3(biprand(seed,2 + n), biprand(seed,9 + n), biprand(seed,7 + n));}

// NOTE: This function gives really nice patters, do not delete!
// float _nicerand_counter = 0.0;
// float nicerand(vec2 seed) {
//     _nicerand_counter += fract(0.58333 * u_time);
//     return fract(sin(dot(seed.xy, vec2(12.9898,78.233))
//                      ) * 43758.5453123 + _nicerand_counter);
// }


//=============================== math
// reimplement with quaternions, there should be faster solution
mat3 vec_to_vec_map(vec3 a, vec3 b) {
    if(a == b) return mat3(1.0);
    if(a == -b) return mat3(1.0);
    vec3 v =  cross(a,b);
    float sine = length(v);
    float cosine = dot(a,b);
    mat3 V, R;
    V[0] = vec3( 0.0, v.z,-v.y);
    V[1] = vec3(-v.z, 0.0, v.x);
    V[2] = vec3( v.y, v.x, 0.0);
    R = mat3(1.0) + V + V*V*1.0/(1.0 - cosine);
    return R;
}


//=============================== colors
#define MAROON vec3(0.501961,0.0,0.0)
#define DARK_RED vec3(0.545098,0.0,0.0)
#define BROWN vec3(0.647059,0.164706,0.164706)
#define FIREBRICK vec3(0.698039,0.133333,0.133333)
#define CRIMSON vec3(0.862745,0.078431,0.235294)
#define RED vec3(1.0,0.0,0.0)
#define TOMATO vec3(1.0,0.388235,0.278431)
#define CORAL vec3(1.0,0.498039,0.313725)
#define INDIAN_RED vec3(0.803922,0.360784,0.360784)
#define LIGHT_CORAL vec3(0.941176,0.501961,0.501961)
#define DARK_SALMON vec3(0.913725,0.588235,0.478431)
#define SALMON vec3(0.980392,0.501961,0.447059)
#define LIGHT_SALMON vec3(1.0,0.627451,0.478431)
#define ORANGE_RED vec3(1.0,0.270588,0.0)
#define DARK_ORANGE vec3(1.0,0.54902,0.0)
#define ORANGE vec3(1.0,0.647059,0.0)
#define GOLD vec3(1.0,0.843137,0.0)
#define DARK_GOLDEN_ROD vec3(0.721569,0.52549,0.043137)
#define GOLDEN_ROD vec3(0.854902,0.647059,0.12549)
#define PALE_GOLDENkROD vec3(0.933333,0.909804,0.666667)
#define DARK_KHAKI vec3(0.741176,0.717647,0.419608)
#define KHAKI vec3(0.941176,0.901961,0.54902)
#define OLIVE vec3(0.501961,0.501961,0.0)
#define YELLOW vec3(1.0,1.0,0.0)
#define YELLOW_GREEN vec3(0.603922,0.803922,0.196078)
#define DARK_OLIVE GREEN vec3(0.333333,0.419608,0.184314)
#define OLIVE_DRAB vec3(0.419608,0.556863,0.137255)
#define LAWN_GREEN vec3(0.486275,0.988235,0.0)
#define CHART_REUSE vec3(0.498039,1.0,0.0)
#define GREEN_YELLOW vec3(0.678431,1.0,0.184314)
#define DARK_GREEN vec3(0.0,0.392157,0.0)
#define GREEN vec3(0.0,0.501961,0.0)
#define FOREST_GREEN vec3(0.133333,0.545098,0.133333)
#define LIME vec3(0.0,1.0,0.0)
#define LIME_GREEN vec3(0.196078,0.803922,0.196078)
#define LIGHT_GREEN vec3(0.564706,0.933333,0.564706)
#define PALE_GREEN vec3(0.596078,0.984314,0.596078)
#define DARK_SEA GREEN vec3(0.560784,0.737255,0.560784)
#define MEDIUM_SPRING GREEN vec3(0.0,0.980392,0.603922)
#define SPRING_GREEN vec3(0.0,1.0,0.498039)
#define SEA_GREEN vec3(0.180392,0.545098,0.341176)
#define MEDIUM_AQUA_MARINE vec3(0.4,0.803922,0.666667)
#define MEDIUM_SEA_GREEN vec3(0.235294,0.701961,0.443137)
#define LIGHT_SEA_GREEN vec3(0.12549,0.698039,0.666667)
#define DARK_SLATE_GRAY vec3(0.184314,0.309804,0.309804)
#define DARK_SLATE_GREY vec3(0.184314,0.309804,0.309804)
#define TEAL vec3(0.0,0.501961,0.501961)
#define DARK_CYAN vec3(0.0,0.545098,0.545098)
#define AQUA vec3(0.0,1.0,1.0)
#define CYAN vec3(0.0,1.0,1.0)
#define LIGHT_CYAN vec3(0.878431,1.0,1.0)
#define DARK_TURQUOISE vec3(0.0,0.807843,0.819608)
#define TURQUOISE vec3(0.25098,0.878431,0.815686)
#define MEDIUM_TURQUOISE vec3(0.282353,0.819608,0.8)
#define PALE_TURQUOISE vec3(0.686275,0.933333,0.933333)
#define AQUA_MARINE vec3(0.498039,1.0,0.831373)
#define POWDER_BLUE vec3(0.690196,0.878431,0.901961)
#define CADET_BLUE vec3(0.372549,0.619608,0.627451)
#define STEEL_BLUE vec3(0.27451,0.509804,0.705882)
#define CORN_FLOWER_BLUE vec3(0.392157,0.584314,0.929412)
#define DEEP_SKY_BLUE vec3(0.0,0.74902,1.0)
#define DODGER_BLUE vec3(0.117647,0.564706,1.0)
#define LIGHT_BLUE vec3(0.678431,0.847059,0.901961)
#define SKY_BLUE vec3(0.529412,0.807843,0.921569)
#define LIGHT_SKY_BLUE vec3(0.529412,0.807843,0.980392)
#define MIDNIGHT_BLUE vec3(0.098039,0.098039,0.439216)
#define NAVY vec3(0.0,0.0,0.501961)
#define DARK_BLUE vec3(0.0,0.0,0.545098)
#define MEDIUM_BLUE vec3(0.0,0.0,0.803922)
#define BLUE vec3(0.0,0.0,1.0)
#define ROYAL_BLUE vec3(0.254902,0.411765,0.882353)
#define BLUE_VIOLET vec3(0.541176,0.168627,0.886275)
#define INDIGO vec3(0.294118,0.0,0.509804)
#define DARK_SLATE_BLUE vec3(0.282353,0.239216,0.545098)
#define SLATE_BLUE vec3(0.415686,0.352941,0.803922)
#define MEDIUM_SLATE_BLUE vec3(0.482353,0.407843,0.933333)
#define MEDIUM_PURPLE vec3(0.576471,0.439216,0.858824)
#define DARK_MAGENTA vec3(0.545098,0.0,0.545098)
#define DARK_VIOLET vec3(0.580392,0.0,0.827451)
#define DARK_ORCHID vec3(0.6,0.196078,0.8)
#define MEDIUM_ORCHID vec3(0.729412,0.333333,0.827451)
#define PURPLE vec3(0.501961,0.0,0.501961)
#define THISTLE vec3(0.847059,0.74902,0.847059)
#define PLUM vec3(0.866667,0.627451,0.866667)
#define VIOLET vec3(0.933333,0.509804,0.933333)
#define MAGENTA vec3(1.0,0.0,1.0)
#define ORCHID vec3(0.854902,0.439216,0.839216)
#define MEDIUM_VIOLET_RED vec3(0.780392,0.082353,0.521569)
#define PALE_VIOLET_RED vec3(0.858824,0.439216,0.576471)
#define DEEP_PINK vec3(1.0,0.078431,0.576471)
#define HOT_PINK vec3(1.0,0.411765,0.705882)
#define LIGHT_PINK vec3(1.0,0.713725,0.756863)
#define PINK vec3(1.0,0.752941,0.796078)
#define ANTIQUE_WHITE vec3(0.980392,0.921569,0.843137)
#define BEIGE vec3(0.960784,0.960784,0.862745)
#define BISQUE vec3(1.0,0.894118,0.768627)
#define BLANCHED_ALMOND vec3(1.0,0.921569,0.803922)
#define WHEAT vec3(0.960784,0.870588,0.701961)
#define CORN_SILK vec3(1.0,0.972549,0.862745)
#define LEMON_CHIFFON vec3(1.0,0.980392,0.803922)
#define LIGHT_GOLDEN_ROD_YELLOW vec3(0.980392,0.980392,0.823529)
#define LIGHT_YELLOW (1.0,1.0,0.878431)
#define SADDLE_BROWN vec3(0.545098,0.270588,0.07451)
#define SIENNA vec3(0.627451,0.321569,0.176471)
#define CHOCOLATE vec3(0.823529,0.411765,0.117647)
#define PERU vec3(0.803922,0.521569,0.247059)
#define SANDY_BROWN vec3(0.956863,0.643137,0.376471)
#define BURLY_WOOD vec3(0.870588,0.721569,0.529412)
#define TAN vec3(0.823529,0.705882,0.54902)
#define ROSY_BROWN vec3(0.737255,0.560784,0.560784)
#define MOCCASIN vec3(1.0,0.894118,0.709804)
#define NAVAJO_WHITE vec3(1.0,0.870588,0.678431)
#define PEACH_PUFF vec3(1.0,0.854902,0.72549)
#define MISTY_ROSE vec3(1.0,0.894118,0.882353)
#define LAVENDER_BLUSH vec3(1.0,0.941176,0.960784)
#define LINEN vec3(0.980392,0.941176,0.901961)
#define OLD_LACE vec3(0.992157,0.960784,0.901961)
#define PAPAYA_WHIP vec3(1.0,0.937255,0.835294)
#define SEA_SHELL vec3(1.0,0.960784,0.933333)
#define MINT_CREAM vec3(0.960784,1.0,0.980392)
#define SLATE_GRAY vec3(0.439216,0.501961,0.564706)
#define LIGHT_SLATE_GRAY vec3(0.466667,0.533333,0.6)
#define LIGHT_SLATE_GREY vec3(0.466667,0.533333,0.6)
#define LIGHT_STEEL_BLUE vec3(0.690196,0.768627,0.870588)
#define LAVENDER vec3(0.901961,0.901961,0.980392)
#define FLORAL_WHITE vec3(1.0,0.980392,0.941176)
#define ALICE_BLUE vec3(0.941176,0.972549,1.0)
#define GHOST_WHITE vec3(0.972549,0.972549,1.0)
#define HONEYDEW vec3(0.941176,1.0,0.941176)
#define IVORY vec3(1.0,1.0,0.941176)
#define AZURE vec3(0.941176,1.0,1.0)
#define SNOW vec3(1.0,0.980392,0.980392)
#define BLACK vec3(0.0,0.0,0.0)
#define DIM_GRAY vec3(0.411765,0.411765,0.411765)
#define DIM_GREY vec3(0.411765,0.411765,0.411765)
#define GRAY vec3(0.501961,0.501961,0.501961)
#define GREY vec3(0.501961,0.501961,0.501961)
#define DARK_GRAY vec3(0.662745,0.662745,0.662745)
#define DARK_GREY vec3(0.662745,0.662745,0.662745)
#define SILVER vec3(0.752941,0.752941,0.752941)
#define LIGHT_GRAY vec3(0.827451,0.827451,0.827451)
#define LIGHT_GREY vec3(0.827451,0.827451,0.827451)
#define GAINSBORO vec3(0.862745,0.862745,0.862745)
#define WHITE_SMOKE vec3(0.960784,0.960784,0.960784)
#define WHITE vec3(1.0,1.0,1.0)

//=============================== util
vec3 split(vec3 a, vec3 b, bool trigger) {
    if(!trigger) return a;
    if(v_position.x >
       0.1*sin(u_time + v_position.y)
       ) {return a;}
    else {return b;}
}

vec3 split(vec3 a, vec3 b) {
    return split(a,b,true);
}

//=============================== scene
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

//=============================== rays_util
vec3 reflect(vec3 n, vec3 v) {
    return v - 2.0 * dot(n,v) * n;
}

vec3 refract(vec3 n, vec3 v, float eta) {
    float cos_in = dot(v,n);

    float discriminant = 1.0 - (1.0 - cos_in) * eta * eta;
    if(discriminant > 0.0){
        return eta * (v + n * cos_in) - n * sqrt(discriminant);
    }
    return n;
}


//=============================== intersect
Hit intersect(Ray ray, bool lightprobe) {
    Hit hit;
    int index = -1;
    float t = MAX_RAY_LENGTH;
    float temp_t;

    float a,b,c,delta;
    float thres = EPS;
    for(int i=0; i < SPHERES_COUNT; i++) { // 2 cancels
        vec3 origin_diff = ray.origin - spheres[i].position;
        a = dot(ray.direction, ray.direction);
        b = dot(origin_diff, ray.direction);
        c = dot(origin_diff, origin_diff) - pow(spheres[i].radius, 2.0);
        delta = b*b - a * c;
        if(delta > 0.0) {
            c = sqrt(delta); // reuse c, sqrt_delta
            temp_t = (-b + c) / a; // reuse temp_t as t1
            delta  = (-b - c) / a; // reuse delta  as t2

            if(temp_t * delta <= 0.0001) thres = 0.1;

            if(temp_t <= 0.0) temp_t = MAX_RAY_LENGTH;
            if(delta <= 0.0) delta = MAX_RAY_LENGTH;

            temp_t = min(temp_t, delta);

            // if(materials[spheres[i].material].opacity)
            if(temp_t < t && temp_t > thres) {
                t = temp_t;
                index = i;
            }
        }
    }
    if(index != -1) {
        hit.t = t;
        hit.point = ray.origin + ray.direction * t;
        hit.normal = normalize(hit.point - spheres[index].position);
        hit.material = spheres[index].material;
        hit.type = SPHERE_HIT;
    }

    index = -1;
    float denominator, numerator;
    for(int i=0; i < PLANES_COUNT; i++) { // 2 cancels
        denominator = dot(ray.direction, planes[i].normal);
        if(denominator < 0.0) {
            numerator = dot(planes[i].position - ray.origin, planes[i].normal);
            temp_t = numerator/denominator;
            if(temp_t < t && temp_t > EPS){
              vec3 point = ray.origin + ray.direction * temp_t;
              mat3 R = vec_to_vec_map(planes[i].normal, UP);
              vec3 ps_point = abs(R * (point - planes[i].position));
                if(max(ps_point.x, ps_point.z) < planes[i].size){
                    t = temp_t;
                    index = i;
                }
            }
        }

    }
    if(index != -1) {
        hit.t = t;
        hit.point = ray.origin + ray.direction * t;
        hit.normal = planes[index].normal;
        hit.material = planes[index].material;
        hit.type = PLANE_HIT;
    }

    if(t == MAX_RAY_LENGTH) {
        hit.t = -1.0;
        hit.type = SKYBOX_HIT;
        hit.material = SKYBOX_MATERIAL;
    }
    return hit;

}

//TEMP
Hit intersect(Ray ray) {return intersect(ray, false);}

//=============================== mainfrag
Ray rays[MAX_RAY_STEPS + 1];
Hit hits[MAX_RAY_STEPS + 1];

Camera camera;
Ray cam_ray;

void main() {

    camera.position = vec3(0.0,1.0,-5.0);
    camera.direction = vec3(0.0,0.0,0.0);

    //LAMP
    

    cam_ray.origin = camera.position;


    vec3 campos = vec3(v_position.xy,0.0);

    cam_ray.direction = //TODO: change to quaternions
        // vec_to_vec_map(FORWARD, camera.direction) *
        (v_position.xyz + FORWARD);

    vec3 out_col = vec3(0.0);
    Hit lightprobe;

    vec3 lightbleed = vec3(0.0);
#ifdef LIGHTBLEED
    for(int rpp = 0; rpp < RAYS_PER_FRAGMENT; rpp++) {
        lightprobe = intersect(Ray(camera.position, 
                                   normalize(cam_ray.direction  +
                                             prand_uvec3(campos.xy,rpp) *
                                             prand(campos.xy,rpp) *
                                             float(rpp)/float(RAYS_PER_FRAGMENT) * 0.6 
                                             )));
        if(lightprobe.t > 0.0) {
            Material mat = materials[lightprobe.material];
            vec3 emission = max(vec3(0.0), mat.albedo - vec3(1.0));
            lightbleed += emission;
        }
    }
    lightbleed /= RAYS_PER_FRAGMENT * 2;
#endif
    for(int rpp = 0; rpp < RAYS_PER_FRAGMENT; rpp++) {
      vec3 color = vec3(0.0);
      campos += prand_uvec3(campos.xy,rpp)*0.001;


      cam_ray.direction = (v_position.xyz + FORWARD);
      rays[0] = cam_ray;
      int r = 0;
      float prev_eta = WORLD_REFRACTIVE_INDEX;
      for(r = 0; r < MAX_RAY_STEPS; r++) {
          hits[r] = intersect(rays[r]);
          Material mat = materials[hits[r].material];

          if(hits[r].t > EPS) {
              vec3 lift = hits[r].normal * EPS;
              
              if(mat.opacity < trand(campos.xy,r) && hits[r].type != PLANE_HIT) {
                  // vec3 refr = rays[r].direction;

                  // if(sign(dot(hits[r].normal, rays[r].direction)) < 0.0) {lift = -lift;}
                  float curr_eta = prev_eta / mat.eta;
                  vec3 refr =
                      normalize(refract(hits[r].normal,rays[r].direction, curr_eta) +
                             prand_uvec3(campos.xy,r) * mat.roughness);

                  prev_eta = mat.eta;

                      // normalize(reflect(hits[r].normal, rays[r].direction) +
                      //           prand_uvec3(campos.xy,r) * mat.roughness);
                  rays[r + 1] = Ray(hits[r].point + lift
                                    , refr);

              } else {
                  vec3 refl =
                      normalize(reflect(hits[r].normal, rays[r].direction) +
                                prand_uvec3(campos.xy,r) * mat.roughness);
                  if(dot(refl, hits[r].normal) < EPS) {refl = -refl;}
                  rays[r + 1] = Ray(hits[r].point + lift,
                                    refl);
              }
          } else break;
      }
      for(int i=r; i >= 0; i--) {
          if(hits[i].type == SKYBOX_HIT) {
              float l = (v_position.y + 1.0) * 0.5;
              color += (1.0 - l) * RED + l * STEEL_BLUE;
              color *= 0.1;
          } else {
              vec3 light = vec3(0.0);
              int lp = 0;
              // HARD LIGHT PROBLES, POINT LIGHTS
              // SOFT LIGHT PROBES
#ifdef LIGHT
              for(lp = 0; lp < LIGHT_PROBES; lp++){
                  lightprobe = intersect(Ray(hits[i].point,
                                             normalize(hits[i].normal * 1.1 +
                                                       prand_uvec3(campos.xy,r))));
                  if(lightprobe.t > 0.0) {
                      Material mat = materials[lightprobe.material];
                      vec3 emission = max(vec3(0.0), mat.albedo - vec3(1.0));
                      light += emission;
                  }
              }
              color += light / float(lp);
#endif

              Material mat = materials[hits[i].material];
              vec3 emission = max(mat.albedo - vec3(1.0), vec3(0.0));
              vec3 reflection = mat.albedo - emission;

              color *= reflection;
              color += emission;

          }
      }
      out_col += color + lightbleed;
    }
    out_col /= float(RAYS_PER_FRAGMENT);
    out_col = sqrt(out_col);

    gl_FragColor = vec4(out_col, 1.0);
}
