
__module "util"

vec3 split(vec3 a, vec3 b, bool trigger) {
    if(!trigger) return a;
    if(v_position.x >
       0.1*sin(u_time + v_position.y)
       ) {return a;}
    else {return b;}
}

vec3 split(vec3 a, vec3 b) {
    return split(a,b,true);
}
