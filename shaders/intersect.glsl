__module "intersect"
__uses "types"
__uses "scene"
__uses "constants"
__uses "quaternion"
__uses "rays_util"

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
