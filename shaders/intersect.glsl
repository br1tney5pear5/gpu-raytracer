__module "intersect"
__uses "types"
__uses "scene"
__uses "constants"
__uses "rays_util"

Hit intersect(Ray ray) {
    Hit hit;
    int index;
    float t = MAX_RAY_LENGTH;

    for(int i=0; i < SPHERES_COUNT; i++) { // 2 cancels
        vec3 origin_diff = ray.origin - spheres[i].position;
        float a = dot(ray.direction, ray.direction);
        float b = dot(origin_diff, ray.direction);
        float c = dot(origin_diff, origin_diff) - pow(spheres[i].radius, 2.0);
        float delta = b*b - a * c;
        if(delta > 0.0) {
            float sqrt_delta = sqrt(delta); // reuse c
            float t1 = (-b + sqrt_delta) / a;
            float t2 = (-b - sqrt_delta) / a;

            if(t1 < 0.0) t1 = MAX_RAY_LENGTH;
            if(t2 < 0.0) t2 = MAX_RAY_LENGTH;

            float temp_t = min(t1, t2); //reuse t1

            if(temp_t < t) {
                t = temp_t;
                index = i;
            }
        }
    }
    if(t != MAX_RAY_LENGTH) {
        hit.t = t;
        hit.point = ray.origin + ray.direction * t;
        hit.normal = normalize(hit.point - spheres[index].position);
        hit.point += hit.normal * EPS;
        hit.material = spheres[index].material;
        hit.type = SPHERE_HIT;
    }

    for(int i=0; i < PLANES_COUNT; i++) { // 2 cancels

    }

    if(t == MAX_RAY_LENGTH) {
        hit.t = -1.0;
        hit.type = SKYBOX_HIT;
    }
    return hit;

}
