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
#define _USE_MATH_DEFINES

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
#include <Texture.h>
#include <math.h>
#include <ModelPaths.h>
#include <imgui/Imgui.h>

#include "DebugUI.h"
#include "Helper.h"



static std::string resources = RESOURCE_PATH;
static const std::string shaderPath = std::string(RESOURCE_PATH) + "shaders/";
static const std::string vertexShader = shaderPath + "pbr.vs";
static const std::string fragmentShader = shaderPath + "pbr.fs";
static const std::string debugFragmentShader = shaderPath + "debug.fs";
static const std::string debugVertexShader = shaderPath + "debug.vs";
static const std::string SKYBOX_FRAG = shaderPath + "skybox.fs";
static const std::string SKYBOX_VERT = shaderPath + "skybox.vs";
static const std::string CONVERT_TO_CUBE_MAP = shaderPath + "convertToCubeMap.fs";
static const std::string IRRADIANCE_CONVOLUTION = shaderPath + "irradianceConvolution.fs";
static const std::string FILTER_MAP = shaderPath + "prefilterMap.fs";
static const std::string BRDF_FRAG = shaderPath + "brdf.fs";
static const std::string BRDF_VERT = shaderPath + "brdf.vs";

static glm::vec3 const UNIT_Z(0.0f, 0.0f, 1.0f);
static glm::vec3 const UNIT_X(1.0f, 0.0f, 0.0f);
static glm::vec3 const UNIT_Y(0.0f, 1.0f, 0.0f);


static float const TWO_PI = 2 * M_PI;


static std::string const IBLTexturePath = resources + "images/IBL/TropicalBeach/Tropical_Beach_3k.hdr";
std::shared_ptr<Texture> IBLTexture;


unsigned int captureFBO, captureRBO, envCubemap, irradianceMap;


unsigned int prefilterMap, brdfLUTTexture;


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



