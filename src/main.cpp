#include "ImGuiLayer.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdio>

#include "HittableList.hpp"
#include "Sphere.hpp"

#include "Renderer.hpp"

// GLOBALS
Texture* rayTracedImage = new Texture(GL_TEXTURE_2D);
Renderer renderer;
glm::vec2 lastMousePosition{0.0, 0.0};

// GLFW Callbacks
void errorCallback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    Camera* cam = reinterpret_cast<Camera*>(glfwGetWindowUserPointer(window));
    cam->onKeyPress(key, action);
}

static void mousePositionCallback(GLFWwindow* window, double xpos, double ypos) {
    // TODO: should be in camera onUpdate method
    Camera* camera = renderer.getCamera();
    glm::vec2 mousePosition = glm::vec2(xpos, ypos);
    glm::vec2 delta = (mousePosition - lastMousePosition) * 0.002f; // mouse sensitivity
    lastMousePosition = mousePosition;

    // TODO: camera locking

    // Will go after movement stuff in Camera onUpdate()
    // Rotation
    if (delta.x != 0.0f || delta.y != 0.0f) {
        float pitchDelta = delta.y * camera->getRotationSpeed();
        float yawDelta = delta.x * camera->getRotationSpeed();

        //glm::quat q = glm::normalize(glm::cross(glm::angleAxis(-pitchDelta, rightDirection),
                    //glm::angleAxis(-yawDelta, glm::vec3(0.0, 1.0, 0.0))));
        //forwardDirection = glm::rotate(q, forwardDirection);
        //moved = true;
    }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    // TODO: camera on update mouse button behaviour
    //if (button != GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}


int main(void) {
    // World
    HittableList world;
    world.add(make_shared<Sphere>(Point3(0,0,-1), 0.5));
    world.add(make_shared<Sphere>(Point3(0,-100.5,-1), 100));

    static float startTime = 0;
    static float elapsedTime = 0;

    GLFWwindow* window;

    glfwSetErrorCallback(errorCallback);
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, mousePositionCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    // Set custom user pointer to camera, so it can handle input
    glfwSetWindowUserPointer(window, renderer.getCamera());

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    if (glewInit() != GLEW_OK) {
        printf("Could not initialise Glew.\n");
    }

    ImGuiLayer uiLayer;
    uiLayer.init(window);

    // Our state
    bool showDemoWindow = false;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        /* Poll for and process events */
        glfwPollEvents();

        // Start the Dear ImGui frame
        uiLayer.begin();

        uiLayer.render();

        if (showDemoWindow)
            ImGui::ShowDemoWindow(&showDemoWindow);


        ImGui::Begin("Raytracer View");

        // Image setup
        int width = ImGui::GetContentRegionAvail().x;
        int height = ImGui::GetContentRegionAvail().y;
        renderer.onResize(width, height);

        // Main real time tracer
        // NOTE: blocks imgui wigets not allowing to change them
        //startTime = glfwGetTime();
        //renderer.raytraceWorld(world, width, height);
        //elapsedTime = (glfwGetTime() - startTime) * 1000;

        ImGui::Image(
                (ImTextureID)renderer.getTextureID(),
                ImGui::GetContentRegionAvail(), // Use texture size instead?
                ImVec2(0, 1),
                ImVec2(1, 0)
                );

        ImGui::End();

        ImGui::Begin("Settings");

        ImGui::Text("Render time: %.1f ms", elapsedTime);
        ImGui::Checkbox("Demo Window", &showDemoWindow);
        if (ImGui::Button("Render raycast")) {
            startTime = glfwGetTime();

            renderer.raytraceWorld(world, width, height);

            elapsedTime = (glfwGetTime() - startTime) * 1000;
        }

        // Render to PPM image file
        if (ImGui::Button("Export to PPM")) {
            startTime = glfwGetTime();

            renderer.exportRaytracedPPM(world, width, height);

            elapsedTime = (glfwGetTime() - startTime) * 1000;
        }

        // Generate checkered texture from OpenGL red book
        if (ImGui::Button("Checkered pattern")) {
            startTime = glfwGetTime();

            renderer.generateCheckerTexture(width, height);

            elapsedTime = (glfwGetTime() - startTime) * 1000;
        }

        const unsigned char min = 0;
        const unsigned char max = 255;
        ImGui::PushItemWidth(150);
        ImGui::DragInt("Number of samples per pixel", renderer.getSamplesPerPixel());
        ImGui::DragInt("Number of light ray bounces", renderer.getMaxDepth());

        ImGui::RadioButton("Render Diffuse", renderer.getRenderType(), 0); ImGui::SameLine();
        ImGui::RadioButton("Render Normals", renderer.getRenderType(), 1); ImGui::SameLine();
        ImGui::RadioButton("Render Hit points", renderer.getRenderType(), 2);
        ImGui::PopItemWidth();

        ImGui::End();

        uiLayer.end();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
