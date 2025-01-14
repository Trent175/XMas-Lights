#include "imgui/imgui.h"           // ImGui core header
#include "imgui/backends/imgui_impl_win32.h"  // ImGui Win32 backend
#include "imgui/backends/imgui_impl_dx11.h"   // ImGui DirectX11 backend
#include <d3d11.h>              // DirectX 11 core header
#include <tchar.h>              // Unicode and multi-byte character header
#include <windows.h>            // Windows API header
#include <iostream>             // IOStream
#include <ConsoleHandler.h>     // Basic console functions
#include "resource.h"           // Application resources
#include <callbacks.h>

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
    case WM_COMMAND: // Handle menu commands
        switch (LOWORD(wParam))
        {
        case ID_FILE_EXIT:
            PostQuitMessage(0); // Exit application
            return 0;
        }
        break;
    case WM_SIZE:
        if (g_pd3dDevice != nullptr && wParam != SIZE_MINIMIZED)
        {
            CleanupRenderTarget();

            // Resize the swap chain buffers to match the new window dimensions.
            UINT newWidth = static_cast<UINT>(LOWORD(lParam));  // Extract the new width from lParam
            UINT newHeight = static_cast<UINT>(HIWORD(lParam)); // Extract the new height from lParam

            HRESULT resizeResult = g_pSwapChain->ResizeBuffers(
                0,                 // Keep the same number of buffers
                newWidth,          // New width for the buffers
                newHeight,         // New height for the buffers
                DXGI_FORMAT_UNKNOWN, // Use the existing format
                0                  // No additional options
            );

            // Check if the resizing operation failed and log an error message if it did.
            if (FAILED(resizeResult)) {
                std::cerr << "Error: Unable to resize swap chain buffers." << std::endl;
            }

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