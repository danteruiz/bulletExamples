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

float V_SmithGGXCorrelated(float NdotL, float NdotV, float alphaG)
{
    float alphaG2 = alphaG * alphaG ;
    // Caution : the " NdotL *" and " NdotV *" are explicitely inversed , this is not a mistake .
    float Lambda_GGXV = NdotL * sqrt (( NdotV * alphaG2 + NdotV ) * NdotV + alphaG2 );
    float Lambda_GGXL = NdotV * sqrt (( NdotL * alphaG2 + NdotL ) * NdotL + alphaG2 );

    return 0.5f / ( Lambda_GGXV + Lambda_GGXL );
}

float D_GGX(float NdotH, float m)
{
    float m2 = m * m;
    float f = (NdotH * m2 - NdotH) * NdotH + 1;
    return m2 / (f * f);
}


float Fr_DisneyDiffuse(float NdotV, float NdotL, float LdotH, float m)
{

    float energyBias = mix(0.0, 0.5, m);
    float energyFactor = mix(1.0, 1.0 / 1.51, m);
    float fd90  = energyBias + 2.0 * LdotH * LdotH * m;
    vec3 f0 = vec3(1.0);
   // float lightScatter = F_Schlick(f0, fd90, NdotL).r;
    //float viewScatter = F_Schlick(f0, fd90, NdotV).r;

    return 0.0f;//lightScatter * viewScatter * energyFactor;
}

float NDF(float NdotH, float roughness)
{
    float alpha = roughness * roughness;
    float roughness2 = alpha * alpha;
    float f = (NdotH * roughness2 - NdotH) * NdotH + 1.0;
    return roughness2 / (PI * f * f);
}
vec3 F_Schlick(float VdotH, vec3 f)
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
    return attenuationL * attenuationV;
    //return (ShlickGGX(NdotL, roughness) * ShlickGGX(NdotV, roughness));
}


vec3 getNormal()
{
    vec3 tangentNormal = texture(normalMap, TexCoord).rgb * 2.0 - 1.0;
    vec3 q1 = dFdx(vPosition);
    vec3 q2 = dFdy(vPosition);
    vec2 st1 = dFdx(TexCoord);
    vec2 st2 = dFdy(TexCoord);

    vec3 N = normalize(vNormal);
    vec3 T = normalize(q1 * st2.t - q1 * st1.t);
    vec3 B = normalize(cross(N,T));

    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

void main() {
    vec3 v = normalize(cameraPosition - vPosition);
    vec3 l = normalize(light.position - vPosition);
    vec3 h = normalize(v + l);
    vec3 n = getNormal();
    vec3 reflection = -normalize(reflect(v, n));
    reflection.y *= -1.0;

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
    float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);
    f0 = specularColor;
    float reflectance90 = clamp(reflectance * 25.0, 0.0, 1.0);
    vec3 f90 = vec3(1.0) * reflectance90;

    float NdotL = clamp(dot(n, l), 0.001, 1.0);
    float NdotV = clamp(abs(dot(n, v)), 0.001, 1.0);
    float NdotH = clamp(dot(n, h), 0.0, 1.0);
    float LdotH = clamp(dot(l, h), 0.0, 1.0);
    float VdotH = clamp(dot(v, h), 0.0, 1.0);

    float d = length(light.position - vPosition);
    float attenuation = light.intensity / d * d;

    vec3 radiance = (light.color) * attenuation;


    //float D = D_GGX(NdotH, perceptualRoughness);
    float D = NDF(NdotH, perceptualRoughness);
    //float G = V_SmithGGXCorrelated(NdotV, NdotL, perceptualRoughness);
    float G = GSmith(NdotL, NdotV, perceptualRoughness);
    vec3 F = F_Schlick(VdotH, specularColor);

    vec3 Fd =(1.0 - F) * (diffuseColor / PI);
    vec3 Fr = D * G * F / (4.0 * NdotL * NdotV);


    vec3 color = NdotL * radiance * (Fr + Fd);

    float ao = texture(occlusionMap, TexCoord).r;
    color += mix(color, color * ao, 1.0f);

    vec3 emissive = texture(emissiveMap, TexCoord).rgb * 1.0f;
    color += emissive;

    //color = color / (color + vec3(1.0));
    //color = color = pow(color, vec3(1.0 / 2.2));
    //color = baseColor;
    FragColor = vec4(color, material.ao);
}
