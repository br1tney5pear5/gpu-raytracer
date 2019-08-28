#module "random"
#uses "uniforms" // needs u_time

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

