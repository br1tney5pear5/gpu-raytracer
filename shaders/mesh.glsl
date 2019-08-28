#module "mesh"

vec3 trinorm(vec3 p0, vec3 p1, vec3 p2) {
    vec3 u = p1 - p0;
    vec3 v = p2 - p0;
    return cross(u,v);
}
