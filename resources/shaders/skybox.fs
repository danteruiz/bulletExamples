#version 330 core

out vec4 FragColor;
in vec3 TexCoord;
uniform sampler2D map;
//uniform samplerCube skybox;


const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 sampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec2 uv = sampleSphericalMap(normalize(TexCoord));
    FragColor = vec4(texture(map, uv).rgb, 1.0);
    //FragColor = texture(skybox, TexCoord);
}
