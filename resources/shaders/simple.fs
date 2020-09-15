#version 330 core

struct Light
{
    float intensity;
    float ambient;
    vec3 color;
    vec3 position;
};



uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D occlusionMap;
uniform sampler2D emissiveMap;

struct Material
{
    vec3 color;
    float roughness;
    float specular;
    float metallic;
    float ao;
    int normalTextureSet;
    int albedoTextureSet;
    int emissiveTextureSet;
    int occlusionTextureSet;
    int metallicTextureSet;
};

out vec4 FragColor;
uniform vec4 color;
uniform Light light;
uniform Material material;
uniform vec3 cameraPosition;


in vec3 vNormal;
in vec3 vPosition;
in vec2 TexCoord;
const float PI = 3.14159265359;

float NDF(float NdotH, float roughness)
{
    float alpha = roughness * roughness;
    float roughness2 = alpha * alpha;
    float f = (NdotH * roughness2 - NdotH) * NdotH + 1.0;
    return roughness2 / (PI * f * f);
}
vec3 Fensel(float VdotH, vec3 f)
{
    return f + (1.0 -f) * pow((1.0 - VdotH), 5.0);
}

float ShlickGGX(float NdotV, float roughness)
{
    float r = roughness;
    float k = (r*r) / 8.0;
    return (NdotV / ( NdotV * (1.0 - k) + k));
}

float GSmith(float NdotL, float NdotV, float roughness)
{
    float r = roughness * roughness;

    float attenuationL = 2.0 * NdotL / (NdotL + sqrt(r * r + (1.0 - r * r) * (NdotL * NdotL)));
	float attenuationV = 2.0 * NdotV / (NdotV + sqrt(r * r + (1.0 - r * r) * (NdotV * NdotV)));
	//return attenuationL * attenuationV;
    return (ShlickGGX(NdotL, roughness) * ShlickGGX(NdotV, roughness));

}

void main() {
    vec3 v = normalize(cameraPosition - vPosition);
    vec3 l = normalize(light.position - vPosition);
    vec3 h = normalize(v + l);
    vec3 n = vNormal;

    vec3 diffuseColor;
    vec3 baseColor = texture(albedoMap, TexCoord).rgb * material.color;
    float perceptualRoughness = material.roughness;
    float metallic = material.metallic;
    vec3 f0 = vec3(0.04);

    vec4 mrSample = texture(metallicMap, TexCoord);

    perceptualRoughness = mrSample.g * perceptualRoughness;
    metallic = mrSample.b * metallic;

    diffuseColor = baseColor * (vec3(1.0 - f0));
    diffuseColor *= 1.0 - metallic;

    float alphaRoughness = perceptualRoughness * perceptualRoughness;

    vec3 specularColor = mix(f0, diffuseColor, metallic);

    float cosTh = max(dot(n, h), 0.0);
    float cosTi = max(dot(n, l), 0.0);


    float NdotL = clamp(dot(n, l), 0.001, 1.0);
    float NdotV = clamp(abs(dot(n, v)), 0.001, 1.0);
    float NdotH = clamp(dot(n, h), 0.0, 1.0);
    float LdotH = clamp(dot(l, h), 0.0, 1.0);
    float VdotH = clamp(dot(v, h), 0.0, 1.0);

    float distance = length(light.position - vPosition);
    float attenuation = 1.0;

    vec3 radiance = (light.color)  * attenuation;

    float ndf = NDF(cosTh, perceptualRoughness);
    float g = GSmith(NdotL, NdotV, perceptualRoughness);
    vec3 fr = Fensel(VdotH, specularColor);


    vec3 diffuseContrib = (1.0 - fr) * (diffuseColor / PI);
    vec3 specContrib = ndf * g * fr / (4.0 * NdotL * NdotV);

    vec3 color = NdotL * light.color * (diffuseContrib + specContrib);

    //color = color / (color + vec3(1.0));
    //color = color = pow(color, vec3(1.0 / 2.2));
    //color = baseColor;
    FragColor = vec4(color, 1.0);
}
