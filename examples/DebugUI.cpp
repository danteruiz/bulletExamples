#include "DebugUI.h"

#include "Entity.h"
#include "Helper.h"

#include <imgui/Imgui.h>
#include <imgui.h>
#include <Window.h>
#include <Material.h>
#include <Model.h>
#include <ModelPaths.h>

DebugUI::DebugUI(std::shared_ptr<Window> const &window)
{
    imgui::initialize(window->getWindowPtr());

#ifdef __APPLE__
    m_path = "/Users/danteruiz/code/glTF-Sample-Models/2.0/DamagedHelmet/glTF/DamagedHelmet.gltf";
#else
    m_path = "C:/Users/dante/code/glTF-Sample-Models/2.0/DamagedHelmet/glTF/DamagedHelmet.gltf";
#endif
    m_path.resize(3000);
}


DebugUI::~DebugUI()
{
    imgui::uninitialize();
}

void DebugUI::show(Entity &entity, Light &light, std::function<void()> compileShader, std::function<void(std::string, bool)> loadNewModel)
{
    m_lightColor[0] = light.color.x;
    m_lightColor[1] = light.color.y;
    m_lightColor[2] = light.color.z;

    m_focus = ImGui::GetIO().WantCaptureMouse;
    imgui::newFrame();
    ImGui::Begin("DebugUI");

    if (ImGui::Button("Recompile PBR Shader")) {
        compileShader();
    }

    ImGui::Separator();
    ImGui::Text("Camera");
    ImGui::Separator();

    ImGui::Text("Light");
    ImGui::Separator();
    //ImGui::SliderFloat("ambient", &light.ambient, 0.0f, 1.0f);
    //ImGui::SliderFloat("intensity", &light.intensity, 0.0f, 6.0f);
    ImGui::SliderFloat("position.x", &light.position.x, -50.0f, 50.0f);
    ImGui::SliderFloat("position.y", &light.position.y, -50.0f, 50.0f);
    ImGui::SliderFloat("position.z", &light.position.z, -50.0f, 50.0f);
    ImGui::ColorEdit3("color", m_lightColor);


    light.color.x = m_lightColor[0];
    light.color.y = m_lightColor[1];
    light.color.z = m_lightColor[2];


    ImGui::Separator();

    ImGui::Text("Entity");
    if (ImGui::RadioButton("Sphere Entity", !m_useModel)) {
        m_useModel = false;
        loadNewModel(gltfModels::getModelPath(m_modelCurrentIndex), m_useModel);
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Model Entity", m_useModel))
    {
        m_useModel = true;
        loadNewModel(gltfModels::getModelPath(m_modelCurrentIndex), m_useModel);
    }


    if (m_useModel)
    {
        std::vector<std::string> modelNames = gltfModels::getModelNames();
        if (imgui::Combo("glTF Models", &m_modelCurrentIndex, modelNames))
        {
            loadNewModel(gltfModels::getModelPath(m_modelCurrentIndex), m_useModel);
        }
    }

    ImGui::Separator();
    ImGui::Text("Edit Entity Scale");

    float scale = entity.scale.x;
    ImGui::SliderFloat("Entity Scale", &scale, 0.0f, 30.0f);

    entity.scale.x = scale;
    entity.scale.y = scale;
    entity.scale.z = scale;
    ImGui::Separator();
    ImGui::Text("Edit Entity Material");


    if (entity.model) {
        auto &material = entity.model->meshes[0].material;
        auto color = material->albedo;
        float entityColor[3] = { color.x, color.y, color.z};
        ImGui::ColorEdit3("entity color", entityColor);
        material->albedo.x = entityColor[0];
        material->albedo.y = entityColor[1];
        material->albedo.z = entityColor[2];

        ImGui::SliderFloat("roughness", &material->roughness, 0.04f, 1.0f);
        ImGui::SliderFloat("metallic", &material->metallic, 0.0f, 1.0f);
        ImGui::SliderFloat("ao", &material->ao, 0.0f, 1.0f);
    }
    ImGui::End();
};

bool DebugUI::focus()
{
    return m_focus;
}
