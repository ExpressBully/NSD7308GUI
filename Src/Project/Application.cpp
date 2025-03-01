#include <memory>

#include "Application.h"

Application::Application()
{
    Initialize();
}

void Application::Run()
{
    while (mpWindow->IsRunning())
    {
        mpWindow->OnUpdate();
        mpImGuiApp->OnUpdate();
    }
}

void Application::Initialize()
{
    mpWindow = std::make_shared<MSCWindow>(1280, 720, "Example");
    mpImGuiApp = std::make_unique<ImGuiApp>(mpWindow, true);
}