#pragma once

#include "Timestep.h"
#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"
#include "Renderer/Camera.h"

namespace Elysium
{
    class OrthographicCameraController
    {
    private:
        float m_AspectRatio;
        float m_ZoomLevel;
        OrthographicCamera m_Camera;

        glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
        float m_CameraTranslationSpeed = 3.0f;

        float m_CameraRotation = 0.0f;
        float m_CameraRotationSpeed = 90.0f;

    private:
        bool onMouseScrollEvent(MouseScrollEvent& event);
        bool onWindowResizeEvent(WindowResizeEvent& event);

    public:
        OrthographicCameraController(float aspectRatio, float zoomLevel=1.0f);

        inline float getBoundsWidth() { return 2 * m_AspectRatio * m_ZoomLevel; }
        inline float getBoundsHeight() { return 2 * m_ZoomLevel; }
        OrthographicCamera& getCamera() { return m_Camera; }
        const OrthographicCamera& getCamera() const { return m_Camera; }

        inline const void setCameraTranslationSpeed(float translationSpeed) { m_CameraTranslationSpeed = translationSpeed; }
        inline const void setCameraRotationSpeed(float rotationSpeed) { m_CameraRotationSpeed = rotationSpeed; }

        void onUpdate(Timestep ts);
        void onEvent(Event& event);
    };


}