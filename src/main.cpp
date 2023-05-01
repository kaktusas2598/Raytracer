#include "ImGuiLayer.hpp"
#include "Texture.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <random>

#include "Color.hpp"
#include "Vec3.hpp"

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

        float width = ImGui::GetContentRegionAvail().x;
        float height = ImGui::GetContentRegionAvail().y;

        GLubyte pixels[int(height)][int(width)][4];
        //GLubyte* pixels = new GLubyte[int(height)*int(width)*4];
        unsigned char* buffer = new unsigned char[int(height) * int(width) * 4];

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

            //for (int i = 0; i < int(height) * int(width) * 4; i+=4) {
                //buffer[i] = distribution(rng);
                //buffer[i + 1] = distribution(rng);
                //buffer[i + 2] = distribution(rng);
                //buffer[i + 3] = 255;
            //}

            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    Color pixelColor(double(i)/(width-1), double(j)/(height-1), 0.25);

                    writeColorToBuffer(buffer, j, i, (int)width, pixelColor);

                    //buffer[(j + i * (int)width) * 4] = 255;
                    //buffer[(j + i * (int)width) * 4 + 1] = 255;
                    //buffer[(j + i * (int)width) * 4 + 2] = 255;
                    //buffer[(j + i * (int)width) * 4 + 3] = 255;
                    //
                    //buffer[i * (int)width * 4 + j] = 255;
                    //buffer[i * (int)width * 4 + j + 1] = 255;
                    //buffer[i * (int)width * 4 + j + 2] = 255;
                    //buffer[i * (int)width * 4 + j + 3] = 255;
                }
            }



            int c;
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    c = ((((i&0x8)==0)^((j&0x8))==0))*255;
                    pixels[i][j][0] = (GLubyte) distribution(rng);
                    pixels[i][j][1] = (GLubyte) *colorChannel;
                    pixels[i][j][2] = (GLubyte) *colorChannel;
                    pixels[i][j][3] = (GLubyte) 255;
                }
            }

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
