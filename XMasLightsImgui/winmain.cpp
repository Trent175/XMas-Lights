#pragma comment(lib, "d3d11.lib") // DirectX library

#include "imgui/imgui.h"           // ImGui core header
#include "imgui/backends/imgui_impl_win32.h"  // ImGui Win32 backend
#include "imgui/backends/imgui_impl_dx11.h"   // ImGui DirectX11 backend
#include <d3d11.h>              // DirectX 11 core header
#include <tchar.h>              // Unicode and multi-byte character header
#include <windows.h>            // Windows API header
#include <iostream>             // IOStream
#include <ConsoleHandler.h>     // Basic console functions
#include <resource.h>           // Application resources
#include <render.h>             // Rendering functions
#include <callbacks.h>
#include <guicreate.h>

// DirectX global objects
extern ID3D11Device* g_pd3dDevice;              // DirectX 11 device
extern ID3D11DeviceContext* g_pd3dDeviceContext; // DirectX 11 device context
extern IDXGISwapChain* g_pSwapChain;            // Swap chain for double buffering
extern ID3D11RenderTargetView* g_mainRenderTargetView; // Render target view

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);               // Initializes DirectX 11
void CleanupDeviceD3D();                       // Cleans up DirectX 11 resources
void CreateRenderTarget();                     // Creates the render target view
void CleanupRenderTarget();                    // Cleans up the render target view

// Configurations
const float background_color[4] = { 0.05f, 0.05f, 0.05f, 1.00f }; // Background color

// Entry point of the program
int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nShowCmd)
{
    CreateConsole();

    // Define and register the application window class
    WNDCLASSEXW wc = {
        sizeof(wc),                 // Size of the structure
        CS_CLASSDC,                 // Window class style
        WndProc,                    // Window procedure callback
        0L, 0L,                     // Extra memory for class and instance
        GetModuleHandle(nullptr),   // Application instance handle
        nullptr,                    // Default icon
        nullptr,                    // Default cursor
        nullptr,                    // Background brush (none)
        nullptr,                    // Menu name
        L"ImGui",           // Window class name
        nullptr                     // Small icon
    };
    RegisterClassExW(&wc);

    // Load the menu instance
    HMENU hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU));

    // Create the application window
    HWND hwnd = CreateWindowW(
        wc.lpszClassName,           // Window class name
        L"XMas Lights", // Window title
        WS_OVERLAPPEDWINDOW,        // Window style
        100, 100, 1280, 800,        // Initial position and size
        nullptr,                    // Parent window
        hMenu,                    // Menu
        wc.hInstance,               // Instance handle
        nullptr                     // Additional parameters
    );

    // Initialize DirectX 11
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show and update the window
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();          // Ensure the correct version of ImGui is used
    ImGui::CreateContext();        // Create ImGui context
    ImGuiIO& io = ImGui::GetIO(); (void)io; // Get ImGui IO structure
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable keyboard navigation
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable gamepad navigation

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();      // Use the dark style theme

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);    // Initialize Win32 backend
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext); // Initialize DirectX 11 backend

    //KillConsole();

    // Main application loop
    bool done = false;
    while (!done)
    {
        // Process Windows messages
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT) // Check for quit message
                done = true;
        }
        if (done)
            break;

        // Start a new frame for Dear ImGui
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        Render(hwnd);

        // Render the ImGui frame
        ImGui::Render();
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr); // Set render target
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, background_color); // Clear screen
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // Render ImGui data

        g_pSwapChain->Present(1, 0); // Present the frame
    }

    // Cleanup resources
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(hwnd);
    UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}