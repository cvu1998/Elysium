#pragma once

#include <Utility>

#include <glm/glm.hpp>

namespace Elysium
{
    class OrthographicCamera
    {
    private:
        glm::mat4 m_ViewProjectionMatrix;
        glm::mat4 m_ProjectionMatrix;
        glm::mat4 m_ViewMatrix;

        glm::vec3 m_Position;
        glm::vec2 m_Bounds;
        float m_Rotation = 0.0f;

    private:
        void recalculateViewMatrix();

    public:
        OrthographicCamera(float left, float right, float bottom, float top);

        void setProjection(float left, float right, float bottom, float top);

        inline const glm::vec3& getPosition() const { return m_Position; }
        inline void setPosition(const glm::vec3& position) {
            m_Position = position;
            recalculateViewMatrix();
        }

        // --Rotation value in radians--
        inline float getRotation() const { return m_Rotation; }
        inline void setRotation(float rotation) {
            m_Rotation = rotation;
            recalculateViewMatrix();
        }

        inline const glm::mat4& getProjectionMatrix() const { return m_ProjectionMatrix; }
        inline const glm::mat4& getViewnMatrix() const { return m_ViewMatrix; }
        inline const glm::mat4& getViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

        glm::vec2 getScreenToWorldPosition(unsigned int width, unsigned int height, std::pair<float,float>& position);
    };
}
