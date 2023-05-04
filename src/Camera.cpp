#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

Camera::Camera(float vertFOV, float near, float far) :
    verticalFOV(vertFOV), nearClip(near), farClip(far)
{
    forwardDirection = glm::vec3(0, 0, -1);
    position = glm::vec3(0, 0, 3);

    // HACK:
    recalculateView();
}

bool Camera::onUpdate(GLFWwindow* window, float timeStep) {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    glm::vec2 mousePosition = glm::vec2(x, y);
    glm::vec2 delta = (mousePosition - lastMousePosition) * 0.002f; // mouse sensitivity
    lastMousePosition = mousePosition;

    // Camera movement disabled if RMB is not held down
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        return false;
    }
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    bool moved = false;

    float speed = 5.0f;
	const glm::vec3 upDirection(0.0, 1.0, 0.0);
    glm::vec3 rightDirection = glm::cross(forwardDirection, upDirection);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		position += forwardDirection * speed * timeStep;
        moved = true;
    } else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		position -= forwardDirection * speed * timeStep;
        moved = true;
    }

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		position -= rightDirection * speed * timeStep;
        moved = true;
    } else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		position += rightDirection * speed * timeStep;
        moved = true;
	}

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		position -= upDirection * speed * timeStep;
        moved = true;
    } else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		position += upDirection * speed * timeStep;
        moved = true;
	}

    // Rotation
    if (delta.x != 0.0f || delta.y != 0.0f) {
        float pitchDelta = delta.y * getRotationSpeed();
        float yawDelta = delta.x * getRotationSpeed();

        glm::quat q = glm::normalize(glm::cross(glm::angleAxis(-pitchDelta, rightDirection),
                    glm::angleAxis(-yawDelta, glm::vec3(0.0, 1.0, 0.0))));
        forwardDirection = glm::rotate(q, forwardDirection);
        moved = true;
    }

    if (moved) {
        recalculateView();
        recalculateRayDirections();
    }

    return moved;
}

void Camera::onResize(uint32_t width, uint32_t height) {
    if (width == viewportWidth && height == viewportHeight)
        return;

    viewportWidth = width;
    viewportHeight = height;

    recalculateProjection();
    recalculateRayDirections();
}

float Camera::getRotationSpeed() {
    return 0.3f;
}

void Camera::recalculateProjection() {
    projMatrix = glm::perspectiveFov(glm::radians(verticalFOV), (float)viewportWidth, (float)viewportHeight, nearClip, farClip);
    invProjMatrix = glm::inverse(projMatrix);
}

void Camera::recalculateView() {
    viewMatrix = glm::lookAt(position, position + forwardDirection, glm::vec3(0.0, 1.0, 0.0));
    invViewMatrix = glm::inverse(viewMatrix);
}

void Camera::recalculateRayDirections() {
    rayDirections.resize(viewportWidth * viewportHeight);

    for (uint32_t y = 0; y < viewportHeight; y++) {
        for (uint32_t x = 0; x < viewportWidth; x++) {
            glm::vec2 coord = {(float)x / (float)viewportWidth, (float)y / (float)viewportHeight};
            coord = coord * 2.0f - 1.0f; // Transform from NDC coords [-1, 1]

            glm::vec4 target = invProjMatrix * glm::vec4(coord.x, coord.y, 1.0, 1.0);
            glm::vec3 rayDirection = glm::vec3(invViewMatrix * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0)); // To world space
            rayDirections[x + y * viewportWidth] = rayDirection;
        }
    }
}

