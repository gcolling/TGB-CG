#version 330 core
out vec4 FragColor;

in vec2 texture;
in vec3 position;
in vec3 normal;
in vec3 oKa;
in vec3 oKd;
in vec3 oKs;
in float oNs;

vec3 lightPos  = vec3 (10.0, 25.0, 50.0);
vec3 Ls = vec3 (1.0, 1.0, 1.0); // white specular colour
vec3 Ld = vec3 (0.7, 0.7, 0.7); // dull white diffuse light colour
vec3 La = vec3 (0.2, 0.2, 0.2); // grey ambient colour

uniform sampler2D text;
uniform mat4 model;
uniform mat4 view;
uniform vec3 viewPos;

void main(){
    // Ambient
    vec3 ambient = La * oKa;
    
    // Diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - position);
    float diff = max(dot(norm,lightDir), 0.0);
    vec3 diffuse = Ld * oKd * diff;

    // Specular
    vec3 viewDir = normalize(viewPos - position);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), oNs);
    vec3 specular = Ls * spec * oKs;

    vec4 nText = texture(text, texture);
    FragColor = vec4(specular + diffuse + ambient, 1.0) * nText * nText.a;
}
