const float PI = 3.14159265359;
float V_SmithGGXCorrelated(float NdotL, float NdotV, float alphaG)
{
    float alphaG2 = alphaG * alphaG;
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
    return r0 + (f90 - r0) * pow((1.0 - VdotH), 5.0);
}

vec3 F_Schlick2(float VdotH, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - VdotH, 5.0);
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
