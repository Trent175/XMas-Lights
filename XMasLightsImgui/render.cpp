#include "imgui/imgui.h"           // ImGui core header
#include "imgui/backends/imgui_impl_win32.h"  // ImGui Win32 backend
#include "imgui/backends/imgui_impl_dx11.h"   // ImGui DirectX11 backend
#include <d3d11.h>              // DirectX 11 core header
#include <tchar.h>              // Unicode and multi-byte character header
#include <windows.h>            // Windows API header
#include <iostream>             // IOStream
#include <ConsoleHandler.h>     // Basic console functions
#include "resource.h"           // Application resources
#include <render.h>

// DirectX global objects
ID3D11Device* g_pd3dDevice = nullptr;              // DirectX 11 device
ID3D11DeviceContext* g_pd3dDeviceContext = nullptr; // DirectX 11 device context
IDXGISwapChain* g_pSwapChain = nullptr;            // Swap chain for double buffering
ID3D11RenderTargetView* g_mainRenderTargetView = nullptr; // Render target view

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