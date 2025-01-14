#include "guicreate.h"
#include <imgui/imgui.h>
#include "imgui/backends/imgui_impl_win32.h"
#include "imgui/backends/imgui_impl_dx11.h"
#include <Windows.h>
#include <iostream>

const int channels = 8;
const float ratio = (1.0f / 10.0f);

void Render(HWND hWnd) {
    int width = 1280;
    int height = 880;
    RECT rect;
    if (GetWindowRect(hWnd, &rect)) {
        width = rect.right - rect.left;
        height = rect.bottom - rect.top;
    }

    // Modify style to remove padding
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(0, 0);
    float scrollbarWidth = style.ScrollbarSize;

    // Begin main window
    ImGui::Begin("Editor", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
    ImGui::SetWindowPos(ImVec2(0, height / 4));
    ImGui::SetWindowSize(ImVec2(width, height));

    int childwidth = width - scrollbarWidth;
    int childheight = height - (57.5f) - (height / 4);

    // Begin the scrollable TrackVertical frame
    ImVec2 childSize(childwidth, childheight);
    ImGui::BeginChild("TrackVertical", childSize, true, ImGuiWindowFlags_HorizontalScrollbar);

    // Calculate content height based on number of items
    float contentHeight = 100 * ImGui::GetTextLineHeightWithSpacing(); // Adjust based on your content

    // Create a non-scrollable container for left and right frames
    ImGui::BeginChild("FramesContainer", ImVec2(childwidth - scrollbarWidth, contentHeight), false,
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    // Left frame (non-scrollable)
    ImVec2 leftFrameSize(childwidth * ratio, contentHeight);
    ImGui::BeginChild("LeftFrame", leftFrameSize, true,
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    for (int i = 0; i < 100; ++i) {
        ImGui::Text("Line %d", i);
    }
    ImGui::EndChild();

    // Right frame (non-scrollable)
    ImGui::SameLine();
    ImVec2 rightFrameSize((childwidth * (1 - ratio)) - scrollbarWidth, contentHeight);
    ImGui::BeginChild("RightFrame", rightFrameSize, true,
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    for (int i = 0; i < 100; ++i) {
        ImGui::Text("Line %d", i);
    }
    ImGui::EndChild();

    ImGui::EndChild(); // End FramesContainer
    ImGui::EndChild(); // End TrackVertical
    ImGui::End(); // End main window
}