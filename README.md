#``c++
//main.cpp

add_module("module0.glsl");
add_module("module1.frag");
add_module("module2.geom");
//...

// or

add_modules({"module0.glsl", "module1.frag", "module2.geom" /*...*/})
```

alternatively you can keep record of your modules in separate file and import it:

```
// glslmodules
module0.glsl
module1.frag
module2.geom
```

```c++
// main.cpp

import_modules_from_file("glslmodules");
```

Nothing prevents you also from importing from many different files.

Keep in mind that order in which you add your modules does not matter.
ShaderBuilder takes care of figuring out dependencies and correctly assembling final shader.
Also if you add module and its not used my root module or any of it's dependencies it's not going to be included in the built shader(s).

## Dependencies

To resolve dependencies, warn you if it detect a circular or missing one, ShaderBuilder needs every module to declare modules used by it.

```glsl
__modules "uniforms"

uniform float u_time;
```

```glsl
__modules "structs"

struct Sphere {
  vec3 position;
  float radius;
}

```

```glsl
__modules "random"
__uses "unifroms"

float rand(vec2 seed) {
    return fract(sin(dot(seed.xy, vec2(12.9898,78.233))) * 43758.5453123 + u_time);
}

```

```glsl
__modules "util"
__uses "random"
__uses "structs"

vec3 random_sphere(vec3 position, vec2 seed) {
  return Sphere(pos, rand(seed));
}
```

## Header
You might want to explicitly put something at the top of your shader, e.g. glsl version declaration. 
You can do that by setting it as the header which is always put at the top of built file.

```c++
//main.cpp

builder.set_header(
  "#ifndef GLSLVIEWER\n"
  "  #version 330\n"
  "#endif\n"
);
```

## Error Handling
This library does not throw any exceptions (though I am not sure about exception safety,
except for methods that are explicitly marked noexcept).
[...]
Unlike in standard library though overloads without error code parameter are also nonthrowing
but instead of setting your error code they just pass internal last_ec which you can lookup instead.

## Hot-Reload
