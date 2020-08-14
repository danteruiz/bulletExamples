#pragma once

#include <memory>
struct Material
{
    using MaterialPointer = std::shared_ptr<Material>;
    glm::vec3 albedo { 1.0f, 1.0f, 1.0f };
    float roughness { 1.0f };
    float metallic { 1.0f };
    float opacity { 1.0f };
    float ao { 0.0f };
};
// texture map

// emissive
// albedo
// metallic
// roughness
// normal
// scattering ?
