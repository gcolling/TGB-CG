#version 330 core
layout (location = 0) in vec3 vp;
layout (location = 1) in vec2 vt;
layout (location = 2) in vec3 vn;

uniform mat4 view, projection, model;
uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform float Ns;

out vec2 texture;
out vec3 position;
out vec3 normal;
out vec3 oKa;
out vec3 oKd;
out vec3 oKs;
out float oNs;

void main(){
    texture = vt;
    position = vp;
    normal = vn;
    oKa = Ka;
    oKd = Kd;
    oKs = Ks;
    oNs = Ns;

    normal = mat3(transpose(inverse(model))) * vn;
    gl_Position = projection * view * model * vec4(vp, 1.0);
}
