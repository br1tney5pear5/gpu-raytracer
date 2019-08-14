__module "math"
__uses "quaternion"
__uses "random"

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

