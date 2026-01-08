
#define WIN32_LEAN_AND_MEAN // windows.h 불필요한 것들 조금 줄여주는 매크로 (선택)
#define STB_IMAGE_IMPLEMENTATION
#define NOMINMAX
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

// struct Mesh
// {
//     GLuint VAO = 0;
//     GLuint posVBO = 0;
//     GLuint colVBO = 0;
//     GLuint uvVBO = 0; // UV를 VBO로 쓸 경우
//     GLsizei vertexCount = 0;
//     GLenum primitive = GL_TRIANGLE_FAN;
// };
// struct Texture
// {
//     int TextureID = 0;
//     GLuint EngineID = 0;
//     int width = 0, height = 0, channels = 0;
//     const char* path;
// };
// 내부 함수
std::vector<float> MakeUV_BoundingBox(const std::vector<float> &pos)
{
    std::vector<float> uv;

    if (pos.size() < 3)
        return uv;

    float minX = pos[0];
    float maxX = pos[0];
    float minY = pos[1];
    float maxY = pos[1];

    // 1️⃣ bounding box 계산
    for (size_t i = 0; i < pos.size() / 3; ++i)
    {
        float x = pos[i * 3 + 0];//{x , y , z} 3개의 백터
        float y = pos[i * 3 + 1];//

        minX = std::min(minX, x);
        maxX = std::max(maxX, x);
        minY = std::min(minY, y);
        maxY = std::max(maxY, y);
    }

    float width = maxX - minX;
    float height = maxY - minY;

    // 0으로 나누기 방지 (도형이 선/점일 때)
    if (width == 0.0f)
        width = 1.0f;
    if (height == 0.0f)
        height = 1.0f;

    uv.reserve((pos.size() / 3) * 2);

    // 2️⃣ UV 계산
    for (size_t i = 0; i < pos.size() / 3; ++i)
    {
        float x = pos[i * 3 + 0];
        float y = pos[i * 3 + 1];

        float u = (x - minX) / width;
        float v = (y - minY) / height;

        uv.push_back(u);
        uv.push_back(v);
    }

    return uv;
}

//내부 함수

Mesh FN::MakeMesh(std::vector<float> &data, std::vector<float> &color, GLenum primitive)
{
    Mesh Mesh;
    glGenVertexArrays(1,&Mesh.VAO);
    glBindVertexArray(Mesh.VAO);
    glGenBuffers(1,&Mesh.posVBO);
    glGenBuffers(1, &Mesh.colVBO);
    glGenBuffers(1, &Mesh.uvVBO);
    Mesh.primitive = primitive;
    Mesh.vertexCount = static_cast<GLsizei>(data.size() / 3);
    //layout 0 - position
    glBindBuffer(GL_ARRAY_BUFFER,Mesh.posVBO);
    glBufferData(GL_ARRAY_BUFFER,data.size() * sizeof(float),data.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(0);
    //layout 1 - color
    glBindBuffer(GL_ARRAY_BUFFER,Mesh.colVBO);
    glBufferData(GL_ARRAY_BUFFER,color.size() * sizeof(float),color.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(1);
    //layout 2 - uv
    std::vector<float> uv = MakeUV_BoundingBox(data);
    glBindBuffer(GL_ARRAY_BUFFER,Mesh.uvVBO);
    glBufferData(GL_ARRAY_BUFFER,uv.size() * sizeof(float),uv.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(2);

    //end
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return Mesh;//Q.E.D
}
int TextureID = 0;
Texture FN::MakeTexture(std::string path)
{
    Texture Texture;
    Texture.path = path;
    Texture.TextureID = TextureID;
    TextureID++;
    glGenTextures(1, &Texture.EngineID);
    glBindTexture(GL_TEXTURE_2D, Texture.EngineID);
    // texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    unsigned char *data = stbi_load(Texture.path.c_str(), &Texture.width, &Texture.height, &Texture.channels, 0);
    if (data)
    {
        if (Texture.channels == 3)
        {
            // RGB
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Texture.width, Texture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else if (Texture.channels == 4)
        {
            // RGBA
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Texture.width, Texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    return Texture;
}
GLFWwindow* FN::StartGLFW(const unsigned int ScrrenX, const unsigned int ScrrenY)
{
    GLFWwindow *window = glfwCreateWindow(ScrrenX, ScrrenY, "SPACE.", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }
    return window;
}
std::string FN::LoadShaderSource(const char *filePath)
{
    std::ifstream file(filePath);
    std::stringstream buffer;
    if (!file.is_open())
    {
        std::cerr << "Failed to open File : Path{ " << filePath << " }" << std::endl;
        return "";
    }
    buffer << file.rdbuf();
    return buffer.str();
}
GLuint FN::CompileShader(GLenum type, const char *path)
{
    std::string source = LoadShaderSource(path);
    const char *src = source.c_str();

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    // 에러 체크
    int success;
    char log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, log);
        std::cerr << "Shader Compile Error : {" << path << "}" << log << std::endl;
    }

    return shader;
}
void FN::processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}
unsigned int FN::MakeShaderProgram(GLuint VertexShader, GLuint FragmentShader)
{
    unsigned int ShaderProgram;
    ShaderProgram = glCreateProgram();             // 셰이더 프로그램 ID 리턴
    glAttachShader(ShaderProgram, VertexShader);   //---------------
    glAttachShader(ShaderProgram, FragmentShader); // 영어 읽을 수 있으면 해석해봐라
    glLinkProgram(ShaderProgram);                  //----------------
    //--ShaderProgrm 컴파일 성공 확인
    char infoLog[512];
    int ProgramivSuccess;
    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &ProgramivSuccess);
    if (!ProgramivSuccess)
    {
        glGetProgramInfoLog(ShaderProgram, 512, NULL, infoLog);
        std::cout << "glGetProgramiv Error info Log:" << infoLog << std::endl;
    }
    return ShaderProgram;
}
void FN::MouseCallbacks(GLFWwindow *window, int button, int action, int mods)
{
    double mouseX;
    double mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    std::cout << "X is" << mouseX << "Y is" << mouseY << std::endl;
    if (mouseX == 1000 && mouseY == 500 && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        std::cout << "1000 and 500!" << std::endl;
    }
}
void FN::framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
