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
#include <GLFW/glfw3.h>
#include <Input.h>
#include <Mouse.h>
#include "Demo.h"


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

std::function<void(int, int)> updateWindowSize;

static const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

static const glm::vec3 UNIT_Z(0.0f, 0.0f, 1.0f);
static const glm::vec3 UNIT_X(1.0f, 0.0f, 0.0f);
static const glm::vec3 UNIT_Y(0.0f, 1.0f, 0.0f);

struct Camera
{
    glm::vec3 position;
    glm::quat orientation;
};


static Camera camera;




void updateCameraPosition(glm::vec2 const &v)
{

    glm::quat orientation = camera.orientation;

    glm::vec3 zOffset = (orientation * UNIT_Z) * v.y;
    glm::vec3 xOffset = (orientation * UNIT_X) * v.x;

    camera.position += zOffset + xOffset;
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key)
        {
            case GLFW_KEY_W:
                updateCameraPosition({0.0f, 1.0f});
                break;

            case GLFW_KEY_S:
                updateCameraPosition({0.0f, -1.0f});
                break;

            case GLFW_KEY_A:
                updateCameraPosition({1.0f, 0.0f});
                break;

            case GLFW_KEY_D:
                updateCameraPosition({-1.0f, 0.0f});
                break;

            default:
                break;
        }
    }
}

float yaw = 0;
float pitch = 0;


void updateCameraOrientation(std::shared_ptr<Mouse> const &mouse)
{
    if (mouse->getButton(GLFW_MOUSE_BUTTON_RIGHT))
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

void onWindowSizeChanged(GLFWwindow* window, int width, int height)
{
    updateWindowSize(height, width);
    glViewport(0, 0, width, height);
}


int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{



    DemoApplication demoApplication;
    glewExperimental = true;
    Window window(500, 500, "Demo");


    if (!window.createWindow())
    {
        std::cout << "Failed to create window" << "\n";
        return -1;
    }


    if (glewInit() != GLEW_OK)
	{
		// error for glew
		std::cout << "glew did not init correctly" << std::endl;
        return -1;
    }


    std::shared_ptr<Mouse> mouse = std::make_shared<Mouse>(InputDevice::MOUSE);

    updateWindowSize = [&window] (int height, int width) {
        GLFWwindow* glfwWindow = window.getWindowPtr();
        window.setHeight(height);
        window.setWidth(width);
    };

    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_LINE_SMOOTH);

    glfwSetKeyCallback(window.getWindowPtr(), keyCallback);
    glfwSetWindowSizeCallback(window.getWindowPtr(), onWindowSizeChanged);

    GLuint VBO, VAO, EBO, vbo2;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &vbo2);

    Input input;


    std::vector<glm::vec3> vertexData;
    std::vector<int> indicesData;

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

    Shader shader(fragmentShaderC, vertexShaderC);
    Shader floorShader(fragmentShaderB, vertexShaderC);


    glm::mat4 model;

    glm::mat4 floorModel;

    floorModel = glm::translate(floorModel, glm::vec3(0.0f, -2.0f, 0.0f));
    floorModel = glm::scale(floorModel, glm::vec3(5.0f, 0.0f, 5.0f));

    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(45.0f), glm::vec3(1.0f, 0.3f, 0.5f));
    camera.position = glm::vec3(0.0f, 0.0f, -4.0f);

    while (!window.shouldClose())
    {

        input.pollInput();

        mouse->mouseUpdate(window.getWindowPtr());
        updateCameraOrientation(mouse);

        glm::vec3 cameraFront = camera.orientation * UNIT_Z;
        glm::vec3 cameraUp = camera.orientation * UNIT_Y;

        glm::vec3 cameraTarget = camera.position + cameraFront;
        glm::mat4 view = glm::lookAt(camera.position, cameraTarget, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) window.getWidth() / (float) window.getHeight(), 0.3f, 100.0f);
        window.simpleUpdate();
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0f, 0.0f ,0.0f, 1.0f);

        glBindBuffer(GL_ARRAY_BUFFER, vbo2);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);


        floorShader.bind();
        floorShader.setUniformMat4("model", floorModel);
        floorShader.setUniformMat4("projection", projection);
        floorShader.setUniformMat4("view", view);

        size_t offset = indices.size();
        void* data = &indicesData[offset];
        int* d = static_cast<int*>(data);

        glDrawElements(GL_TRIANGLES, (GLsizei) floor_indices.size(), GL_UNSIGNED_INT, (void*) (offset * sizeof(int)));

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

        model = glm::rotate(model, glm::radians(0.5f), glm::vec3(1.0f, 0.3f, 0.5f));

        shader.bind();
        shader.setUniformMat4("model", model);
        shader.setUniformMat4("projection", projection);
        shader.setUniformMat4("view", view);
        glDrawElements(GL_TRIANGLES, (GLsizei) indices.size(), GL_UNSIGNED_INT, 0);
        window.swap();
    }
    return 0;
}
