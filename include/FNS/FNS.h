#pragma once

struct Mesh
{
    GLuint VAO = 0;
    GLuint posVBO = 0;
    GLuint colVBO = 0;
    GLuint uvVBO = 0; // UV를 VBO로 쓸 경우
    GLsizei vertexCount = 0;
    GLenum primitive = GL_TRIANGLE_FAN;
};
struct Texture
{
    GLuint EngineID = 0;
    int TextureID = 0;
    int width = 0, height = 0, channels = 0;
    std::string path = "";
};
namespace FN {
    Mesh MakeMesh(std::vector<float> &data, std::vector<float> &color, GLenum primitive);
    Texture MakeTexture(std::string path);
    GLFWwindow *StartGLFW(const unsigned int ScrrenX, const unsigned int ScrrenY);
    std::string LoadShaderSource(const char *filePath);
    GLuint CompileShader(GLenum type, const char *path);
    void processInput(GLFWwindow *window);
    unsigned int MakeShaderProgram(GLuint VertexShader, GLuint FragmentShader);
    void MouseCallbacks(GLFWwindow *window, int button, int action, int mods);
    void framebuffer_size_callback(GLFWwindow *window, int width, int height);
}