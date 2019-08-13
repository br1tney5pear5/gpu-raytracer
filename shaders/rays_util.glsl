__module "rays_util"
__uses "quaternion"
__uses "random"

vec3 random_offset(vec3 v, float theta) {
    return quat_rot(v, quat(cross(v, random_unit_vec3()), theta));
}

vec3 reflect(vec3 n, vec3 v) {
    return v - 2.0 * dot(n,v) * n;
}

vec3 refract(vec3 n, vec3 v, float eta) {
    float cos_in = dot(v,n);
    float r;
    // if(cos_in < 0.0) {
    //     r = WORLD_REFRACTIVE_INDEX/eta;
    // } else {
    //     r = eta/WORLD_REFRACTIVE_INDEX;
    // }


    float discriminant = 1.0 - (1.0 - cos_in) * r * r;
    if(discriminant > 0.0){
        return r * (v + n * cos_in) - n * sqrt(discriminant);
    }
    return n;
}

float random_offset_angle = 1.0;
vec3 random_offset(vec3 v) {
    return vec3(1.0);
}

