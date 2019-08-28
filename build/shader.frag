#version 450

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
#define BOX_HIT 3

#define RAYS_PER_FRAGMENT 30
#define MAX_RAY_STEPS 5
#define MAX_RAY_LENGTH 10.0
#define LIGHT_PROBES 8
#define SCENE 1

#define LIGHT
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

mat4 rot_axis_angle( vec3 v, float angle )
{
    float s = sin( angle );
    float c = cos( angle );
    float ic = 1.0 - c;

    return mat4( v.x*v.x*ic + c,     v.y*v.x*ic - s*v.z, v.z*v.x*ic + s*v.y, 0.0,
                 v.x*v.y*ic + s*v.z, v.y*v.y*ic + c,     v.z*v.y*ic - s*v.x, 0.0,
                 v.x*v.z*ic - s*v.y, v.y*v.z*ic + s*v.x, v.z*v.z*ic + c,     0.0,
                 0.0,                0.0,                0.0,                1.0 );
}


//=============================== ssbo
layout(std430, binding = 3) buffer teapot {
    float mesh[];
};


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

//=============================== mesh
vec3 trinorm(vec3 p0, vec3 p1, vec3 p2) {
    vec3 u = p1 - p0;
    vec3 v = p2 - p0;
    return cross(u,v);
}

//=============================== scene
#if (SCENE == 0)
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
               Material(vec3(2.6,2.6,2.5), 1.0 , 1.0, 1.0), // 3.LIGHT
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
             Sphere(vec3(-2.7,-0.3,-1.0), 0.7, 5), // big right
             Sphere(vec3(-0.2,-0.6,-2.0), 0.4, 5), // medium center
             Sphere(vec3( 3.2, -0.2, 0.2), 0.8, 7),
             Sphere(vec3(-1.0,-0.5,-1.0), 0.5, 1),
             Sphere(vec3( 1.0, 0.0,-0.6), 1.0, 7)
             );

#define PLANES_COUNT 4
Plane planes[PLANES_COUNT] =
    Plane[](
            Plane(vec3(0.0,-1.0, 0.0),    normalize(vec3( 0.0, 1.0,0.0)), vec2(3.8,1.8), 0),
            Plane(vec3(0.0,-1.0, 0.0),    normalize(vec3( 0.0, 1.0,0.0)), vec2(3.8,1.8), 0),
            Plane(vec3(0.0,-1.0, 0.0),    normalize(vec3( 0.0, 1.0,0.0)), vec2(3.8,1.8), 0),
            Plane(vec3(0.0,-1.0, 0.0),    normalize(vec3( 0.0, 1.0,0.0)), vec2(3.8,1.8), 0)
            // Plane(vec3(0.0, 2.0, 3.0),    vec3( 0.0, 0.0, -1.0), vec2(3.0), 3),
            // Plane(vec3(0.0, 2.0, 3.2),    vec3( 0.0, 0.0, -1.0), vec2(4.0), 4),
            // Plane(vec3(0.0, 2.0, 3.9999), vec3( 0.0, 0.0, -1.0), vec2(2.7), 4)
            );

#define BOXES_COUNT 1
Box boxes[BOXES_COUNT] =
    Box[](Box(vec3(-1.0,1.0,0.0), vec3(1.0,1.0,1.0),
              rot_axis_angle(UP, 10)
              , 1)
          );

#elif (SCENE == 1)

// #define DIRECTIONAL_LIGHTS
#ifdef DIRECTIONAL_LIGHTS
  #define DIRECTIONAL_LIGHTS_COUNT 1
  DirectionalLight directional_lights[DIRECTIONAL_LIGHTS_COUNT] = 
      DirectionalLight[](DirectionalLight(vec3(1.0,-1.0,0.0), vec3(0.2,0.2,0.2)));
#endif

// #define POINT_LIGHTS
#ifdef DIRECTIONAL_LIGHTS
  #define POINT_LIGHTS_COUNT 1
  PointLight point_lights[POINT_LIGHTS_COUNT] = 
      PointLight[](PointLight(vec3(0.0,4.0,4.0), vec3(0.2,0.2,0.2)));
#endif


#define MATERIALS_COUNT 6
Material materials[MATERIALS_COUNT] =
    Material[](
               Material(vec3(1.0,1.0,1.0), 1.0 , 1.0, 1.0),// 0. white
               Material(vec3(0.9,0.4,0.4), 1.0 , 1.0, 1.0),// 1. red
               Material(vec3(0.4,0.4,0.9), 1.0 , 1.0, 1.0),// 2. blue
               Material(vec3(3.0,3.0,3.0), 1.0 , 1.0, 1.0),// 3. light
               Material(vec3(1.0,1.0,1.0), 0.0 , 1.0, 2.7),// 4. metal
               Material(vec3(1.0,1.0,1.0), 0.0 , 0.1, 2.8) // 5. glass
               );

