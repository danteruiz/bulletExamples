#include "DebugUI.h"

#include "Entity.h"
#include "Helper.h"
#include <imgui/Imgui.h>
#include <imgui.h>
#include <Window.h>

DebugUI::DebugUI(std::shared_ptr<Window> const &window)
{
    imgui::initialize(window->getWindowPtr());
}


DebugUI::~DebugUI()
{
    imgui::uninitialize();
}

void DebugUI::show(std::vector<Entity> &entities, Light &light, std::function<void()> compileShader)
{
    m_lightColor[0] = light.color.x;
    m_lightColor[1] = light.color.y;
    m_lightColor[2] = light.color.z;

    m_focus = ImGui::GetIO().WantCaptureMouse;
    imgui::newFrame();
    ImGui::Begin("DebugUI");
    if (ImGui::Button("Compile Shader"))
    {
        compileShader();
    }
    ImGui::Separator();
    ImGui::Text("Light");
    ImGui::Separator();
    ImGui::SliderFloat("ambient", &light.ambient, 0.0f, 1.0f);
    ImGui::SliderFloat("intensity", &light.intensity, 0.0f, 400.0f);
    ImGui::SliderFloat("position.x", &light.position.x, -50.0f, 50.0f);
    ImGui::SliderFloat("position.y", &light.position.y, -50.0f, 50.0f);
    ImGui::SliderFloat("position.z", &light.position.z, -50.0f, 50.0f);
    ImGui::ColorEdit3("color", m_lightColor);


    light.color.x = m_lightColor[0];
    light.color.y = m_lightColor[1];
    light.color.z = m_lightColor[2];


    ImGui::Separator();

    std::vector<std::string> entityNames;

    for (auto entity: entities)
    {
        entityNames.push_back(entity.name);
    }
    ImGui::Separator();

    imgui::ListBox("Entities", &m_entityIndex, entityNames);

    ImGui::Separator();
    ImGui::Text("Edit Entity");

    Entity &entity = entities[m_entityIndex];
    float entityColor[4] = { entity.color.x, entity.color.y, entity.color.z, entity.color.w};
    ImGui::ColorEdit4("entity color", entityColor);
    entity.color.x = entityColor[0];
    entity.color.y = entityColor[1];
    entity.color.z = entityColor[2];
    entity.color.w = entityColor[3];

    ImGui::SliderFloat("specular", &entity.specular, 0.0f, 1.0f);
    ImGui::SliderFloat("roughness", &entity.roughness, 0.0f, 1.0f);
    ImGui::SliderFloat("metallic", &entity.metallic, 0.0f, 1.0f);
    ImGui::End();
};

bool DebugUI::focus()
{
    return m_focus;
}
