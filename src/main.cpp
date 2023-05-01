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
    bool show_demo_window = true;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        /* Poll for and process events */
        glfwPollEvents();

        // Start the Dear ImGui frame
        uiLayer.begin();

        uiLayer.render();

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);



        ImGui::Begin("Raytracer View");

        // Image setup
        int width = ImGui::GetContentRegionAvail().x;
        int height = ImGui::GetContentRegionAvail().y;
        auto aspectRatio = width / height;

        // World
        HittableList world;
        world.add(make_shared<Sphere>(Point3(0,0,-1), 0.5));
        world.add(make_shared<Sphere>(Point3(0,-100.5,-1), 100));

        // Camera setup
        // FIXME: only rendering sphere correctly if viewport window is big in imgui, probably because of viewport settings here
        auto viewportHeight = 2.0;
        auto viewportWidth = aspectRatio * viewportHeight;
        auto focalLength = 1.0;

        auto origin = Point3(0, 0, 0);
        auto horizontal = Vec3(viewportWidth, 0, 0);
        auto vertical = Vec3(0, viewportHeight, 0);
        auto lowerLeftCorner = origin - horizontal/2 - vertical/2 - Vec3(0, 0, focalLength);

        GLubyte pixels[height][width][4];
        unsigned char* buffer = new unsigned char[height * width * 4];

        static unsigned char* colorChannel = new unsigned char{0};


        ImGui::Image(
                (ImTextureID)rayTracedImage->getID(),
                ImGui::GetContentRegionAvail(),
                ImVec2(0, 1),
                ImVec2(1, 0)
                );

        ImGui::End();

        ImGui::Begin("Settings");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("Options");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        if (ImGui::Button("Render")) {
            std::random_device randDevice;
            std::mt19937 rng(randDevice());
            std::uniform_int_distribution<GLubyte> distribution(0, 255);

            // Generate colour buffer, Alpha always 1.0
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    //Color pixelColor(double(i)/(width-1), double(j)/(height-1), 0.25);
                    auto u = double(j) / (width-1);
                    auto v = double(i) / (height-1);
                    Ray r(origin, lowerLeftCorner + u*horizontal + v*vertical - origin);
                    Color pixelColor = rayColor(r, world);
                    writeColorToBuffer(buffer, j, i, width, pixelColor);
                }
            }

            //////////////////////
            // Render to PPM image
            std::ofstream outFile;
            outFile.open("image.ppm");
            outFile << "P3\n" << (int)width << ' ' << (int)height << "\n255\n";

            for (int j = height-1; j >= 0; --j) {
                std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
                for (int i = 0; i < width; ++i) {
                    auto u = double(i) / (width-1);
                    auto v = double(j) / (height-1);
                    Ray r(origin, lowerLeftCorner + u*horizontal + v*vertical - origin);
                    Color pixelColor = rayColor(r, world);

                    writeColorPPM(outFile, pixelColor);
                }
            }
            outFile.close();
            //////////////////////


            /////////////////////
            int c;
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    // Use c to generate checkered texture! from OpenGL red book
                    c = ((((i&0x8)==0)^((j&0x8))==0))*255;
                    pixels[i][j][0] = (GLubyte) distribution(rng);
                    pixels[i][j][1] = (GLubyte) *colorChannel;
                    pixels[i][j][2] = (GLubyte) *colorChannel;
                    pixels[i][j][3] = (GLubyte) 255;
                }
            }
            ////////////////////////////

            //rayTracedImage->setData((unsigned char*)pixels);
            rayTracedImage->setData(buffer);
            rayTracedImage->init(width, height);

        }

        const unsigned char min = 0;
        const unsigned char max = 255;
        ImGui::SliderScalar("Color", ImGuiDataType_U8, colorChannel, &min, &max);

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
