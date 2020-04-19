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


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>
#include <Shader.h>
#include <Window.h>
#include <Input.h>
#include <InputChannels.h>
#include <Mouse.h>
#include <Keyboard.h>


static std::string shaderPath("C:/Users/dante/code/rendering-examples/resources/shaders/");


static const std::string vertexShaderC = shaderPath + "simple.vs";
static const std::string fragmentShaderC = shaderPath + "simple.fs";
static const std::string fragmentShaderB = shaderPath + "simple2.fs";

static const std::vector<glm::vec3> triangle = {
    glm::vec3(-1.0f, -1.0f, 0.0f),
    glm::vec3(1.0f, -1.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f)
};


static std::vector<glm::vec3> vertices = {
    glm::vec3(1.0f, 1.0f, 1.0f),
    glm::vec3(1.0f, 1.0f, -1.0f),
    glm::vec3(1.0f, -1.0f, 1.0f),
    glm::vec3(1.0f, -1.0f, -1.0f),
    glm::vec3(-1.0f, 1.0f, 1.0f),
    glm::vec3(-1.0f, 1.0f, -1.0f),
    glm::vec3(-1.0f, -1.0f, 1.0f),
    glm::vec3(-1.0f, -1.0f, -1.0f)
};


static const std::vector<glm::vec3> floor_vertices = {
    glm::vec3(1.0f, -2.0f, 1.0f),
    glm::vec3(1.0f, -2.0f, -1.0f),
    glm::vec3(-1.0f, -2.0f, 1.0f),
    glm::vec3(-1.0f, -2.0f, -1.0f)
};


static const std::vector<int> floor_indices = {
    0, 3, 1,
    0, 3, 2
};

static const std::vector<int> indices = {
    0, 1, 2,
    2, 1, 3,
    0, 1, 4,
    1, 4, 5,
    4, 6, 5,
    5, 6, 7,
    0, 4, 2,
    2, 4, 6,
    2, 6, 3,
    3, 6, 7,
    1, 5, 3,
    5, 3, 2
};


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
    std::cout << "W: " << glfw::KEY_W << " S: " << glfw::KEY_S << std::endl;

    float zDirection = (keyboard->getButton(glfw::KEY_S)  * -1.0f) + keyboard->getButton(glfw::KEY_W);
    float xDirection = (keyboard->getButton(glfw::KEY_D) * -1.0f) + keyboard->getButton(glfw::KEY_A);

    glm::vec3 zOffset = (orientation * UNIT_Z) * zDirection;
    glm::vec3 xOffset = (orientation * UNIT_X) * xDirection;

    camera.position += zOffset  + xOffset;
}

std::function<void(int, int)> updateWindowSize;

static glm::vec3 const up = glm::vec3(0.0f, 1.0f, 0.0f);

GLuint VBO, VAO, EBO, vbo2;
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
	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_LINE_SMOOTH);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &vbo2);


    std::copy(vertices.begin(), vertices.end(), std::back_inserter(vertexData));
    std::copy(floor_vertices.begin(), floor_vertices.end(), std::back_inserter(vertexData));

    std::copy(indices.begin(), indices.end(), std::back_inserter(indicesData));
    std::copy(floor_indices.begin(), floor_indices.end(), std::back_inserter(indicesData));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesData.size() * sizeof(int), indicesData.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(glm::vec3), vertexData.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);


    glBindBuffer(GL_ARRAY_BUFFER, vbo2);
    glBufferData(GL_ARRAY_BUFFER, floor_vertices.size() * sizeof(glm::vec3), floor_vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    shader1 = std::make_shared<Shader>(fragmentShaderC, vertexShaderC);
    shader2 = std::make_shared<Shader>(fragmentShaderB, vertexShaderC);

    floorModel = glm::translate(floorModel, glm::vec3(0.0f, -2.0f, 0.0f));
    floorModel = glm::scale(floorModel, glm::vec3(5.0f, 0.0f, 5.0f));

    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(45.0f), glm::vec3(1.0f, 0.3f, 0.5f));
    camera.position = glm::vec3(0.0f, 0.0f, -4.0f);

    mouse = std::make_shared<Mouse>(InputDevice::MOUSE);
    keyboard = std::make_shared<Keyboard>(InputDevice::KEYBOARD);
}


void DemoApplication::exec()
{
    while (!m_window->shouldClose())
    {

        //input->pollInpout();
        mouse->update();
        updateCameraOrientation(mouse);
        updateCameraPosition(keyboard);


        std::cout << mouse->getAxis(Mouse::MOUSE_AXIS_Y) << std::endl;


        glm::vec3 cameraFront = camera.orientation * UNIT_Z;
        glm::vec3 cameraUp = camera.orientation * UNIT_Y;

        glm::vec3 cameraTarget = camera.position + cameraFront;
        glm::mat4 view = glm::lookAt(camera.position, cameraTarget, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) m_window->getWidth() / (float) m_window->getHeight(), 0.3f, 100.0f);

        m_window->simpleUpdate();

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0f, 0.0f ,0.0f, 1.0f);

        glBindBuffer(GL_ARRAY_BUFFER, vbo2);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

        shader1->bind();
        shader1->setUniformMat4("model", floorModel);
        shader1->setUniformMat4("projection", projection);
        shader1->setUniformMat4("view", view);

        size_t offset = indices.size();
        void* data = &indicesData[offset];
        int* d = static_cast<int*>(data);

        glDrawElements(GL_TRIANGLES, (GLsizei) floor_indices.size(), GL_UNSIGNED_INT, (void*) (offset * sizeof(int)));

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

        model = glm::rotate(model, glm::radians(0.5f), glm::vec3(1.0f, 0.3f, 0.5f));

        shader2->bind();
        shader2->setUniformMat4("model", model);
        shader2->setUniformMat4("projection", projection);
        shader2->setUniformMat4("view", view);
        glDrawElements(GL_TRIANGLES, (GLsizei) indices.size(), GL_UNSIGNED_INT, 0);
        m_window->swap();
    }
}
