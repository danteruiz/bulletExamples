#version 330 core

struct Light
{
    float intensity;
    float ambient;
    vec3 color;
    vec3 position;
};


struct Material
{
    vec4 color;
    float shininess;
    float specular;
};

out vec4 FragColor;
uniform vec4 color;
uniform Light light;
uniform Material material;
uniform vec3 cameraPosition;


in vec3 vNormal;
in vec3 vPosition;
void main() {
    vec3 viewDirection = normalize(cameraPosition - vPosition);
    vec3 lightDirection = normalize(light.position - vPosition);

    vec3 h = normalize(viewDirection + lightDirection);

    float cosTh = max(dot(vNormal, h), 0.0);
    float cosTi = max(dot(vNormal, lightDirection), 0.0);

    vec3 kd = material.color.xyz;
    vec3 ks = kd * material.specular;

    vec3 el = light.color * light.intensity;
    vec3 lo = ((kd + ks * pow(cosTh, material.shininess))) * el * cosTi;

    FragColor = vec4(lo, 1.0);
    //FragColor = material.color;

}
