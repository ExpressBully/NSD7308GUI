#ifndef IMGUI_APP_H
#define IMGUI_APP_H

#include <vector>

#include <imgui/imgui.h>
#include "ImGuiBackend/ImGuiOpengl/imgui_impl_opengl3.h"
#include "ImGuiBackend/ImGuiGlfw/imgui_impl_glfw.h"
#include "../Window/MSCWindow.h"

class WidgetBase;
class ImGuiApp
{
public:
    enum class ImGuiSkin
    {
        Dark,
        Light
        // ...
    };
public:
    ImGuiApp(std::weak_ptr<MSCWindow> pWindow, bool bDockingEnabled = false);
    ~ImGuiApp() {};

    void OnUpdate();
    void SetDockingEnabled(bool bEnabled);

    // Todo: Set ImGui Skin
    // void SetImGuiSkin(ImGuiSkin skin);
private:
    void ImGuiInit();
    void ImGuiBeginRender();
    void ImGuiRenderWidgets();
    void ImGuiEndRender();

    void DockingSpace();

    void SetDarkThemeColors();
private:
    bool mbDockingEnabled;
    std::weak_ptr<MSCWindow> mpWindow;
    std::vector<std::shared_ptr<WidgetBase>> mWidgets;
};

#endif // !IMGUI_APP_H