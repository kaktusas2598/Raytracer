#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "GLFW/glfw3.h"

Camera::Camera(float vertFOV, float near, float far) :
    verticalFOV(vertFOV), nearClip(near), farClip(far)
{
    forwardDirection = glm::vec3(0, 0, -1);
    rightDirection = glm::cross(forwardDirection, glm::vec3(0.0, 1.0, 0.0));
    position = glm::vec3(0, 0, 3);

    // HACK:
    recalculateView();
}


void Camera::onKeyPress(int key, int action, float timeStep) {
    // TODO: update actual movement from onUpdate() only set directions here
	float speed = 5.0f;
	const glm::vec3 upDirection(0.0, 1.0, 0.0);
    rightDirection = glm::cross(forwardDirection, upDirection);

	if (key == GLFW_KEY_W && action == GLFW_PRESS) {
	    currentMovement = CameraMovement::FORWARD;
		position += forwardDirection * speed * timeStep;
        moved = true;
	} else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
	    currentMovement = CameraMovement::BACKWARD;
		position -= forwardDirection * speed * timeStep;
        moved = true;
    } else if (key == GLFW_KEY_A && action == GLFW_PRESS) {
	    currentMovement = CameraMovement::LEFT;
		position -= rightDirection * speed * timeStep;
        moved = true;
	} else if (key == GLFW_KEY_D && action == GLFW_PRESS) {
	    currentMovement = CameraMovement::RIGHT;
		position += rightDirection * speed * timeStep;
        moved = true;
	} else if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
	    currentMovement = CameraMovement::DOWN;
		position -= upDirection * speed * timeStep;
        moved = true;
	} else if (key == GLFW_KEY_E && action == GLFW_PRESS) {
	    currentMovement = CameraMovement::UP;
		position += upDirection * speed * timeStep;
        moved = true;
	} else {
	    currentMovement = CameraMovement::NONE;
	}

	// TODO: rotation here based on mouse


	//return moved;
}

void Camera::onMouseMove(double xpos, double ypos) {
    glm::vec2 mousePosition = glm::vec2(xpos, ypos);
    glm::vec2 delta = (mousePosition - lastMousePosition) * 0.002f; // mouse sensitivity
    lastMousePosition = mousePosition;

    // TODO: camera locking
    //if (!Input::IsMouseButtonDown(MouseButton::Right))
	//{
		//Input::SetCursorMode(CursorMode::Normal);
		//return false;
	//}

	//Input::SetCursorMode(CursorMode::Locked);

    // Will go after movement stuff in Camera onUpdate()
    // Rotation
    if (delta.x != 0.0f || delta.y != 0.0f) {
        float pitchDelta = delta.y * getRotationSpeed();
        float yawDelta = delta.x * getRotationSpeed();

        glm::quat q = glm::normalize(glm::cross(glm::angleAxis(-pitchDelta, rightDirection),
                    glm::angleAxis(-yawDelta, glm::vec3(0.0, 1.0, 0.0))));
        forwardDirection = glm::rotate(q, forwardDirection);
        moved = true;
    }
}

bool Camera::onUpdate(float timeStep) {
    // TODO: implement, somehow route GLFW input for mouse move/buttons and key input to here
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

