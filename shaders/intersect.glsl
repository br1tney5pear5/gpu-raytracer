#module "intersect"
#uses "types"
#uses "ssbo"
#uses "mesh"
#uses "scene"
#uses "simplex"
#uses "constants"
#uses "quaternion"
#uses "rays_util"

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
