__module "quaternion"
__type "NONE"

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
