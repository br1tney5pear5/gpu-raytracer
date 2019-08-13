__module "mainfrag"
__type "FRAG"
__uses "quaternion"
__uses "constants"
__uses "rays_util"
__uses "random"

void main() {
    vec3 scene_origin = vec3(0.0,0.0,3.0);
    camera.position = vec3(0.0, 0.8,-1.0);
    // camera.position = quat_rot(camera.position, quat(UP, u_time * 50.0))*4.0 + scene_origin;
    camera.direction = normalize(scene_origin - camera.position);

    vec3 out_color = vec3(0.14);
    spheres[0] = Sphere( vec3(-0.6, 0.2, 3.0 + sin(u_time)), 0.7, 2);
    spheres[1] = Sphere( vec3( 1.0,-1.0, 2.5), 1.0, 1);
    spheres[2] = Sphere( vec3( 2.0 * cos(u_time/2.0), 0.1, 1.0), 0.7, 2);
    spheres[3] = Sphere( vec3(2.2, 0.2, 5.0), 0.3,  2);
    spheres[4] = Sphere( vec3(-1.5,1.2, 3.0) , 0.3, 2);
    planes[0] = Plane( vec3( 0.0,-1.0, 3.0), vec3(0.0, 1.0, 0.0), 6.0, 1);
    planes[1] = Plane( vec3( 0.0, 2.7, 3.0), vec3(0.0, 0.0,-1.0), 3.0, 1);
    // planes[2] = Plane( vec3( 0.0,-1.2, 3.0), vec3(0.0,-1.0, 0.0), 3.0, 1);
    // planes[3] = Plane( vec3( 0.0,-0.0, 4.0), vec3(0.0, 0.0, 1.0), 3.0, 1);
    // material[0] = Material(0.0)
#if 1
    Hit hit;
    int iter = 0;
    float sample_radius = 0.0001;
    for(int r=0; r < RAYS_PER_FRAG; r++){

      vec3 dir =
          vec_to_vec_map(FORWARD, camera.direction) * 
          normalize(vec3(v_position.xy, 1.0));

      rays_history[0] = Ray(camera.position , random_offset(dir, rand() * sample_radius));

      while(iter < MAX_SCATTER_COUNT-1) {
          Ray ray = rays_history[iter];
          hit = intersect(ray);
          if(hit.t >= 0.0) {
              iter = iter + 1;
              vec3 scatter_dir;
              if(hit.material == 0) {
                  // scatter_dir = normalize(reflect(hit.normal, ray.direction));
                  scatter_dir = normalize(hit.normal + random_unit_vec3());
              } else if(hit.material == 1) {
                  scatter_dir = normalize(reflect(hit.normal, ray.direction));
              } else if(hit.material == 2) {
                  scatter_dir = normalize(refract(hit.normal, ray.direction, 20.8));
              }
              scatter_dir = random_offset(scatter_dir, rand() * sample_radius);
              vec3 lift = vec3(0.0);
              if( hit.material == 2 && dot(hit.normal, ray.direction) < 0.0) {
                lift = hit.normal * -EPSILON;
              }else{
                lift = hit.normal * EPSILON;
              }
              rays_history[iter] = Ray(hit.point + lift, 
                                       scatter_dir);

          } else break;
      }

      float l = abs(rays_history[iter].direction.y + -1.0);
      vec3 background = (1.0 - l ) * WHITE + l * BLUE;
      out_color = background;
      float attenuation = 1.0;
      for(int i=iter; i >= 0; i--) {
          attenuation *= 0.9;
      }
      out_color *= attenuation ;
    }
#else
    out_color = vec3(0.0);
    for(int r=0; r < RAYS_PER_FRAG; r++){
        out_color += vec3(rand(), rand(), rand());
    }
#endif

     // out_color /= float(RAYS_PER_FRAG);
     gl_FragColor = vec4(out_color, 1.0);
}
