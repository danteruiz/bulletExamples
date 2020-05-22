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

std::ostream& operator<<(std::ostream& os, const glm::quat& q)
{
    os << "w: " << q.w;
    os << " x: " << q.x;
    os << " y: " << q.y;
    os << " z: " << q.z;

    return os;
}

std::ostream& operator<<(std::ostream& os, const glm::vec3& v)
{
    os << "x: " << v.x;
    os << " y: " << v.y;
    os << " z: " << v.z;

    return os;
}


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




    m_basicShapes = std::make_shared<BasicShapes>();

    m_cubeEntity.geometry = m_basicShapes->getShape(BasicShapes::CUBE);
    m_floorEntity.geometry = m_basicShapes->getShape(BasicShapes::CUBE);
    m_sphereEntity.geometry = m_basicShapes->getShape(BasicShapes::SPHERE);
    m_triangleEntity.geometry = m_basicShapes->getShape(BasicShapes::TRIANGLE);


    m_cubeEntity.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    m_sphereEntity.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    m_triangleEntity.color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

    m_floorEntity.color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    m_floorEntity.translation = glm::vec3(0.0f, -4.0f, 1.0f);
    m_floorEntity.scale = glm::vec3(9.0f, 0.1f, 9.0f);
    camera.position = glm::vec3(0.0f, 0.0f, -4.0f);


    m_light.position = glm::vec3(0.0f, 8.0f, 1.0f);

    mouse = std::make_shared<Mouse>(InputDevice::MOUSE);
    keyboard = std::make_shared<Keyboard>(InputDevice::KEYBOARD);
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
        shader->setUniformVec4("color", entity.color);

        for (auto mesh: geometry->meshes)
        {
            auto vertexBuffer = mesh.vertexBuffer;
            vertexBuffer->bind();
             vertexBuffer->setAttri(0, 3, sizeof(Vertex));

            mesh.indexBuffer->bind();
            glDrawElements(GL_TRIANGLES, (GLsizei) mesh.indices.size(), GL_UNSIGNED_INT, 0);
        }
    }
}

void DemoApplication::exec()
{
    while (!m_window->shouldClose())
    {
        mouse->update();
        updateCameraOrientation(mouse);
        updateCameraPosition(keyboard);


        glm::vec3 cameraFront = camera.orientation * UNIT_Z;
        glm::vec3 cameraUp = camera.orientation * UNIT_Y;

        glm::vec3 cameraTarget = camera.position + cameraFront;
        glm::mat4 view = glm::lookAt(camera.position, cameraTarget, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float) m_window->getWidth() / (float) m_window->getHeight(), 0.3f, 700.0f);

        RenderArgs renderArgs;
        renderArgs.view = view;
        renderArgs.projection = projection;
        renderArgs.entities = { m_floorEntity, m_sphereEntity };
        renderArgs.light = m_light;

        m_window->simpleUpdate();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f ,0.0f, 1.0f);

        renderEntities(renderArgs);
        m_window->swap();
    }
}
