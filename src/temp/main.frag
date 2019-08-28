#module "mainfrag"

varying vec4 v_position;

void main() {
    gl_FragColor = vec4(v_position.xy, 1.0,1.0);
}
