#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "GLFW/glfw3.h"

Camera::Camera() {
    // FIXME: only rendering sphere correctly if viewport window is big in imgui, probably because of viewport settings here
    //auto aspectRatio = width / height;
    auto aspectRatio = 16.0 / 9.0;

    auto viewportHeight = 2.0;
    auto viewportWidth = aspectRatio * viewportHeight;
    auto focalLength = 1.0;

    origin = Point3(0, 0, 0);
    horizontal = Vec3(viewportWidth, 0, 0);
    vertical = Vec3(0, viewportHeight, 0);
    lowerLeftCorner = origin - horizontal/2 - vertical/2 - Vec3(0, 0, focalLength);
}

Camera::Camera(float vertFOV, float near, float far) :
    verticalFOV(vertFOV), nearClip(near), farClip(far)
{
    forwardDirection = glm::vec3(0, 0, -1);
    position = glm::vec3(0, 0, 3);

    // HACK:
    recalculateView();
}


void Camera::onKeyPress(int key, int action) {
    // TODO: Should be set and called in onUpdate()
    float timeStep = 1.0f;

	bool moved = false;
	float speed = 5.0f;
	const glm::vec3 upDirection(0.0, 1.0, 0.0);
    glm::vec3 rightDirection = glm::cross(forwardDirection, upDirection);

	if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		position += forwardDirection * speed * timeStep;
        moved = true;
	} else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		position -= forwardDirection * speed * timeStep;
        moved = true;
    }

	if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		position -= rightDirection * speed * timeStep;
        moved = true;
	}
	else if (key == GLFW_KEY_D && action == GLFW_PRESS) {
		position += rightDirection * speed * timeStep;
        moved = true;
	}

	if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
		position -= upDirection * speed * timeStep;
        moved = true;
	}
	else if (key == GLFW_KEY_E && action == GLFW_PRESS) {
		position += upDirection * speed * timeStep;
        moved = true;
	}

	// TODO: rotation here based on mouse

	if (moved) {
	    recalculateView();
	    recalculateRayDirections();
	}

	//return moved;
}

bool Camera::onUpdate(float timeStep) {
    // TODO: implement, somehow route GLFW input for mouse move/buttons and key input to here
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

