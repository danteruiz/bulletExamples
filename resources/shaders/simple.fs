#version 330 core

struct Light
{
    float intensity;
    vec3 color;
    vec3 position;
};

out vec4 FragColor;
uniform vec4 color;
uniform Light light;

void main() {
    FragColor = color;
}
