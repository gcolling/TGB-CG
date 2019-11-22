#version 330 core

in vec3 color;
out vec4 fragColor;

void main(){
    vec3 finalColor = color * vec3(0.6f, 0.9f, 0.1f);
    fragColor = vec4(finalColor, 1.0f);
}
