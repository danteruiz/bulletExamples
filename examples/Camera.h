class Camera
{
public:
    Camera(Input* input);
    glm::mat4 getViewMatrix() const;
    glm::vec3 getViewPosition() const;
    void rotateAroundTarget(bool enable) { m_rotateAroundTarget = enable; }
    bool isRotatingAroundTarget() const { m_rotateAroundTarget; }
    void update(float deltaTime);
protected:
    glm::vec3 position;
    glm::quat orientation;
    bool m_rotateAroundTarget { false }
    Input* m_input { nullptr }
};
