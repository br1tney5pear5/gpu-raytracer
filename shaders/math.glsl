#module "math"
#uses "quaternion"
#uses "random"

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

