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

#include <imgui/Imgui.h>

#include "DebugUI.h"
#include "Helper.h"

static glm::vec3 const UNIT_Z(0.0f, 0.0f, 1.0f);
static glm::vec3 const UNIT_X(1.0f, 0.0f, 0.0f);
static glm::vec3 const UNIT_Y(0.0f, 1.0f, 0.0f);


struct Camera
{
    glm::vec3 position;
    glm::quat orientation;
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
void updateCameraOrientation(std::shared_ptr<Mouse> const &mouse)
{
    if (mouse->getButton(Mouse::MOUSE_BUTTON_LEFT))
    {
        static float const sensitivity = 0.05f;
        float xOffset = mouse->getAxis(Mouse::MOUSE_AXIS_X_DELTA) * sensitivity;
        float yOffset = mouse->getAxis(Mouse::MOUSE_AXIS_Y_DELTA) * sensitivity;


        yaw += (-1.0f * xOffset);
        pitch += yOffset;

        pitch = std::clamp(pitch, -89.9f, 89.9f);

        glm::vec3 eulerAngle(pitch, yaw, 0.0f);

        camera.orientation = glm::quat(glm::radians(eulerAngle));
    }
}


void updateCameraPosition(std::shared_ptr<Keyboard> const &keyboard)
{
    glm::quat orientation = camera.orientation;

    float zDirection = (keyboard->getButton(glfw::KEY_S)  * -1.0f) + keyboard->getButton(glfw::KEY_W);
    float xDirection = (keyboard->getButton(glfw::KEY_D) * -1.0f) + keyboard->getButton(glfw::KEY_A);

    glm::vec3 zOffset = (orientation * UNIT_Z) * zDirection;
    glm::vec3 xOffset = (orientation * UNIT_X) * xDirection;

    camera.position += zOffset  + xOffset;
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
    Entity triangleEntity;
    cubeEntity.geometry = m_basicShapes->getShape(BasicShapes::CUBE);
    floorEntity.geometry = m_basicShapes->getShape(BasicShapes::CUBE);
    sphereEntity.geometry = m_basicShapes->getShape(BasicShapes::SPHERE);
    triangleEntity.geometry = m_basicShapes->getShape(BasicShapes::TRIANGLE);


    cubeEntity.name = "cube";
    sphereEntity.name = "sphere";
    floorEntity.name = "floor";
    cubeEntity.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    sphereEntity.color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    triangleEntity.color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

    floorEntity.color = glm::vec4(1.0f, 0.5f, 0.5f, 1.0f);
    floorEntity.translation = glm::vec3(0.0f, -4.0f, 1.0f);
    floorEntity.scale = glm::vec3(9.0f, 0.1f, 9.0f);
    camera.position = glm::vec3(0.0f, 0.0f, -4.0f);


    m_light.position = glm::vec3(-1.6f, 8.0f, 0.0f);
    m_light.ambient = 0.05f;
    m_light.intensity = 0.9f;
    m_light.color = glm::vec3(1.0f, 1.0f, 1.0f);

    mouse = std::make_shared<Mouse>(InputDevice::MOUSE);
    keyboard = std::make_shared<Keyboard>(InputDevice::KEYBOARD);


    m_entities = { floorEntity, sphereEntity };
}

struct RenderArgs
{
    glm::mat4 view;
    glm::mat4 projection;
    std::vector<Entity> entities;
    Light light;
};

void renderEntities(RenderArgs const &renderArgs)
{
    for (auto entity: renderArgs.entities)
    {
        auto geometry = entity.geometry;
        auto shader = geometry->shader;
        shader->bind();
        shader->setUniformMat4("model", getMatrix(entity));
        shader->setUniformMat4("projection", renderArgs.projection);
        shader->setUniformMat4("view", renderArgs.view);
        shader->setUniformVec4("material.color", entity.color);
        shader->setUniform1f("light.intensity", renderArgs.light.intensity);
        shader->setUniform1f("light.ambient", renderArgs.light.ambient);
        shader->setUniformVec3("light.color", renderArgs.light.color);
        shader->setUniformVec3("light.position", renderArgs.light.position);
        shader->setUniformVec3("cameraPosition", camera.position);
        shader->setUniform1f("material.specular", 0.8f);
        shader->setUniform1f("material.shininess", 32.0f);

        for (auto mesh: geometry->meshes)
        {
            auto vertexBuffer = mesh.vertexBuffer;
            vertexBuffer->bind();
            vertexBuffer->setAttri(0, 3, sizeof(Vertex));
            vertexBuffer->setAttri(1, 3, sizeof(Vertex), (unsigned int) offsetof(Vertex, normal));

            mesh.indexBuffer->bind();
            glDrawElements(GL_TRIANGLES, (GLsizei) mesh.indices.size(), GL_UNSIGNED_INT, 0);
        }
    }
}

void DemoApplication::exec()
{
    while (!m_window->shouldClose())
    {
        m_window->simpleUpdate();
        float f = 0.0f;
        mouse->update();
        //updateCameraOrientation(mouse);
        updateCameraPosition(keyboard);


        glm::vec3 cameraFront = camera.orientation * UNIT_Z;
        glm::vec3 cameraUp = camera.orientation * UNIT_Y;

        glm::vec3 cameraTarget = camera.position + cameraFront;
        glm::mat4 view = glm::lookAt(camera.position, cameraTarget, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float) m_window->getWidth() / (float) m_window->getHeight(), 0.3f, 700.0f);


        m_debugUI->show(m_entities, m_light);
        RenderArgs renderArgs;
        renderArgs.view = view;
        renderArgs.projection = projection;
        renderArgs.entities = m_entities;
        renderArgs.light = m_light;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f ,0.0f, 1.0f);
        renderEntities(renderArgs);
        imgui::render();
        m_window->swap();
    }
}
