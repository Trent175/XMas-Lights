// Add the DirectX library
#pragma comment(lib, "d3d11.lib")

#include "imgui/imgui.h"           // ImGui core header
#include "imgui/backends/imgui_impl_win32.h"  // ImGui Win32 backend
#include "imgui/backends/imgui_impl_dx11.h"   // ImGui DirectX11 backend
#include <d3d11.h>              // DirectX 11 core header
#include <tchar.h>              // Unicode and multi-byte character header
#include <windows.h>            // Windows API header
#include <iostream>             // IOStream
#include <ConsoleHandler.h>     // Basic console functions

// DirectX global objects
static ID3D11Device* g_pd3dDevice = nullptr;              // DirectX 11 device
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr; // DirectX 11 device context
static IDXGISwapChain* g_pSwapChain = nullptr;            // Swap chain for double buffering
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr; // Render target view

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);               // Initializes DirectX 11
void CleanupDeviceD3D();                       // Cleans up DirectX 11 resources
void CreateRenderTarget();                     // Creates the render target view
void CleanupRenderTarget();                    // Cleans up the render target view
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); // Windows message handler

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

    // Create the application window
    HWND hwnd = CreateWindowW(
        wc.lpszClassName,           // Window class name
        L"XMas Lights", // Window title
        WS_OVERLAPPEDWINDOW,        // Window style
        100, 100, 1280, 800,        // Initial position and size
        nullptr,                    // Parent window
        nullptr,                    // Menu
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

    KillConsole();

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

        static bool checks[1] = { false };

        // Define ImGui windows and controls
        ImGui::Begin("Test", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
        ImGui::SetWindowPos(ImVec2(0, 0));
        ImGui::SetWindowSize(ImVec2(1280, 880));
        ImGui::StyleColorsDark();
        ImGui::Text("What is this?");
        ImGui::Checkbox("test", &checks[0]);
        ImGui::End();

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

// Initializes DirectX 11
bool CreateDeviceD3D(HWND hWnd)
{
    // Describe the swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;                              // Double buffering
    sd.BufferDesc.Width = 0;                         // Use window dimensions
    sd.BufferDesc.Height = 0;                        // Use window dimensions
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 32-bit color
    sd.BufferDesc.RefreshRate.Numerator = 60;        // Refresh rate numerator
    sd.BufferDesc.RefreshRate.Denominator = 1;       // Refresh rate denominator
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // Allow fullscreen
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // Render target usage
    sd.OutputWindow = hWnd;                          // Target window
    sd.SampleDesc.Count = 1;                         // No multisampling
    sd.SampleDesc.Quality = 0;                       // No multisampling
    sd.Windowed = TRUE;                              // Start in windowed mode
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;        // Discard frames

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;                  // Selected feature level
    const D3D_FEATURE_LEVEL featureLevelArray[2] = {
        D3D_FEATURE_LEVEL_11_0,                      // DirectX 11
        D3D_FEATURE_LEVEL_10_0                       // DirectX 10
    };

    // Attempt to create the device and swap chain
    HRESULT res = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags,
        featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain,
        &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext
    );

    // If hardware acceleration is unsupported, fallback to WARP driver
    if (res == DXGI_ERROR_UNSUPPORTED)
    {
        res = D3D11CreateDeviceAndSwapChain(
            nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags,
            featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain,
            &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext
        );
    }

    // Return failure if initialization failed
    if (res != S_OK)
        return false;

    // Create the render target
    CreateRenderTarget();
    return true;
}

// Cleans up DirectX 11 resources
void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

// Creates the render target view
void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    HRESULT hr = g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    if (FAILED(hr) || pBackBuffer == nullptr)
    {
        // Handle error
        return;
    }

    hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();

    if (FAILED(hr))
    {
        // Handle error
        return;
    }
}

// Cleans up the render target view
void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Windows message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // Let ImGui handle certain messages
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != nullptr && wParam != SIZE_MINIMIZED)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT menu
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0); // Signal application termination
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}