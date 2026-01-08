
#include <windows.h>
#include <Other/std_image.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <numeric>
#include <FNS/FNS.h>
#include <algorithm>
using namespace std;
const unsigned int ScreenX = 1000;
const unsigned int ScreenY = 1000;
int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow *window = FN::StartGLFW(ScreenX,ScreenY);
    GLuint VertexShader = FN::CompileShader(GL_VERTEX_SHADER, "shader\\shader.vert");
    GLuint FragmentShader = FN::CompileShader(GL_FRAGMENT_SHADER, "shader\\shader.frag");
    stbi_set_flip_vertically_on_load(true);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    unsigned int ShaderProgram = FN::MakeShaderProgram(VertexShader, FragmentShader);
    glDeleteShader(VertexShader);
    glDeleteShader(FragmentShader);
    glfwSwapInterval(1);
    glfwSetMouseButtonCallback(window, FN::MouseCallbacks);
    // Typing here.
    std::vector<float> vertices = {
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f
    };
    std::vector<float> colors = {
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f
    };
    std::vector<Mesh> Meshes = {FN::MakeMesh(vertices,colors,GL_TRIANGLES)};

    std::vector<Texture> Textures = {};//비워두세요
    std::vector<std::string> paths = {"resource\\wall.jpg"};
    for (const std::string &path:paths) {
        Textures.push_back(FN::MakeTexture(path));
    }
    // Typing here.
    glUseProgram(ShaderProgram);
    while (!glfwWindowShouldClose(window))
    {
        FN::processInput(window);
        glClear(GL_COLOR_BUFFER_BIT);
        for (const Mesh &mesh : Meshes)
        {
            glBindVertexArray(mesh.VAO);
            glDrawArrays(mesh.primitive, 0, mesh.vertexCount);
        }
        glBindVertexArray(0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}