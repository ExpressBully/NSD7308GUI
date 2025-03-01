#ifndef APPLICATION_H
#define APPLICATION_H

#include "Window/MSCWindow.h"
#include "ImGuiRender/ImGuiApp.h"

class Application
{
public:
    Application();
    ~Application() {};
    
    void Run();

protected:
    void Initialize();

private:
    std::shared_ptr<MSCWindow> mpWindow;
    std::unique_ptr<ImGuiApp> mpImGuiApp;
};

#endif // APPLICATION_H