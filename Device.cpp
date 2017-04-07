#include "Device.h"
#include "Defines.h"


//--------------------------------------------------------------------------------------
// Global Scene Variables
//--------------------------------------------------------------------------------------

// The main D3D interface, this pointer is used to access most D3D functions (and is shared across all cpp files through Defines.h)
ID3D10Device* g_pd3dDevice;


// Variables used to setup D3D
IDXGISwapChain*         SwapChain;
ID3D10Texture2D*        DepthStencil;
ID3D10DepthStencilView* DepthStencilView;
ID3D10RenderTargetView* RenderTargetView;

//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
bool InitDevice(HWND hWnd)
{
	// Many DirectX functions return a "HRESULT" variable to indicate success or failure. Microsoft code often uses
	// the FAILED macro to test this variable, you'll see it throughout the code - it's fairly self explanatory.
	HRESULT hr = S_OK;


	////////////////////////////////
	// Initialise Direct3D

	// Calculate the visible area the window we are using - the "client rectangle" refered to in the first function is the 
	// size of the interior of the window, i.e. excluding the frame and title
	RECT rc;
	GetClientRect(hWnd, &rc);
	g_ViewportWidth = rc.right - rc.left;
	g_ViewportHeight = rc.bottom - rc.top;


	// Create a Direct3D device (i.e. initialise D3D), and create a swap-chain (create a back buffer to render to)
	DXGI_SWAP_CHAIN_DESC sd;         // Structure to contain all the information needed
	ZeroMemory(&sd, sizeof(sd)); // Clear the structure to 0 - common Microsoft practice, not really good style
	sd.BufferCount = 1;
	sd.BufferDesc.Width = g_ViewportWidth;             // Target window size
	sd.BufferDesc.Height = g_ViewportHeight;           // --"--
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Pixel format of target window
	sd.BufferDesc.RefreshRate.Numerator = 60;          // Refresh rate of monitor
	sd.BufferDesc.RefreshRate.Denominator = 1;         // --"--
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.OutputWindow = hWnd;                            // Target window
	sd.Windowed = TRUE;                                // Whether to render in a window (TRUE) or go fullscreen (FALSE)
	hr = D3D10CreateDeviceAndSwapChain(NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, 0,
		D3D10_SDK_VERSION, &sd, &SwapChain, &g_pd3dDevice);
	if (FAILED(hr)) return false;


	// Specify the render target as the back-buffer - this is an advanced topic. This code almost always occurs in the standard D3D setup
	ID3D10Texture2D* pBackBuffer;
	hr = SwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr)) return false;
	hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &RenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr)) return false;


	// Create a texture (bitmap) to use for a depth buffer
	D3D10_TEXTURE2D_DESC descDepth;
	descDepth.Width = g_ViewportWidth;
	descDepth.Height = g_ViewportHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D10_USAGE_DEFAULT;
	descDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = g_pd3dDevice->CreateTexture2D(&descDepth, NULL, &DepthStencil);
	if (FAILED(hr)) return false;

	// Create the depth stencil view, i.e. indicate that the texture just created is to be used as a depth buffer
	D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = g_pd3dDevice->CreateDepthStencilView(DepthStencil, &descDSV, &DepthStencilView);
	if (FAILED(hr)) return false;

	// Select the back buffer and depth buffer to use for rendering now
	g_pd3dDevice->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);


	// Setup the viewport - defines which part of the window we will render to, almost always the whole window
	D3D10_VIEWPORT vp;
	vp.Width = g_ViewportWidth;
	vp.Height = g_ViewportHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_pd3dDevice->RSSetViewports(1, &vp);

	return true;
}

void ReleaseDevice()
{
	if (g_pd3dDevice)  g_pd3dDevice->ClearState(); // Resets Direct3D internal state to normal

	if (DepthStencilView)  DepthStencilView->Release();
	if (RenderTargetView)  RenderTargetView->Release();
	if (DepthStencil)      DepthStencil->Release();
	if (SwapChain)         SwapChain->Release();
	if (g_pd3dDevice)      g_pd3dDevice->Release();
}