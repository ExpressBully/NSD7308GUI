#include "MSCWindow.h"

static void GLFWErrorCallback(int error, const char* description)
{
    std::cout << ("GLFW Error ({0}): {1}", error, description) << std::endl;
}

MSCWindow::MSCWindow(unsigned int width, unsigned int height, const std::string& title)
{
    Init(width, height, title);
}

MSCWindow::~MSCWindow()
{
    Shutdown();
}

void MSCWindow::OnUpdate()
{
    glfwPollEvents();
    glfwSwapBuffers(mpWindow);
    glClearColor(0.0f, 0.0f, 0.0f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);
}

const unsigned int MSCWindow::GetWidth() const
{
    return mData.Width;
}

const unsigned int MSCWindow::GetHeight() const
{
    return mData.Height;
}

const bool MSCWindow::IsRunning() const
{
    return mData.IsRunning;
}

float MSCWindow::GetTime() const
{
    return (float)glfwGetTime();
}

GLFWwindow* MSCWindow::GetWindow() const
{
    return mpWindow;
}

void MSCWindow::Init(int width, int height, const std::string& title)
{
    mData.Height = height;
    mData.Width = width;
    mData.Title = title;
    mData.IsRunning = true;

    static bool sbGLFWInitialized = false;

    if (!sbGLFWInitialized)
    {
        int success = glfwInit();
        sbGLFWInitialized = true;
        glfwSetErrorCallback(GLFWErrorCallback);
    }

    {
        mpWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    }
    glfwMakeContextCurrent(mpWindow);
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSetWindowUserPointer(mpWindow, &mData);

    // WindowResizeEvent lambda函数[](){}
    glfwSetWindowSizeCallback(mpWindow, [](GLFWwindow* tpWindow, int width, int height)
        {
            // Update window size   
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(tpWindow);
            data.Width = width;
            data.Height = height;
        });

    // WindowCloseEvent
    glfwSetWindowCloseCallback(mpWindow, [](GLFWwindow* tpWindow)
        {
            // Set isRunning to false to exit main loop
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(tpWindow);
            data.IsRunning = false;
        });
}

void MSCWindow::Shutdown()
{
    if (mpWindow)
    {
        glfwDestroyWindow(mpWindow);
        mpWindow = nullptr;
    }
    glfwTerminate();
}
