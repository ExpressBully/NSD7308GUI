#include <memory>

#include "Application.h"

Application::Application()
{
    Initialize();
}

void Application::Run()
{
    mpImGuiApp->SetDockingEnabled(false);
    while (mpWindow->IsRunning())
    {
        mpWindow->OnUpdate();
        mpImGuiApp->OnUpdate();
    }
}

void Application::Initialize()
{
    // make_share 智能指针
    mpWindow = std::make_shared<MSCWindow>(1280, 720, "Example");
    mpImGuiApp = std::make_unique<ImGuiApp>(mpWindow, true);
}  