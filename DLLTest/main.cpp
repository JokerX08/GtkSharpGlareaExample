#include <stdio.h>
#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <Windows.h>
#include <chrono>
#include <GLFW/glfw3.h>

#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCALL(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__));

GLFWwindow* glfw_window;
static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[Opengl Error] (" << error << "): " << function
                  << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

bool g_IsInit = false;

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};


int location = 0;
float r = 0.0f;
float increment = 0.005f;

static ShaderProgramSource ParseShader(std::string shaderPath)
{
    std::ifstream stream(shaderPath);
    std::string line;

    enum class ShaderType
    {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };

    ShaderType type = ShaderType::NONE;
    std::stringstream ss[2];
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos)
            {
                type = ShaderType::FRAGMENT;
            }
        }
        else
        {
            ss[(int)type] << line << "\n";
        }
    }

    return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Faild to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    // 检查链接状态
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

// 初始化 GLFW 和创建窗口
void init_glfw() {
    if (!glfwInit()) {
        // 初始化失败
        return;
    }

    // 创建一个隐藏窗口
    glfw_window = glfwCreateWindow(1, 1, "Hidden", NULL, NULL);
    if (!glfw_window) {
        glfwTerminate();
        return;
    }
    glfwHideWindow(glfw_window);
    // Make the context current for GLFW
    glfwMakeContextCurrent(glfw_window);

    glfwSwapInterval(1); // 设置垂直同步
}

auto startTime = std::chrono::high_resolution_clock::now();
unsigned int shader;
// example.cpp  
extern "C" {
    __declspec(dllexport) void InitMap() {  
        //init_glfw();
        // 你的代码逻辑  
        // 例如: 打印 "Hello from DLL!"  
        GLenum err = glewInit();
        if (GLEW_OK != err)
        {
            /* Problem: glewInit failed, something is seriously wrong. */
            fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        }
        fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

        std::cout << glGetString(GL_VERSION) << std::endl;

        float positions[] = {
            -0.5f, -0.5f, 0.1f,   //0
            -0.5f,  0.5f, 0.1f,     //1
             0.5f,  0.5f, 0.1f,     //2
             0.5f, -0.5f,  0.1f     //3
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };


        glEnable(GL_MULTISAMPLE);
        unsigned int vao;
        GLCALL(glGenVertexArrays(1, &vao));
        GLCALL(glBindVertexArray(vao));

        unsigned int buffer;
        GLCALL(glGenBuffers(1, &buffer));
        GLCALL(glBindBuffer(GL_ARRAY_BUFFER, buffer));
        GLCALL(glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(float), positions, GL_STATIC_DRAW));

        GLCALL(glEnableVertexAttribArray(0));
        GLCALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0));

        unsigned int ibo;
        GLCALL(glGenBuffers(1, &ibo));
        GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
        GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW));

        ShaderProgramSource  source = ParseShader("Basic.shader");

        shader = CreateShader(source.VertexSource, source.FragmentSource);
        GLCALL(glUseProgram(shader));
        GLCALL(location = glGetUniformLocation(shader, "u_color"));
        ASSERT(location != -1);
        GLCALL(glUniform4f(location, 0.8f, 0.3f, 0.8f, 1.0f));
    }


    __declspec(dllexport) void DrawMap() {
        if (!g_IsInit)
        {
            InitMap();
            g_IsInit = true;
        }

        //glfwMakeContextCurrent(glfw_window);

        if (r > 1.0f)
        {
            increment = -0.005f;
        }
        else if (r < 0.0f)
        {
            increment = 0.005f;
        }

        r += increment;
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsedTime = currentTime - startTime;
        float angle = elapsedTime.count() * 0.1f; // 旋转速度

        // 旋转矩阵代码保持不变
        float rotationMatrix[16] = {
            cosf(angle), -sinf(angle), 0.0f, 0.0f,
            sinf(angle), cosf(angle),  0.0f, 0.0f,
            0.0f,        0.0f,         1.0f, 0.0f,
            0.0f,        0.0f,         0.0f, 1.0f
        };

        /*char outputdata[100];
        sprintf(outputdata, "Called DrawMap r: %f\n", r);
        OutputDebugStringA(outputdata);*/
        GLCALL(glUniform4f(location, 1.0f, 1.0f, 1.0f, 1.0f));
        int modelLoc = glGetUniformLocation(shader, "model"); // 传递旋转角度给着色器
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, rotationMatrix);
        /* Render here */
        GLCALL(glClearColor(0.2f, 0.4f, 0.8f, 1.0f));
        GLCALL(glClear(GL_COLOR_BUFFER_BIT));
        GLCALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        /*glfwSwapBuffers(glfw_window);
        glfwPollEvents();*/

    }
}