#define SPHERES
#define SPHERES_COUNT 4
Sphere spheres[SPHERES_COUNT] =
    Sphere[](Sphere(vec3(-1.0,-1.5, 0.0), 0.5, 4),
             Sphere(vec3( 0.8,-1.4,-0.5), 0.6, 5),
             Sphere(vec3( -0.5,-1.8,-0.5), 0.2, 5),
             Sphere(vec3( -1.0,-1.95,-0.5), 0.05, 5)
             );

#define PLANES
#define PLANES_COUNT 6
Plane planes[PLANES_COUNT] =
    Plane[](Plane(vec3( 0.0,-2.0, 0.0), vec3( 0.0, 1.0, 0.0), vec2(2.01,2.01), 0),
            Plane(vec3( 0.0, 2.0, 0.0), vec3( 0.0,-1.0, 0.0), vec2(2.01,2.01), 0),
            Plane(vec3( 0.0, 0.0, 2.0), vec3( 0.0, 0.0,-1.0), vec2(2.01,2.01), 0),
            Plane(vec3(-2.0, 0.0, 0.0), vec3( 1.0, 0.0, 0.0), vec2(2.01,2.01), 1),
            Plane(vec3( 2.0, 0.0, 0.0), vec3(-1.0, 0.0, 0.0), vec2(2.01,2.01), 2),
            Plane(vec3( 0.0, 1.99,0.0), vec3( 0.0,-1.0, 0.0), vec2(0.51,0.51), 3)
            );

#define BOXES_COUNT 1
Box boxes[BOXES_COUNT] =
    Box[](Box(vec3(-0.5,-1.2,0.8), vec3(0.5,1.2,0.5),
              rot_axis_angle(UP, PI/3.0)
              , 0)
          );

#endif

//=============================== simplex
vec3 random3(vec3 c) {
	float j = 4096.0*sin(dot(c,vec3(17.0, 59.4, 15.0)));
	vec3 r;
	r.z = fract(512.0*j);
	j *= .125;
	r.x = fract(512.0*j);
	j *= .125;
	r.y = fract(512.0*j);
	return r-0.5;
}

const float F3 =  0.3333333;
const float G3 =  0.1666667;

