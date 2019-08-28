#module "rays_util"
#uses "quaternion"
#uses "random"
#uses "constants"


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

