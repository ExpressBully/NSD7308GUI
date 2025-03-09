#include "TextWidget.h"

TextWidget::TextWidget()
{
}

TextWidget::~TextWidget()
{
}

void TextWidget::Render()
{
    // Create a main window
    ImGui::Begin("Main Window");
    static bool show_popup = false;
    // Button to open a popup
    if (ImGui::Button("Open Popup")) {
        show_popup = true;
        ImGui::OpenPopup("My Popup");
    }

    // Popup
    if (show_popup) {
        if (ImGui::BeginPopupModal("My Popup", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("This is a popup!");

            // Button to close the popup
            if (ImGui::Button("Close")) {
                show_popup = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    // End the main window
    ImGui::End();
}