float yawOffset = 0.0f;
void rotateCameraAroundEntity(Entity const &entity, float deltaTime)
{
    static float const YAW_SPEED = TWO_PI;

    yawOffset +=  YAW_SPEED * deltaTime;

    camera.orientation = glm::quat(glm::radians(glm::vec3(0.0f, yawOffset, 0.0f)));
    camera.position = entity.translation + ((camera.orientation * UNIT_Z) * -4.0f);
}
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

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);


    m_debugUI = std::make_shared<DebugUI>(m_window);
    m_basicShapes = std::make_shared<BasicShapes>();
    camera.position = glm::vec3(0.0f, 0.0f, -4.0f);

    // setting up model entity 90.0f, 180.0f
    m_modelEntity.rotation = glm::quat(glm::radians(glm::vec3(90.0f, 180.0f, 0.0)));
    //m_modelEntity.model = m_basicShapes->getShape(BasicShapes::SPHERE);
    m_modelEntity.model = loadModel(m_debugUI->getModelPath());

    // setting up skybox
    //m_skybox.texture = loadCubeMap(CUBE_MAP_IMAGES);
    m_skybox.shader = std::make_shared<Shader>(SKYBOX_FRAG, SKYBOX_VERT);
    m_skybox.model = m_basicShapes->getShape(BasicShapes::CUBE);


    auto quad = m_basicShapes->getShape(BasicShapes::QUAD);

    IBLTexture = loadIBLTexture(IBLTexturePath);

    // setting up lighting
    m_light.position = glm::vec3(-1.6f, 8.0f, 0.0f);
    m_light.ambient = 0.05f;
    m_light.intensity = 0.9f;
    m_light.color = glm::vec3(1.0f, 1.0f, 1.0f);

    mouse = std::make_shared<Mouse>(InputDevice::MOUSE);
    keyboard = std::make_shared<Keyboard>(InputDevice::KEYBOARD);


    // setup debug rendering objects
    m_pipeline = std::make_shared<Shader>(fragmentShader, vertexShader);
    m_debugDraw = std::make_shared<DebugDraw>();

    m_convertToCubeMap = std::make_shared<Shader>(CONVERT_TO_CUBE_MAP, SKYBOX_VERT);
    m_irradiance = std::make_shared<Shader>(IRRADIANCE_CONVOLUTION, SKYBOX_VERT);
    m_filterMap = std::make_shared<Shader>(FILTER_MAP, SKYBOX_VERT);
    m_brdfLut = std::make_shared<Shader>(BRDF_FRAG, BRDF_VERT);



    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        // note that we store each face with 16 bit floating point values
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
                     512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] = {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };


    m_convertToCubeMap->bind();
    m_convertToCubeMap->setUniformMat4("projection", captureProjection);
    m_convertToCubeMap->setUniform1i("hdrTexture", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, IBLTexture->id);

    glViewport(0, 0, 512, 512);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        m_convertToCubeMap->setUniformMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //renderCube();

        auto mesh = m_skybox.model->meshes[0];
        auto vertexBuffer = mesh.vertexBuffer;
        vertexBuffer->bind();
        vertexBuffer->getLayout()->enableAttributes();

        mesh.indexBuffer->bind();
        glDrawElements(GL_TRIANGLES, (GLsizei) mesh.indices.size(), GL_UNSIGNED_INT, 0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // irradiance map
    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

    m_irradiance->bind();
    m_irradiance->setUniform1i("envMap", 0);
    m_irradiance->setUniformMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

    glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        m_irradiance->setUniformMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto mesh = m_skybox.model->meshes[0];
        auto vertexBuffer = mesh.vertexBuffer;
        vertexBuffer->bind();
        vertexBuffer->getLayout()->enableAttributes();

        mesh.indexBuffer->bind();
        glDrawElements(GL_TRIANGLES, (GLsizei) mesh.indices.size(), GL_UNSIGNED_INT, 0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);




    // specular IBL

    glGenTextures(1, &prefilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);


    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);


    m_filterMap->bind();
    m_filterMap->setUniform1i("environmentMap", 0);
    m_filterMap->setUniformMat4("projection", captureProjection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        unsigned int mipWidth  = 128 * std::pow(0.5, mip);
        unsigned int mipHeight = 128 * std::pow(0.5, mip);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        m_filterMap->setUniform1f("roughness", roughness);
        for (unsigned int i = 0; i < 6; ++i)
        {
            m_filterMap->setUniformMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            auto mesh = m_skybox.model->meshes[0];
            auto vertexBuffer = mesh.vertexBuffer;
            vertexBuffer->bind();
            vertexBuffer->getLayout()->enableAttributes();

            mesh.indexBuffer->bind();
            glDrawElements(GL_TRIANGLES, (GLsizei) mesh.indices.size(), GL_UNSIGNED_INT, 0);
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    glGenTextures(1, &brdfLUTTexture);

    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

    glViewport(0, 0, 512, 512);
    m_brdfLut->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto mesh = quad->meshes[0];
    auto vertexBuffer = mesh.vertexBuffer;
    vertexBuffer->bind();
    vertexBuffer->getLayout()->enableAttributes();

    mesh.indexBuffer->bind();
    glDrawElements(GL_TRIANGLES, (GLsizei) mesh.indices.size(), GL_UNSIGNED_INT, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_window->setWidthAndHeight(1800, 1200);
}

struct RenderArgs
{
    glm::mat4 view;
    glm::mat4 projection;
    Entity modelEntity;
    Light light;
    std::shared_ptr<Shader> shader;
};



void enableCubeTexture(unsigned int slot, unsigned int texture)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
}


void enableTexture(unsigned int slot, unsigned int texture)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture);
}

void enableTexture(unsigned int slot, std::shared_ptr<Texture> const &texture)
{
    if (texture) {
        enableTexture(slot, texture->id);
    } else
    {
        //std::cout << "No Texture" << std::endl;
    }
}

