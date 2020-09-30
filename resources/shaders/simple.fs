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
uniform sampler2D brdfLut;
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;

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


struct PBRInfo
{
    vec3 baseColor;
    vec3 albedoColor;
    vec3 f0;
    vec3 f90;
    float metallic;
    float perceptualRoughness;
    float alphaRoughness;
};

float V_SmithGGXCorrelated(float NdotL, float NdotV, float alphaG)
{
    float alphaG2 = alphaG * alphaG ;
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
    float f = (NdotH * NdotH) * (roughness2 - 1.0) + 1.0;
    return roughness2 / (PI * f * f);
}
vec3 F_Schlick(float VdotH, vec3 r0, vec3 f90)
{
    return r0 + (1.0 - r0) * pow((1.0 - VdotH), 5.0);
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
    vec3 B = -normalize(cross(N,T));

    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

void main() {
    vec3 v = normalize(cameraPosition - vPosition);
    vec3 l = normalize(light.position - vPosition);
    vec3 h = normalize(v + l);
    vec3 n = getNormal();
    vec3 reflection = -reflect(v, n);

    vec3 f0 = vec3(0.04);
    PBRInfo pbrInfo;
    pbrInfo.baseColor = texture(albedoMap, TexCoord).rgb * material.color;
    pbrInfo.perceptualRoughness = material.roughness;
    pbrInfo.metallic = material.metallic;

    vec4 mrSample = texture(metallicMap, TexCoord);
    pbrInfo.perceptualRoughness *= mrSample.g;
    pbrInfo.metallic *= mrSample.b;


    pbrInfo.albedoColor = pbrInfo.baseColor * (vec3(1.0) - f0);
    pbrInfo.albedoColor *= 1.0 - pbrInfo.metallic;
    pbrInfo.f0 = mix(f0, pbrInfo.baseColor.rgb, pbrInfo.metallic);

    pbrInfo.alphaRoughness = pbrInfo.perceptualRoughness * pbrInfo.perceptualRoughness;

    float reflectance = max(max(pbrInfo.f0.r, pbrInfo.f0.g), pbrInfo.f0.b);
    pbrInfo.f90 = vec3(clamp(reflectance * 50.0, 0.0, 1.0));

    float NdotL = clamp(dot(n, l), 0.001, 1.0);
    float NdotV = clamp(abs(dot(n, v)), 0.001, 1.0);
    float NdotH = clamp(dot(n, h), 0.0, 1.0);
    float LdotH = clamp(dot(l, h), 0.0, 1.0);
    float VdotH = clamp(dot(v, h), 0.0, 1.0);

    float d = length(light.position - vPosition);
    float attenuation = 1.0 / d * d;

    vec3 radiance = light.color;// * attenuation;


    float D = NDF(NdotH, pbrInfo.perceptualRoughness);
    float G = GSmith(NdotL, NdotV, pbrInfo.perceptualRoughness);
    vec3 F = F_Schlick(VdotH, pbrInfo.f0, pbrInfo.f90);


    vec3 irradiance = texture(irradianceMap, n).rgb;
    vec3 Fd =(1.0 - F) * (pbrInfo.albedoColor / PI);
    //Fd *= 1.0 - pbrInfo.metallic;
    vec3 Fr = D * G * F / (4.0 * NdotL * NdotV);


    vec3 color = NdotL * radiance * (Fr + Fd);

    vec3 specularLight = textureLod(prefilterMap, reflection, pbrInfo.perceptualRoughness * 4.0).rgb;
    vec2 brdf = texture(brdfLut, vec2(NdotV, pbrInfo.perceptualRoughness)).rg;

    vec3 specular = specularLight * (F * brdf.x + brdf.y);
    color += irradiance * pbrInfo.albedoColor;
    //color += vec3(0.1) * material.color;
    color += specular;
    float ao = texture(occlusionMap, TexCoord).r;
    color += mix(color, color * ao, 1.0f);

    vec3 emissive = texture(emissiveMap, TexCoord).rgb * 1.0;
    color += emissive;

    //color = color / (color + vec3(1.0));
    //color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, material.ao);
}
