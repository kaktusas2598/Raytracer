#include "ImGuiLayer.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdio>

#include "HittableList.hpp"
#include "Sphere.hpp"

#include "Renderer.hpp"

// GLOBALS
Texture* rayTracedImage = new Texture(GL_TEXTURE_2D);
bool realTimeRendering = false;
Camera* mainCamera = new Camera(45.0f, 0.1f, 100.0f);
Renderer renderer;
glm::vec2 lastMousePosition{0.0, 0.0};

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// GLFW Callbacks
void errorCallback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    // Toggle real-time ray tracing
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        realTimeRendering = !realTimeRendering;

    // Example of setting user pointer to handle input
    //Camera* cam = reinterpret_cast<Camera*>(glfwGetWindowUserPointer(window));
    //cam->onKeyPress(key, action, deltaTime);
}

static void mousePositionCallback(GLFWwindow* window, double xpos, double ypos) {
    //Camera* cam = reinterpret_cast<Camera*>(glfwGetWindowUserPointer(window));
    //cam->onMouseMove(xpos, ypos);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) { }


int main(void) {
    // World
    HittableList world;
    shared_ptr<Sphere> sphere = make_shared<Sphere>(Point3(0,0,-1), 0.5);
    sphere->Albedo = Color(1, 0, 1);
    world.add(sphere);
    shared_ptr<Sphere> ground = make_shared<Sphere>(Point3(0,-100.5,-1), 100);
    ground->Albedo = Color(0, 1, 1);
    world.add(ground);

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
    glfwSetWindowUserPointer(window, mainCamera);

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
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        /* Poll for and process events */
        glfwPollEvents();
        mainCamera->onUpdate(window, deltaTime);

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
        mainCamera->onResize(width, height);

        // Main real time tracer
        if (realTimeRendering) {
            startTime = glfwGetTime();
            renderer.raytraceWorld(world, width, height, mainCamera);
            elapsedTime = (glfwGetTime() - startTime) * 1000;
        }

        ImGui::Image(
                (ImTextureID)renderer.getTextureID(),
                ImGui::GetContentRegionAvail(), // Use texture size instead?
                ImVec2(0, 1),
                ImVec2(1, 0)
                );

        ImGui::End();

        ImGui::Begin("Settings");

        ImGui::Text("Render time: %.1f ms", elapsedTime);
        ImGui::Separator();
        ImGui::Text("Use RMB to enable camera movement in real time");
        ImGui::Checkbox("Real Time Raytracing", &realTimeRendering);
        ImGui::Checkbox("ImGui Demo Window", &showDemoWindow);
        if (ImGui::Button("Render raycast")) {
            startTime = glfwGetTime();

            renderer.raytraceWorld(world, width, height, mainCamera);

            elapsedTime = (glfwGetTime() - startTime) * 1000;
        }

        // Render to PPM image file
        if (ImGui::Button("Export to PPM")) {
            startTime = glfwGetTime();

            renderer.exportRaytracedPPM(world, width, height, mainCamera);

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
        ImGui::SeparatorText("Render options");
        ImGui::DragInt("Number of samples per pixel", renderer.getSamplesPerPixel());
        ImGui::DragInt("Number of light ray bounces", renderer.getMaxDepth());

        ImGui::SeparatorText("Render types");
        ImGui::RadioButton("Render Diffuse", renderer.getRenderType(), 0);
        ImGui::RadioButton("Render Colors", renderer.getRenderType(), 1);
        ImGui::RadioButton("Render Normals", renderer.getRenderType(), 2);
        ImGui::RadioButton("Render Hit points", renderer.getRenderType(), 3);
        ImGui::PopItemWidth();

        ImGui::SeparatorText("Scene");
        int i = 1;
        for(auto& object : world.getObjects()) {
            //glm::value_ptr(glm::vec3);
            Sphere* s = (Sphere*)(object.get());
            ImGui::PushID(i); // Solves issues with multiple elements sharing same names
            ImGui::DragFloat3("Position", (float*)s->getCenter());
            ImGui::DragFloat("Radius", s->getRadius());
            ImGui::ColorEdit3("Albedo", (float*)&object->Albedo);
            ImGui::PopID();
            i++;
        }

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
