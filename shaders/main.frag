#module "mainfrag"
#uses "types"
#uses "quaternion"
#uses "math"
#uses "ssbo"
#uses "util"
#uses "intersect"
#uses "simplex"

#type "FRAG"

out vec4 out_color;
Ray rays[MAX_RAY_STEPS + 1];
Hit hits[MAX_RAY_STEPS + 1];

Camera camera;
Ray cam_ray;

void main() {
    camera.position = vec3(0.0,0.0,-3.0);
    camera.direction = vec3(0.0,0.0,0.0);

    //LAMP
    cam_ray.origin = camera.position;


    vec3 campos = vec3(v_position.xy,0.0);

    cam_ray.direction = //TODO: change to quaternions
        // vec_to_vec_map(FORWARD, camera.direction) *
        (v_position.xyz + FORWARD);

    vec3 out_col = vec3(0.0);
    Hit lightprobe;

#if 1
    vec3 lightbleed = vec3(0.0);
#ifdef LIGHTBLEED
    for(int rpp = 0; rpp < RAYS_PER_FRAGMENT; rpp++) {
        lightprobe = intersect(Ray(camera.position, 
                                   normalize(cam_ray.direction  +
                                             prand_uvec3(campos.xy,rpp) *
                                             prand(campos.xy,rpp) *
                                             float(rpp)/float(RAYS_PER_FRAGMENT) * 0.6 
                                             )));
        if(lightprobe.t > 0.0) {
            Material mat = materials[lightprobe.material];
            vec3 emission = max(vec3(0.0), mat.albedo - vec3(1.0));
            lightbleed += emission;
        }
    }
    lightbleed /= RAYS_PER_FRAGMENT * 3;
#endif
    for(int rpp = 0; rpp < RAYS_PER_FRAGMENT; rpp++) {
      vec3 color = vec3(0.0);
      campos += prand_uvec3(campos.xy,rpp)*0.001;


      cam_ray.direction = (v_position.xyz + FORWARD);
      rays[0] = cam_ray;
      int r = 0;
      float prev_eta = WORLD_REFRACTIVE_INDEX;
      for(r = 0; r < MAX_RAY_STEPS; r++) {
          hits[r] = intersect(rays[r]);
          Material mat = materials[hits[r].material];

          if(hits[r].t > EPS) {
              vec3 lift = hits[r].normal * EPS;

              if(mat.opacity < trand(campos.xy,r) && hits[r].type != PLANE_HIT) {
                  float curr_eta = prev_eta / mat.eta;
                  vec3 refr =
                      normalize(refract(hits[r].normal,rays[r].direction, curr_eta) +
                             prand_uvec3(campos.xy,r) * mat.roughness);

                  prev_eta = mat.eta;

                  rays[r + 1] = Ray(hits[r].point + lift
                                    , refr);

              } else {
                  vec3 refl =
                      normalize(reflect(hits[r].normal, rays[r].direction) +
                                prand_uvec3(campos.xy,r) * mat.roughness);
                  if(dot(refl, hits[r].normal) < EPS) {refl = -refl;}
                  rays[r + 1] = Ray(hits[r].point + lift,
                                    refl);
              }
          } else break;
      }
      for(int i=r; i >= 0; i--) {
          if(hits[i].type == SKYBOX_HIT) {
              float l = (v_position.y + 1.0) * 0.5;
              // color += (1.0 - l) * WHITE + l * STEEL_BLUE;
              color = vec3(1.0);
              color *= 0.3;
          } else {
              vec3 light = vec3(0.0);
              int lp = 0;
              // HARD LIGHT PROBLES, POINT LIGHTS
              // SOFT LIGHT PROBES
#ifdef LIGHT
              vec3 lift = hits[r].normal * EPS;
              for(lp = 0; lp < LIGHT_PROBES; lp++){
                  lightprobe = intersect(Ray(hits[i].point + lift,
                        normalize(normalize(vec3(0.0, 1.99, 0.0) - hits[i].point) * 2.0 +
                                  prand_uvec3(campos.xy,r)
                                )));

                  if(lightprobe.t > EPS) {
                      Material hitmat = materials[hits[i].material];
                      Material mat = materials[lightprobe.material];
                      vec3 emission = max(vec3(0.0), mat.albedo - vec3(1.0));
                      light += emission * mat.roughness * (1.0/mat.eta);
                  }
              }

              // color += (light / float(lp)) * min(1.0/pow(length(color) - 0.42,2.0), 2.0);
              if(lp > 0) color += (light / float(lp)) * 0.2;//min(1.0/pow(length(color) - 0.42,2.0), 2.0);
#endif // LIGHT
              Material mat = materials[hits[i].material];
              vec3 emission = max(mat.albedo - vec3(1.0), vec3(0.0));
              vec3 reflection = mat.albedo - emission;

              color *= reflection;
              color += emission;
          }
      }
      out_col += color + lightbleed;
    }
    out_col /= float(RAYS_PER_FRAGMENT);

    // out_col = sqrt(out_col);
#else
    cam_ray.origin = vec3(0.0,0.0,-4.0);
    cam_ray.direction = (v_position.xyz + FORWARD);
    out_col = vec3(0.0);
    float val = mesh_intersect(cam_ray);
    if(val > 0.0) out_col = vec3(val/5.0,0.0,1.0);
    else out_col = vec3(0.0,0.0,1.0);
#endif

    out_color = vec4(out_col, 1.0);
}
