#ifndef MSCWINDOW_H
#define MSCWINDOW_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class MSCWindow
{
protected:
    struct WindowData
    {
        std::string Title;
        unsigned int Width;
        unsigned int Height;
        bool IsRunning;
    };
public:
    MSCWindow(unsigned int width = 1280, unsigned int height = 720, const std::string & = "MSCWindow");
    ~MSCWindow();

    void OnUpdate();

    const unsigned int GetWidth() const;
    const unsigned int GetHeight() const;
    const bool IsRunning() const;

    virtual float GetTime() const;
    GLFWwindow* GetWindow() const;

    // ToDo: It may need event system in the future. If so, just contact JZNNMCO.
    // void OnEvent(Event& event);
private:
    void Init(int width, int height, const std::string& title);
    void Shutdown();

private:
    GLFWwindow* mpWindow;
    WindowData mData;
};

#endif // MSCWINDOW_H