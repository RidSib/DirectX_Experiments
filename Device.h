#pragma once
#include <d3d10.h>
//--------------------------------------------------------------------------------------
// Global Scene Variables
//--------------------------------------------------------------------------------------

// The main D3D interface, this pointer is used to access most D3D functions (and is shared across all cpp files through Defines.h)
extern ID3D10Device* g_pd3dDevice;


// Variables used to setup D3D
extern IDXGISwapChain*         SwapChain;
extern ID3D10Texture2D*        DepthStencil;
extern ID3D10DepthStencilView* DepthStencilView;
extern ID3D10RenderTargetView* RenderTargetView;


// Create Direct3D device and swap chain (pass the window to attach Direct3D to)
bool InitDevice(HWND hWnd);

// Release Direct3D objects to free memory when quitting
void ReleaseDevice();