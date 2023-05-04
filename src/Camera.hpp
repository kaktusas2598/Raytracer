#pragma once

#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "Common.hpp"

class Camera {
    public:
        Camera(float vertFOV, float near, float far);

        bool onUpdate(GLFWwindow* window, float timeStep);
        void onResize(uint32_t width, uint32_t height);

        const glm::mat4& getProjection() const { return projMatrix; }
        const glm::mat4& getInvProjection() const { return invProjMatrix; }
        const glm::mat4& getView() const { return viewMatrix; }
        const glm::mat4& getInvView() const { return invViewMatrix; }

        const glm::vec3& getPosition() const { return position; }
        const glm::vec3& getDirection() const { return forwardDirection; }
        // Get cached ray direction, to prevent recalculating rays from camera matrix when it's not moving
        const std::vector<glm::vec3>& getRayDirections() const { return rayDirections; }
        float getRotationSpeed();

        //Ray getRay(double u, double v) const {
            //return Ray(origin, lowerLeftCorner + u*horizontal + v*vertical - origin);
        //}
    private:
        void recalculateProjection();
        void recalculateView();
        void recalculateRayDirections();

        glm::mat4 projMatrix{0.0};
        glm::mat4 viewMatrix{0.0};
        glm::mat4 invProjMatrix{0.0};
        glm::mat4 invViewMatrix{0.0};

        float verticalFOV = 45.0f;
        float nearClip = 0.1f;
        float farClip = 100.0f;

        glm::vec3 position{0.0, 0.0, 0.0};
        glm::vec3 forwardDirection{0.0, 0.0, 0.0};

        // Cached ray directions
        std::vector<glm::vec3> rayDirections;

        glm::vec2 lastMousePosition{0.0, 0.0};
        uint32_t viewportWidth = 0, viewportHeight = 0;
};
