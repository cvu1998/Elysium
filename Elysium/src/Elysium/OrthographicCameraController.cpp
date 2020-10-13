#include "OrthographicCameraController.h"

#include <algorithm> 

#include "Elysium/Input.h"
#include "Elysium/Log.h"

namespace Elysium
{
    OrthographicCameraController::OrthographicCameraController(float aspectRatio, float zoomLevel, const InputKeys& cameraInputKeys) :
        m_AspectRatio(aspectRatio), 
        m_ZoomLevel(zoomLevel),
        m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel),
        CameraInputKeys(cameraInputKeys)
    {
    }

    void OrthographicCameraController::resizeBounds(float width, float height)
    {
        m_AspectRatio = width / height;
        m_Camera.setProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
    }

    void OrthographicCameraController::onUpdate(Timestep ts)
    {
        if (Input::isKeyPressed(CameraInputKeys.MoveUpKeyCode))
        {
            m_CameraPosition.x += -sin(glm::radians(m_CameraRotation)) * CameraTranslationSpeed * ts;
            m_CameraPosition.y += cos(glm::radians(m_CameraRotation)) * CameraTranslationSpeed * ts;
        }
        else if (Input::isKeyPressed(CameraInputKeys.MoveDownKeyCode))
        {
            m_CameraPosition.x -= -sin(glm::radians(m_CameraRotation)) * CameraTranslationSpeed * ts;
            m_CameraPosition.y -= cos(glm::radians(m_CameraRotation)) * CameraTranslationSpeed * ts;
        }

        if (Input::isKeyPressed(CameraInputKeys.MoveLeftKeyCode))
        {
            m_CameraPosition.x -= cos(glm::radians(m_CameraRotation)) * CameraTranslationSpeed * ts;
            m_CameraPosition.y -= sin(glm::radians(m_CameraRotation)) * CameraTranslationSpeed * ts;
        }
        else if (Input::isKeyPressed(CameraInputKeys.MoveRightKeyCode))
        {
            m_CameraPosition.x += cos(glm::radians(m_CameraRotation)) * CameraTranslationSpeed * ts;
            m_CameraPosition.y += sin(glm::radians(m_CameraRotation)) * CameraTranslationSpeed * ts;
        }

        if (Input::isKeyPressed(CameraInputKeys.RotateKeyCode))
            m_CameraRotation += CameraRotationSpeed * ts;

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
        m_ZoomLevel -= CameraZoomSpeed * event.getYOffset();
        m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
        m_Camera.setProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
        return false;
    }

    bool OrthographicCameraController::onWindowResizeEvent(WindowResizeEvent& event)
    {
        resizeBounds((float)event.getWidth(), (float)event.getHeight());
        return false;
    }
}