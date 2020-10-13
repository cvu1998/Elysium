#pragma once

#include "Elysium/KeyCodes.h"
#include "Elysium/Timestep.h"
#include "Elysium/Events/ApplicationEvent.h"
#include "Elysium/Events/MouseEvent.h"
#include "Elysium/Renderer/Camera.h"

namespace Elysium
{
    class OrthographicCameraController
    {
    private:
        float m_AspectRatio;
        float m_ZoomLevel;
        OrthographicCamera m_Camera;

        glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
        float m_CameraRotation = 0.0f;

    public:
        struct InputKeys
        {
            int MoveUpKeyCode = ELY_KEY_UP;
            int MoveDownKeyCode = ELY_KEY_DOWN;
            int MoveRightKeyCode = ELY_KEY_RIGHT;
            int MoveLeftKeyCode = ELY_KEY_LEFT;

            int RotateKeyCode = ELY_KEY_RIGHT_ALT;
        };

        InputKeys CameraInputKeys;

        float CameraTranslationSpeed = 3.0f;
        float CameraRotationSpeed = 90.0f;
        float CameraZoomSpeed = 1.0f;

    private:
        bool onMouseScrollEvent(MouseScrollEvent& event);
        bool onWindowResizeEvent(WindowResizeEvent& event);

    public:
        OrthographicCameraController(float aspectRatio, float zoomLevel=1.0f, 
            const InputKeys& cameraInputKeys = { ELY_KEY_UP,  ELY_KEY_DOWN, ELY_KEY_RIGHT, ELY_KEY_LEFT, ELY_KEY_RIGHT_ALT });

        inline float getBoundsWidth() { return 2 * m_AspectRatio * m_ZoomLevel; }
        inline float getBoundsHeight() { return 2 * m_ZoomLevel; }
        OrthographicCamera& getCamera() { return m_Camera; }
        const OrthographicCamera& getCamera() const { return m_Camera; }

        void resizeBounds(float width, float height);

        void onUpdate(Timestep ts);
        void onEvent(Event& event);
    };
}