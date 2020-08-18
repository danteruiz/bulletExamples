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
    vec3 color;
    float roughness;
    float specular;
    float metallic;
    float ao;
};

out vec4 FragColor;
uniform vec4 color;
uniform Light light;
uniform Material material;
uniform vec3 cameraPosition;


in vec3 vNormal;
in vec3 vPosition;
const float PI = 3.14159265359;
float NDF(float cosTheta, float roughness)
{
    float roughness2 = roughness * roughness;
    float cosTheta2 = cosTheta * cosTheta;
    float denom = (cosTheta2 * ( roughness2 - 1.0) + 1.0);
    return cosTheta2 / (max(PI * ( denom * denom), 0.001));
}
vec3  Fensel(float cosTheta, vec3 f)
{
    return f + (1.0 -f) * pow((1.0 - cosTheta), 5.0);
}

float ShlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;
    return (NdotV / ( NdotV * (1.0 - k) + k));
}

float GSmith(vec3 n, vec3 v, vec3 l, float roughness)
{
    float NdotL = max(dot(n, l), 0.0);
    float NdotV = max(dot(n, v), 0.0);

    return (ShlickGGX(NdotL, roughness) * ShlickGGX(NdotV, roughness));

}





void main() {
    vec3 v = normalize(cameraPosition - vPosition);
    vec3 l = normalize(light.position - vPosition);
    vec3 h = normalize(v + l);

    float cosTh = max(dot(vNormal, h), 0.0);
    float cosTi = max(dot(vNormal, l), 0.0);

    float distance = length(light.position - vPosition);
    float attenuation = 1.0;

   /* vec3 kd = material.color.xyz;
    vec3 ambient = light.ambient * kd;
    vec3 ks = kd * material.specular;

    vec3 el = (light.color * light.intensity) * attenuation;

    //vec3 lo = (kd + el) * cosTi;

    vec3 lo = (kd + ks * pow(cosTh, material.shininess)) * (el * cosTi);
    FragColor = vec4(lo, 1.0);*/

    vec3 n = vNormal;
    vec3 radiance = (light.color)  * attenuation;
    vec3 fo = vec3(0.04);
    fo = mix(fo, material.color.rgb, material.metallic);
    float ndf = NDF(cosTh, material.roughness);
    float g = GSmith(n, v, l, material.roughness);
    vec3 fr = Fensel(clamp(dot(h,v), 0.0, 1.0), fo);

    vec3 numerator = ndf * g * fr;
    float denominator = 4.0 * max(dot(n, v), 0.0) * max(dot(n,l), 0.0);
    vec3 specular = numerator / max(denominator, 0.0001);


    vec3 ks = fr;
    vec3 kd = vec3(1.0) - ks;
    kd *= 1.0 - material.metallic;


    vec3 lo = (kd * material.color.rgb / PI + specular) * radiance * cosTi;
    vec3 ambient = material.color.rgb * light.ambient * material.ao;
    vec3 color = ambient + lo;

    //color = color / (color + vec3(1.0));
    color = color = pow(color, vec3(1.0 / 2.2));
    FragColor = vec4(color, 1.0);
}
