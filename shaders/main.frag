__module "mainfrag"
__uses "types"
__uses "quaternion"
__uses "math"
__uses "colors"
__uses "util"

__type "FRAG"



Ray rays_history[MAX_RAY_STEPS];
Hit hits_history[MAX_RAY_STEPS];

Camera camera;
Ray cam_ray;

void main() {
    camera.position = vec3(0.0,0.0,-2.0);
    camera.direction = FORWARD;

    cam_ray.origin = camera.position;
    cam_ray.direction =
        vec_to_vec_map(FORWARD, camera.direction) * (FORWARD + v_position.xyz);

    Hit hit;



    vec3 out_col;
    out_col = vec3(v_position.xyz);
    float l = (v_position.y + 1.0) * 0.5;
    out_col = split((1.0 - l) * SALMON + l * STEEL_BLUE,
                    SALMON,
                    false);

    gl_FragColor = vec4(out_col, 1.0);
}