void renderModelEntity(RenderArgs const &renderArgs)
{
    auto entity = renderArgs.modelEntity;
    auto geometry = entity.model;

    if (geometry == nullptr)
    {
        return;
    }


    auto vertexBuffer = geometry->vertexBuffer;
    vertexBuffer->bind();
    vertexBuffer->getLayout()->enableAttributes();
    if (geometry->hasIndexBuffer)
    {
        geometry->indexBuffer->bind();
    }

    glm::mat4 entityMatrix = getMatrix(entity);
    for (auto mesh: geometry->meshes)
    {
        glm::mat4 modelMatrix = entityMatrix * mesh.matrix;

        if (mesh.primitives.size() > 0)
        {
            for (auto primitive: mesh.primitives)
            {
                auto& tuple = geometry->materials[primitive.materialName];
                auto& shader = std::get<1>(tuple);
                auto& material = std::get<0>(tuple);
                shader->bind();
                shader->setUniformMat4("model", modelMatrix);
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
                shader->setUniform1i("albedoMap", 0);
                shader->setUniform1i("normalMap", 1);
                shader->setUniform1i("metallicMap", 2);
                shader->setUniform1i("occlusionMap", 3);
                shader->setUniform1i("emissiveMap", 4);
                shader->setUniform1i("brdfLut", 5);
                shader->setUniform1i("irradianceMap", 6);
                shader->setUniform1i("prefilterMap", 7);


                enableTexture(0, material->albedoTexture);
                enableTexture(1, material->normalTexture);
                enableTexture(2, material->metallicTexture);
                enableTexture(3, material->occlusionTexture);
                enableTexture(4, material->emissiveTexture);
                enableTexture(5, brdfLUTTexture);
                enableCubeTexture(6, irradianceMap);
                enableCubeTexture(7, prefilterMap);

                if(geometry->hasIndexBuffer)
                {
                    glDrawElements(GL_TRIANGLES, (GLsizei) primitive.indexCount, GL_UNSIGNED_INT, (void*) (primitive.indexStart * sizeof(GLuint)));
                } else
                {
                    glDrawArrays(GL_TRIANGLES, 0, geometry->vertices.size());
                }
            }
        } else {
            //glDrawElements(GL_TRIANGLES, (GLsizei) mesh.indices.size(), GL_UNSIGNED_INT, 0);
        }

    }
}

std::vector<Marker> getMarkers(RenderArgs const &renderArgs) {
    Marker light;
    light.position = renderArgs.light.position;

    std::vector<Marker> markers;
    auto entity = renderArgs.modelEntity;
    Marker entityMarker;
    entityMarker.position = entity.translation;
    entityMarker.orientation = entity.rotation;
    markers.push_back(entityMarker);
    markers.push_back(light);

    return markers;
}


void drawSkybox(const Skybox& skybox, const RenderArgs& renderArgs)
{
    glDepthMask(GL_FALSE);
    auto shader = skybox.shader;
    shader->bind();
    shader->setUniformMat4("projection", renderArgs.projection);
    shader->setUniformMat4("view", glm::mat4(glm::mat3(renderArgs.view)));

    auto mesh = skybox.model->meshes[0];
    auto vertexBuffer = mesh.vertexBuffer;
    vertexBuffer->bind();
    vertexBuffer->getLayout()->enableAttributes();
    shader->setUniform1i("skybox", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);

    mesh.indexBuffer->bind();
    glDrawElements(GL_TRIANGLES, (GLsizei) mesh.indices.size(), GL_UNSIGNED_INT, 0);
    glDepthMask(GL_TRUE);
}

void DemoApplication::exec()
{
    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_LINE_SMOOTH);
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

        //rotateCameraAroundEntity(m_modelEntity, deltaTime);

        glm::vec3 cameraFront = camera.orientation * UNIT_Z;
        glm::vec3 cameraUp = camera.orientation * UNIT_Y;

        glm::vec3 cameraTarget = camera.position + cameraFront;
        glm::mat4 view = glm::lookAt(camera.position, cameraTarget, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float) m_window->getWidth() / (float) m_window->getHeight(), 0.3f, 700.0f);


        auto compileShader = [&] {
            m_pipeline = std::make_shared<Shader> (fragmentShader, vertexShader);
        };

        auto loadNewModel = [&](std::string path, bool useModel) {
            m_modelEntity.model = nullptr;
            if (useModel)
            {
                m_modelEntity.model = std::move(loadModel(path));
            } else
            {
                m_modelEntity.model = std::move(m_basicShapes->getShape(BasicShapes::SPHERE));
            }
        };

        m_debugUI->show(m_modelEntity, m_light, [&] {
            m_pipeline = std::make_shared<Shader> (fragmentShader, vertexShader);
        }, loadNewModel);
        RenderArgs renderArgs;
        renderArgs.view = view;
        renderArgs.projection = projection;
        renderArgs.modelEntity = m_modelEntity;
        renderArgs.light = m_light;
        renderArgs.shader = m_pipeline;



        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f ,0.0f, 1.0f);
        drawSkybox(m_skybox, renderArgs);
        if (m_modelEntity.model)
        {
            renderModelEntity(renderArgs);
        }
        m_debugDraw->renderMarkers(getMarkers(renderArgs), view, projection);
        imgui::render();
        m_window->swap();
    }
}
