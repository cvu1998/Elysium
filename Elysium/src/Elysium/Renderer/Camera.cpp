#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Elysium
{
    OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top) :
        m_Position(0.0f),
        m_Bounds(fabs(left) + fabs(right), fabs(bottom) + fabs(top)),
        m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)),
        m_ViewMatrix(1.0f)
    {
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    void OrthographicCamera::setProjection(float left, float right, float bottom, float top)
    {
        m_Bounds = { fabs(left) + fabs(right), fabs(bottom) + fabs(top) };
        m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    glm::vec2 OrthographicCamera::getScreenToWorldPosition(unsigned int width, unsigned int height, std::pair<float, float>& position)
    {
        return { ((position.first / width) * m_Bounds.x - m_Bounds.x * 0.5f) + m_Position.x,
            (m_Bounds.y * 0.5f - (position.second / height) * m_Bounds.y) + m_Position.y };
    }

    void OrthographicCamera::recalculateViewMatrix()
    {
        glm::mat4 transform =
            glm::translate(glm::mat4(1.0f), m_Position) *
            glm::rotate(glm::mat4(1.0f), m_Rotation, glm::vec3(0, 0, 1));

        m_ViewMatrix = glm::inverse(transform);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }
}