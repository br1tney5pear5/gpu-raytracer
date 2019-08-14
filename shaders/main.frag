__module "mainfrag"
__uses "types"
__uses "quaternion"
__uses "math"
__uses "colors"
__uses "util"
__uses "intersect"

__type "FRAG"



Ray rays[MAX_RAY_STEPS + 1];
Hit hits[MAX_RAY_STEPS + 1];

Camera camera;
Ray cam_ray;

void main() {
    spheres[0].position.x = sin(u_time) * 4.0;
    spheres[0].position.y = cos(u_time) * 4.0;

    spheres[1].position.z = 1.0 + sin(u_time);
    camera.position = vec3(0.0,1.0,-5.0);
    camera.direction = vec3(0.0,0.0,0.0);

    //LAMP
    
    point_lights[0].position.z =cos(u_time);

    cam_ray.origin = camera.position;


    vec3 campos = vec3(v_position.xy,0.0);

    cam_ray.direction = //TODO: change to quaternions
        // vec_to_vec_map(FORWARD, camera.direction) *
        (v_position.xyz + FORWARD);

    vec3 out_col = vec3(0.0);
    for(int rpp = 0; rpp < RAYS_PER_FRAGMENT; rpp++) {
      vec3 color = vec3(0.0);
      campos += prand_uvec3(campos.xy,rpp)*0.01;


      cam_ray.direction = (v_position.xyz + FORWARD);
      rays[0] = cam_ray;
      int r = 0;
      for(r = 0; r < MAX_RAY_STEPS; r++) {
          hits[r] = intersect(rays[r]);
          Material mat = materials[hits[r].material];

          if(hits[r].t > 0.0) {
              vec3 ref =
                  normalize(reflect(hits[r].normal, rays[r].direction) +
                            prand_uvec3(campos.xy,r) * mat.roughness);
              if(dot(ref, hits[r].normal) < EPS) {ref = -ref;}

              rays[r + 1] = Ray(hits[r].point, ref);
          } else break;
      }
      for(int i=r; i >= 0; i--) {
          if(hits[i].type == SKYBOX_HIT) {
              float l = (v_position.y + 1.0) * 0.5;
              color += (1.0 - l) * SALMON + l * STEEL_BLUE;
              color *= 0.8;
          } else {

              Hit lightprobe;
              for(int pl = 0; pl < POINT_LIGHTS_COUNT; pl++){
                  lightprobe =
                      intersect(Ray(hits[i].point,
                                    point_lights[0].position - hits[i].point
                                    ));
                  if(lightprobe.t == -1.0) {color += point_lights[0].emission;}
              }
              Material mat = materials[hits[i].material];
              vec3 emission = max(mat.albedo - vec3(1.0), vec3(0.0));
              vec3 reflection = mat.albedo - emission;

              color *= reflection;
              color += emission;

          }
      }
      out_col += color;
    }
    out_col /= float(RAYS_PER_FRAGMENT);
    // out_col = sqrt(out_col);

    gl_FragColor = vec4(out_col, 1.0);
}
