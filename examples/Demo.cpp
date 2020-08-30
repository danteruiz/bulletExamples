#include "Demo.h"

#include <chrono>
#include <iostream>

#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <memory>
#include <chrono>


#include <Shader.h>
#include <Window.h>
#include <Input.h>
#include <InputChannels.h>
#include <Mouse.h>
#include <Keyboard.h>
#include <Buffer.h>
#include <GL/glew.h>
#include <BasicShapes.h>
#include <Model.h>
#include <Format.h>

#include <imgui/Imgui.h>

#include "DebugUI.h"
#include "Helper.h"

#ifdef __APPLE__
static std::string shaderPath("/Users/danteruiz/code/rendering-examples/resources/shaders/");
#else
static std::string shaderPath("C:/Users/dante/code/rendering-examples/resources/shaders/");
#endif

static const std::string vertexShader = shaderPath + "simple.vs";
static const std::string fragmentShader = shaderPath + "simple.fs";
static const std::string debugFragmentShader = shaderPath + "debug.fs";
static const std::string debugVertexShader = shaderPath + "debug.vs";
#ifdef __APPLE__
static std::string const suzanne("/Users/danteruiz/code/hifi/interface/resources/meshes/mannequin/man_stolen.fbx");
#else
static std::string const suzanne("C:/Users/dante/code/hifi-content/avatars/GrayFoxWithEyes/GrayFoxWithEyeys.fbx");
#endif

static glm::vec3 const UNIT_Z(0.0f, 0.0f, 1.0f);
static glm::vec3 const UNIT_X(1.0f, 0.0f, 0.0f);
static glm::vec3 const UNIT_Y(0.0f, 1.0f, 0.0f);

struct Camera
{
    glm::vec3 position;
    glm::quat orientation;
};


inline glm::mat4 getMatrix(Marker const &marker)
{
    glm::mat3 rot = glm::mat3_cast(marker.orientation);
    rot[0] *= marker.scale.x;
    rot[1] *= marker.scale.y;
    rot[2] *= marker.scale.z;

    glm::mat4 matrix;
    matrix[0] = glm::vec4(rot[0], 0.0f);
    matrix[1] = glm::vec4(rot[1], 0.0f);
    matrix[2] = glm::vec4(rot[2], 0.0f);
    matrix[3] = glm::vec4(marker.position, 1.0f);


    return matrix;
};


inline glm::mat4 getMatrix(Entity const &entity)
{
    glm::mat3 rot = glm::mat3_cast(entity.rotation);
    rot[0] *= entity.scale.x;
    rot[1] *= entity.scale.y;
    rot[2] *= entity.scale.z;

    glm::mat4 matrix;
    matrix[0] = glm::vec4(rot[0], 0.0f);
    matrix[1] = glm::vec4(rot[1], 0.0f);
    matrix[2] = glm::vec4(rot[2], 0.0f);
    matrix[3] = glm::vec4(entity.translation, 1.0f);


    return matrix;
}

static Camera camera;


float pitch { 0.0f };
float yaw { 0.0f };
void updateCameraOrientation(std::shared_ptr<Mouse> const &mouse, float deltaTime)
{
    if (mouse->getButton(Mouse::MOUSE_BUTTON_LEFT))
    {
        static float const sensitivity = 4.0f;
        float xOffset = mouse->getAxis(Mouse::MOUSE_AXIS_X_DELTA) * sensitivity;
        float yOffset = mouse->getAxis(Mouse::MOUSE_AXIS_Y_DELTA) * sensitivity;


        yaw += (-1.0f * xOffset) * deltaTime;
        pitch += yOffset * deltaTime;

        pitch = std::clamp(pitch, -89.9f, 89.9f);

        glm::vec3 eulerAngle(pitch, yaw, 0.0f);

        camera.orientation = glm::quat(glm::radians(eulerAngle));
    }
}


void updateCameraPosition(std::shared_ptr<Keyboard> const &keyboard, float deltaTime)
{
    glm::quat orientation = camera.orientation;

    float zDirection = (keyboard->getButton(glfw::KEY_S)  * -1.0f) + keyboard->getButton(glfw::KEY_W);
    float xDirection = (keyboard->getButton(glfw::KEY_D) * -1.0f) + keyboard->getButton(glfw::KEY_A);

    glm::vec3 zOffset = (orientation * UNIT_Z) * zDirection * 20.0f;
    glm::vec3 xOffset = (orientation * UNIT_X) * xDirection * 20.0f;

    camera.position += (zOffset  + xOffset) * deltaTime;
}