float simplex3d(vec3 p) {
	 vec3 s = floor(p + dot(p, vec3(F3)));
	 vec3 x = p - s + dot(s, vec3(G3));

	 vec3 e = step(vec3(0.0), x - x.yzx);
	 vec3 i1 = e*(1.0 - e.zxy);
	 vec3 i2 = 1.0 - e.zxy*(1.0 - e);

	 vec3 x1 = x - i1 + G3;
	 vec3 x2 = x - i2 + 2.0*G3;
	 vec3 x3 = x - 1.0 + 3.0*G3;

	 vec4 w, d;

	 w.x = dot(x, x);
	 w.y = dot(x1, x1);
	 w.z = dot(x2, x2);
	 w.w = dot(x3, x3);

	 w = max(0.6 - w, 0.0);

	 d.x = dot(random3(s), x);
	 d.y = dot(random3(s + i1), x1);
	 d.z = dot(random3(s + i2), x2);
	 d.w = dot(random3(s + 1.0), x3);

	 w *= w;
	 w *= w;
	 d *= w;

	 return dot(d, vec4(52.0));
}


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
float mesh_intersect(Ray ray){
    for(int i=0; i<1800; i += 9){
        vec3 p0 = vec3(mesh[i + 0], mesh[i + 1], mesh[i + 2]);
        vec3 p1 = vec3(mesh[i + 3], mesh[i + 4], mesh[i + 5]);
        vec3 p2 = vec3(mesh[i + 6], mesh[i + 7], mesh[i + 8]);
        vec3 N = trinorm(p0, p1, p2);
        float area2 = length(N);

        float D = dot(p0, N);

        float tmesh = - (dot(N, ray.origin) + D) / dot(N, ray.direction);

        vec3 e0 = p1 - p0;
        vec3 e1 = p2 - p1;
        vec3 e2 = p0 - p2;

        vec3 P = ray.origin + tmesh * ray.direction;

        vec3 C;

        vec3 edge0 = p1 - p0;
        vec3 vp0  = P - p0;
        C = cross(edge0, vp0);
        if(dot(N,C) < 0) continue;

        vec3 edge1 = p2 - p1;
        vec3 vp1  = P - p1;
        C = cross(edge1, vp1);
        if(dot(N,C) < 0) continue;

        vec3 edge2 = p0 - p2;
        vec3 vp2  = P - p2;
        C = cross(edge2, vp2);
        if(dot(N,C) < 0) continue;

        return tmesh;
    }
    return -1.0;
}
Hit intersect(Ray ray, bool lightprobe) {
    Hit hit;
    int index = -1;
    float t = MAX_RAY_LENGTH;
    float temp_t;
#if 1
#ifdef SPHERES
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
#endif // SPHERES

#ifdef PLANES
    index = -1;
    float denominator, numerator;
    for(int i=0; i < PLANES_COUNT; i++) { 
        denominator = dot(ray.direction, planes[i].normal);
        if(denominator < 0.0) {
            numerator = dot(planes[i].position - ray.origin, planes[i].normal);
            temp_t = numerator/denominator;
            if(temp_t < t+EPS && temp_t > EPS){
              vec3 point = ray.origin + ray.direction * temp_t;


              mat3 R = vec_to_vec_map(planes[i].normal, UP);
              vec3 ps_point = abs(R * (point - planes[i].position));

                if(ps_point.x < planes[i].size.x &&
                   ps_point.z < planes[i].size.y){
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
#endif // PLANES

    index = -1;
    // float tmin, tmax, t1, t2;
    for(int i=0; i < BOXES_COUNT; i++) {

        mat4 txx = boxes[i].rotation;
        txx[3] = vec4(-boxes[i].position, 1.0);
        vec3 rdd = (txx * vec4(ray.direction, 0.0)).xyz;
        vec3 roo = (txx * vec4(ray.origin,    1.0)).xyz;

        vec3 m = 1.0/rdd;
        vec3 n = m*roo ;
        vec3 k = abs(m) * (boxes[i].size);

        vec3 t1 = -n - k;
        vec3 t2 = -n + k;

        float tN = max( max( t1.x, t1.y ), t1.z );
        float tF = min( min( t2.x, t2.y ), t2.z );

        if( tN > tF || tF < 0.0) continue;

        vec3 nor = -sign(rdd)*step(t1.yzx,t1.xyz)*step(t1.zxy,t1.xyz);

        nor = (inverse(txx) * vec4(nor, 0.0)).xyz;

        if(tN < t) {
            t = tN;
            index = i;
            hit.normal = nor;
        }
    }
    if(index != -1) {
        hit.t = t;
        hit.point = ray.origin + ray.direction * t;
        hit.material = boxes[index].material;
        hit.type = BOX_HIT;
    }
    if(t == MAX_RAY_LENGTH) {
        hit.t = -1.0;
        hit.type = SKYBOX_HIT;
        hit.material = SKYBOX_MATERIAL;
    }
#endif

    return hit;
}

//TEMP
Hit intersect(Ray ray) {return intersect(ray, false);}

//=============================== mainfrag
out vec4 out_color;
Ray rays[MAX_RAY_STEPS + 1];
Hit hits[MAX_RAY_STEPS + 1];

Camera camera;
Ray cam_ray;

void main() {
    camera.position = vec3(0.0,0.0,-3.0);
    camera.direction = vec3(0.0,0.0,0.0);

    //LAMP
    cam_ray.origin = camera.position;


    vec3 campos = vec3(v_position.xy,0.0);

    cam_ray.direction = //TODO: change to quaternions
        // vec_to_vec_map(FORWARD, camera.direction) *
        (v_position.xyz + FORWARD);

    vec3 out_col = vec3(0.0);
    Hit lightprobe;

#if 1
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
    lightbleed /= RAYS_PER_FRAGMENT * 3;
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
                  float curr_eta = prev_eta / mat.eta;
                  vec3 refr =
                      normalize(refract(hits[r].normal,rays[r].direction, curr_eta) +
                             prand_uvec3(campos.xy,r) * mat.roughness);

                  prev_eta = mat.eta;

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
              // color += (1.0 - l) * WHITE + l * STEEL_BLUE;
              color = vec3(1.0);
              color *= 0.3;
          } else {
              vec3 light = vec3(0.0);
              int lp = 0;
              // HARD LIGHT PROBLES, POINT LIGHTS
              // SOFT LIGHT PROBES
#ifdef LIGHT
              vec3 lift = hits[r].normal * EPS;
              for(lp = 0; lp < LIGHT_PROBES; lp++){
                  lightprobe = intersect(Ray(hits[i].point + lift,
                        normalize(normalize(vec3(0.0, 1.99, 0.0) - hits[i].point) * 2.0 +
                                  prand_uvec3(campos.xy,r)
                                )));

                  if(lightprobe.t > EPS) {
                      Material hitmat = materials[hits[i].material];
                      Material mat = materials[lightprobe.material];
                      vec3 emission = max(vec3(0.0), mat.albedo - vec3(1.0));
                      light += emission * mat.roughness * (1.0/mat.eta);
                  }
              }

              // color += (light / float(lp)) * min(1.0/pow(length(color) - 0.42,2.0), 2.0);
              if(lp > 0) color += (light / float(lp)) * 0.2;//min(1.0/pow(length(color) - 0.42,2.0), 2.0);
#endif // LIGHT
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

    // out_col = sqrt(out_col);
#else
    cam_ray.origin = vec3(0.0,0.0,-4.0);
    cam_ray.direction = (v_position.xyz + FORWARD);
    out_col = vec3(0.0);
    float val = mesh_intersect(cam_ray);
    if(val > 0.0) out_col = vec3(val/5.0,0.0,1.0);
    else out_col = vec3(0.0,0.0,1.0);
#endif

    out_color = vec4(out_col, 1.0);
}
