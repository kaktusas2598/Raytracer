#include "ImGuiLayer.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdio>

#include "HittableList.hpp"
#include "Sphere.hpp"

#include "Renderer.hpp"

void errorCallback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

Texture* rayTracedImage = new Texture(GL_TEXTURE_2D);
Renderer renderer;

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

        ImGui::Image(
                (ImTextureID)renderer.getTextureID(),
                ImGui::GetContentRegionAvail(),
                ImVec2(0, 1),
                ImVec2(1, 0)
                );

        ImGui::End();

        ImGui::Begin("Settings");

        // Render for viewport
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

        ImGui::Text("Render time: %.1f ms", elapsedTime);
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
