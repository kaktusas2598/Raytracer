#include "Camera.hpp"
#include "ImGuiLayer.hpp"
#include "Texture.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <random>
#include <fstream>

#include "Common.hpp"
#include "Color.hpp"
#include "HittableList.hpp"
#include "Sphere.hpp"

Color rayColor(const Ray& r, const Hittable& world) {
    HitRecord rec;
    if (world.hit(r, 0, INF, rec)) {
        return 0.5 * (rec.normal + Color(1,1,1));
    }
    Vec3 normalizedDir = normalize(r.direction());
    auto t = 0.5*(normalizedDir.y() + 1.0);
    return (1.0-t)*Color(1.0, 1.0, 1.0) + t*Color(0.5, 0.7, 1.0);
}

void errorCallback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

Texture* rayTracedImage = new Texture(GL_TEXTURE_2D);

int main(void) {
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
    bool showDemoWindow = true;

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
        // Higher sample count will solve aliasing issues, but decrease performance, static to make it easier with imgui
        static int samplesPerPixel = 1;

        // World
        HittableList world;
        world.add(make_shared<Sphere>(Point3(0,0,-1), 0.5));
        world.add(make_shared<Sphere>(Point3(0,-100.5,-1), 100));

        // Camera setup
        Camera camera;

        GLubyte pixels[height][width][4];
        unsigned char* buffer = new unsigned char[height * width * 4];
        static float startTime = 0;
        static float elapsedTime = 0;

        ImGui::Image(
                (ImTextureID)rayTracedImage->getID(),
                ImGui::GetContentRegionAvail(),
                ImVec2(0, 1),
                ImVec2(1, 0)
                );

        ImGui::End();

        ImGui::Begin("Settings");

        // Render for viewport
        ImGui::Checkbox("Demo Window", &showDemoWindow);
        if (ImGui::Button("Render raycast")) {
            std::random_device randDevice;
            std::mt19937 rng(randDevice());
            std::uniform_int_distribution<GLubyte> distribution(0, 255);

            startTime = glfwGetTime();

            // Generate colour buffer, Alpha always 1.0
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    Color pixelColor(0,0,0);
                    for (int s = 0; s < samplesPerPixel; ++s) {
                        auto u = (j + randomDouble()) / (width-1);
                        auto v = (i + randomDouble()) / (height-1);
                        Ray r = camera.getRay(u, v);
                        pixelColor += rayColor(r, world);
                    }
                    writeColorToBuffer(buffer, j, i, width, pixelColor, samplesPerPixel);
                }
            }

            rayTracedImage->setData(buffer);
            rayTracedImage->init(width, height);

            elapsedTime = (glfwGetTime() - startTime) * 1000;
        }

        // Render to PPM image file
        if (ImGui::Button("Export to PPM")) {
            std::random_device randDevice;
            std::mt19937 rng(randDevice());
            std::uniform_int_distribution<GLubyte> distribution(0, 255);

            startTime = glfwGetTime();

            std::ofstream outFile;
            outFile.open("image.ppm");
            outFile << "P3\n" << (int)width << ' ' << (int)height << "\n255\n";

            for (int j = height-1; j >= 0; --j) {
                std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
                for (int i = 0; i < width; ++i) {
                    Color pixelColor(0,0,0);
                    for (int s = 0; s < samplesPerPixel; ++s) {
                        auto u = (i + randomDouble()) / (width-1);
                        auto v = (j + randomDouble()) / (height-1);
                        Ray r = camera.getRay(u, v);
                        pixelColor += rayColor(r, world);
                    }
                    writeColorPPM(outFile, pixelColor, samplesPerPixel);
                }
            }
            outFile.close();

            rayTracedImage->setData(buffer);
            rayTracedImage->init(width, height);

            elapsedTime = (glfwGetTime() - startTime) * 1000;
        }

        // Generate checkered texture from OpenGL red book
        if (ImGui::Button("Checkered pattern")) {
            std::random_device randDevice;
            std::mt19937 rng(randDevice());
            std::uniform_int_distribution<GLubyte> distribution(0, 255);

            startTime = glfwGetTime();

            int c;
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    c = ((((i&0x8)==0)^((j&0x8))==0))*255;
                    pixels[i][j][0] = (GLubyte) distribution(rng);
                    pixels[i][j][1] = (GLubyte) c;
                    pixels[i][j][2] = (GLubyte) c;
                    pixels[i][j][3] = (GLubyte) 255;
                }
            }

            rayTracedImage->setData((unsigned char*)pixels);
            rayTracedImage->init(width, height);

            elapsedTime = (glfwGetTime() - startTime) * 1000;
        }


        const unsigned char min = 0;
        const unsigned char max = 255;
        ImGui::DragInt("Number of Samples per Pixel", &samplesPerPixel);
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
