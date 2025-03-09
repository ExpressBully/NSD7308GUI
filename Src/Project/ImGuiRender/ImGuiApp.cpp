#include "ImGuiApp.h"
#include "ImGuiWidget/ExamplePage.h"
#include "ImGuiWidget/TestClass.h"
#include "ImGuiWidget/PWM_Slider.h"
#include "ImGuiWidget/UART_DEC.h"

ImGuiApp::ImGuiApp(std::weak_ptr<MSCWindow> pWindow, bool bDockingEnabled)
    : mpWindow(pWindow),
      mbDockingEnabled(bDockingEnabled)
{
    ImGuiInit();
}

void ImGuiApp::OnUpdate()
{
    ImGuiBeginRender();
    ImGuiRenderWidgets();
    ImGuiEndRender();
}

void ImGuiApp::SetDockingEnabled(bool bEnabled)
{
    mbDockingEnabled = bEnabled;
    ImGuiIO& io = ImGui::GetIO();
    if (bEnabled)
    {
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    }
    else
    {
        io.ConfigFlags &= ~ImGuiConfigFlags_DockingEnable;
    }
}

void ImGuiApp::ImGuiInit()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

    io.FontDefault = io.Fonts->AddFontFromFileTTF(R"(..\..\Resource\Fonts\Deng.ttf)", 18.0f);
    io.Fonts->AddFontFromFileTTF(R"(..\..\Resource\Fonts\Dengb.ttf)", 18.0f);

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    SetDarkThemeColors();
    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    auto tpWindow = static_cast<GLFWwindow*>(mpWindow.lock()->GetWindow());

    if (!io.BackendPlatformUserData)
    {
        ImGui_ImplGlfw_InitForOpenGL(tpWindow, true);
        ImGui_ImplOpenGL3_Init("#version 410");
    }

    // Insert example widgets here 
    /***************
    1. Eample
    2.TestClass:Conclude a botton and PopupModal
    3.PWM_Slider
    4.

    *******************/
    //std::shared_ptr<ExamplePage> pExampleWidget = std::make_shared<ExamplePage>();
    std::shared_ptr<TestClass> pTestClassWidget = std::make_shared<TestClass>();
    std::shared_ptr<PWM_Slider> pPWM_SliderWidget = std::make_shared<PWM_Slider>();
    std::shared_ptr<UART_DEC> pUART_DECWidget = std::make_shared<UART_DEC>();


    //mWidgets.emplace_back(pExampleWidget);
    mWidgets.emplace_back(pTestClassWidget);
    mWidgets.emplace_back(pPWM_SliderWidget);
    mWidgets.emplace_back(pUART_DECWidget);
}

void ImGuiApp::ImGuiBeginRender()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiApp::ImGuiRenderWidgets()
{
    if (mbDockingEnabled)
    {
        DockingSpace();
    }

    for (const auto& tpWidget : mWidgets)
    {
        tpWidget->Render();
    }
}

void ImGuiApp::ImGuiEndRender()
{
    ImGuiIO& io = ImGui::GetIO();
    
    io.DisplaySize = ImVec2((float)mpWindow.lock()->GetWidth(),  (float)mpWindow.lock()->GetHeight());

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

void ImGuiApp::DockingSpace()
{
    static bool dockspaceOpen = true;
    static bool opt_fullscreen_persistant = true;
    bool opt_fullscreen = opt_fullscreen_persistant;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
    ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    float minWinSizeX = style.WindowMinSize.x;
    style.WindowMinSize.x = 370.0f;

    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    style.WindowMinSize.x = minWinSizeX;

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Exit")) { /* Handle exit */ }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::End();
}

void ImGuiApp::SetDarkThemeColors()
{
    auto& colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

    // Headers
    colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

    // Buttons
    colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

    // Frame BG
    colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
    colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
    colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

    // Title
    colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
}
