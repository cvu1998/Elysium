#include "OrthographicCameraController.h"

#include <algorithm> 

#include "Input.h"
#include "KeyCodes.h"

namespace Elysium
{
    OrthographicCameraController::OrthographicCameraController(float aspectRatio, float zoomLevel) : m_AspectRatio(aspectRatio), m_ZoomLevel(zoomLevel),
        m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel)
    {
    }

    void OrthographicCameraController::onUpdate(Timestep ts)
    {
        if (Input::isKeyPressed(ELY_KEY_UP))
        {
            m_CameraPosition.x += -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
            m_CameraPosition.y += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
        }
        else if (Input::isKeyPressed(ELY_KEY_DOWN))
        {
            m_CameraPosition.x -= -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
            m_CameraPosition.y -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
        }

        if (Input::isKeyPressed(ELY_KEY_LEFT))
        {
            m_CameraPosition.x -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
            m_CameraPosition.y -= sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
        }
        else if (Input::isKeyPressed(ELY_KEY_RIGHT))
        {
            m_CameraPosition.x += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
            m_CameraPosition.y += sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
        }

        if (Input::isKeyPressed(ELY_KEY_R))
            m_CameraRotation += m_CameraRotationSpeed * ts;
        else if (Elysium::Input::isKeyPressed(ELY_KEY_T))
            m_CameraRotation -= m_CameraRotationSpeed * ts;

        m_Camera.setPosition(m_CameraPosition);
        m_Camera.setRotation(glm::radians(m_CameraRotation));
    }

    void OrthographicCameraController::onEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<Elysium::MouseScrollEvent>(BIND_EVENT_FUNCTION(OrthographicCameraController::onMouseScrollEvent));
        dispatcher.Dispatch<Elysium::WindowResizeEvent>(BIND_EVENT_FUNCTION(OrthographicCameraController::onWindowResizeEvent));
    }

    bool OrthographicCameraController::onMouseScrollEvent(MouseScrollEvent& event)
    {
        m_ZoomLevel -= event.getYOffset();
        m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
        m_Camera.setProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
        return false;
    }

    bool OrthographicCameraController::onWindowResizeEvent(WindowResizeEvent& event)
    {
        m_AspectRatio = (float)event.getWidth() / (float)event.getHeight();
        m_Camera.setProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
        return false;
    }
}