std::function<void(int, int)> updateWindowSize;

static glm::vec3 const up = glm::vec3(0.0f, 1.0f, 0.0f);

GLuint VAO;
std::vector<glm::vec3> vertexData;
std::vector<int> indicesData;

std::shared_ptr<Mouse> mouse;
std::shared_ptr<Keyboard> keyboard;

std::shared_ptr<Input> input;

std::shared_ptr<Shader> shader1;
std::shared_ptr<Shader> shader2;


glm::mat4 model;
glm::mat4 floorModel;


struct MarkerVertex
{
    MarkerVertex(glm::vec3 pos, glm::vec3 col) : position(pos), color(col) {}
    glm::vec3 position;
    glm::vec3 color;
};

DebugDraw::DebugDraw()
{
    std::vector<MarkerVertex> positions = {
        MarkerVertex(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
        MarkerVertex(glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
        MarkerVertex(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
        MarkerVertex(glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
        MarkerVertex(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        MarkerVertex(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 1.0f))
    };

    std::shared_ptr<Layout> layout = std::make_shared<Layout>();
    layout->setAttribute(0, 3, sizeof(MarkerVertex), 0);
    layout->setAttribute(1, 3, sizeof(MarkerVertex), (unsigned int) offsetof(MarkerVertex, color));
    m_vertexBuffer = std::make_shared<Buffer>(Buffer::ARRAY, positions.size() * sizeof(MarkerVertex), positions.size(), positions.data());
    m_vertexBuffer->setLayout(layout);
    m_debugPipeline = std::make_shared<Shader>(debugFragmentShader, debugVertexShader);
}

void DebugDraw::renderMarkers(std::vector<Marker> const  &markers, glm::mat4 const &view, glm::mat4 const &projection) {

    m_vertexBuffer->bind();
    m_vertexBuffer->getLayout()->enableAttributes();
    m_debugPipeline->bind();
    m_debugPipeline->setUniformMat4("view", view);
    m_debugPipeline->setUniformMat4("projection", projection);
    for (auto marker : markers)
    {
        glm::mat4 model = getMatrix(marker);
        m_debugPipeline->setUniformMat4("model", model);
        glDrawArrays(GL_LINES, 0, 6);
    }
}
DemoApplication::DemoApplication()
{
    glewExperimental = true;
    if (glewInit() != GLEW_OK)
    {
        "Failed to init glew";
    }

    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_LINE_SMOOTH);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);




    m_debugUI = std::make_shared<DebugUI>(m_window);
    m_basicShapes = std::make_shared<BasicShapes>();

    Entity cubeEntity;
    Entity floorEntity;
    Entity sphereEntity;
    Entity suzanneEntity;
    cubeEntity.model = m_basicShapes->getShape(BasicShapes::CUBE);
    floorEntity.model = m_basicShapes->getShape(BasicShapes::CUBE);
    sphereEntity.model = m_basicShapes->getShape(BasicShapes::SPHERE);
    suzanneEntity.model = loadModel(suzanne);

    cubeEntity.material = std::make_shared<Material>();
    floorEntity.material = std::make_shared<Material>();
    sphereEntity.material = std::make_shared<Material>();
    suzanneEntity.material = std::make_shared<Material>();

    cubeEntity.name = "cube";
    sphereEntity.name = "sphere";
    floorEntity.name = "floor";
    suzanneEntity.name = "suzanne";
    cubeEntity.material->albedo = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    sphereEntity.material->albedo = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    suzanneEntity.material->albedo = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

    floorEntity.material->albedo = glm::vec4(1.0f, 0.5f, 0.5f, 1.0f);
    floorEntity.translation = glm::vec3(0.0f, -4.0f, 1.0f);
    floorEntity.scale = glm::vec3(30.0f, 0.1f, 30.0f);
    camera.position = glm::vec3(0.0f, 0.0f, -4.0f);

    suzanneEntity.rotation = glm::quat(glm::radians(glm::vec3(90.0f, 0.0f, 180.0f)));


    m_light.position = glm::vec3(-1.6f, 8.0f, 0.0f);
    m_light.ambient = 0.05f;
    m_light.intensity = 0.9f;
    m_light.color = glm::vec3(1.0f, 1.0f, 1.0f);

    mouse = std::make_shared<Mouse>(InputDevice::MOUSE);
    keyboard = std::make_shared<Keyboard>(InputDevice::KEYBOARD);


    m_entities = { floorEntity, suzanneEntity };
    m_pipeline = std::make_shared<Shader>(fragmentShader, vertexShader);
    m_debugDraw = std::make_shared<DebugDraw>();
}

struct RenderArgs
{
    glm::mat4 view;
    glm::mat4 projection;
    std::vector<Entity> entities;
    Light light;
    std::shared_ptr<Shader> shader;
};

void renderEntities(RenderArgs const &renderArgs)
{

    auto shader = renderArgs.shader;
    for (auto entity: renderArgs.entities)
    {
        auto geometry = entity.model;
        auto material = entity.material;
        shader->bind();
        shader->setUniformMat4("model", getMatrix(entity));
        shader->setUniformMat4("projection", renderArgs.projection);
        shader->setUniformMat4("view", renderArgs.view);
        shader->setUniform1f("light.intensity", renderArgs.light.intensity);
        shader->setUniform1f("light.ambient", renderArgs.light.ambient);
        shader->setUniformVec3("light.color", renderArgs.light.color);
        shader->setUniformVec3("light.position", renderArgs.light.position);
        shader->setUniformVec3("cameraPosition", camera.position);
        shader->setUniformVec3("material.color", material->albedo);
        shader->setUniform1f("material.roughness", material->roughness);
        shader->setUniform1f("material.metallic", material->metallic);
        shader->setUniform1f("material.ao", material->ao);

        for (auto mesh: geometry->meshes)
        {
            auto vertexBuffer = mesh.vertexBuffer;
            vertexBuffer->bind();
            vertexBuffer->getLayout()->enableAttributes();

            mesh.indexBuffer->bind();
            glDrawElements(GL_TRIANGLES, (GLsizei) mesh.indices.size(), GL_UNSIGNED_INT, 0);
        }
    }
}




std::vector<Marker> getMarkers(RenderArgs const &renderArgs) {
    Marker light;
    light.position = renderArgs.light.position;

    std::vector<Marker> markers;
    for (auto entity : renderArgs.entities) {
        Marker entityMarker;
        entityMarker.position = entity.translation;
        entityMarker.orientation = entity.rotation;
        markers.push_back(entityMarker);
    }
    markers.push_back(light);

    return markers;
}

void DemoApplication::exec()
{
    auto currentTime = std::chrono::steady_clock::now();
    auto previousTime = currentTime;
    while (!m_window->shouldClose())
    {

        currentTime = std::chrono::steady_clock::now();


        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - previousTime);

        previousTime = currentTime;
        float deltaTime = (float) elapsed.count() / 1000.0f;
        m_window->simpleUpdate();
        float f = 0.0f;
        mouse->update();
        if (!m_debugUI->focus())
        {
            updateCameraOrientation(mouse, deltaTime);
            updateCameraPosition(keyboard, deltaTime);
        }


        glm::vec3 cameraFront = camera.orientation * UNIT_Z;
        glm::vec3 cameraUp = camera.orientation * UNIT_Y;

        glm::vec3 cameraTarget = camera.position + cameraFront;
        glm::mat4 view = glm::lookAt(camera.position, cameraTarget, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float) m_window->getWidth() / (float) m_window->getHeight(), 0.3f, 700.0f);


        m_debugUI->show(m_entities, m_light, [&] {
            m_pipeline = std::make_shared<Shader> (fragmentShader, vertexShader);
        });
        RenderArgs renderArgs;
        renderArgs.view = view;
        renderArgs.projection = projection;
        renderArgs.entities = m_entities;
        renderArgs.light = m_light;
        renderArgs.shader = m_pipeline;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f ,0.0f, 1.0f);
        renderEntities(renderArgs);
        m_debugDraw->renderMarkers(getMarkers(renderArgs), view, projection);
        imgui::render();
        m_window->swap();
    }
}
