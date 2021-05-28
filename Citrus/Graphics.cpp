#include "Graphics.h"
#include "imgui\imgui_impl_dx11.h"
#include "imgui\imgui_impl_win32.h"
#include <d3dcompiler.h>

bool Graphics::InitializeGraphics(HWND hwnd, const int width, const int height)
{
	timer.Start();

	if (!InitDxBase(hwnd, width, height))
	{
		Error::Log("Failed to init dx11 basement func."); return false;
	}

	if (!InitScene())
	{
		Error::Log("Failed to initialize scene."); return false;
	}

	return true;
}

bool Graphics::InitDxBase(HWND hwnd, const int width, const int height)
{
	namespace wrl = Microsoft::WRL;
	//create device and swap chain
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hwnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	HRESULT hr;
	hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pChain,
		&pDevice,
		nullptr,
		&pContext
	);
	if (FAILED(hr))
	{
		Error::Log(hr, "Failed to create device and swap chain."); return false;
	}

	//create back buffer
	wrl::ComPtr<ID3D11Texture2D> backBuffer;
	hr = pChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	if (FAILED(hr)) { Error::Log(hr, "Failed to create Back buffer."); return false; }
	//create render target view
	hr = pDevice->CreateRenderTargetView(backBuffer.Get(), NULL, &pRtv);
	if (FAILED(hr)) { Error::Log(hr, "Failed to create RTV."); return false; }

	//create depth stencil state
	wrl::ComPtr<ID3D11DepthStencilState> pDepthState = {};
	CD3D11_DEPTH_STENCIL_DESC dsDesc(D3D11_DEFAULT);
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	hr = pDevice->CreateDepthStencilState(&dsDesc, &pDepthState);
	if (FAILED(hr)) { Error::Log(hr, "Failed to create depths stencil state check out description."); return false; }

	pContext->OMSetDepthStencilState(pDepthState.Get(), 1u);

	//create depth stencil texture
	wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
	CD3D11_TEXTURE2D_DESC stencilDesc = {};
	stencilDesc.Width = width;
	stencilDesc.Height = height;
	stencilDesc.MipLevels = 1u;
	stencilDesc.ArraySize = 1u;
	stencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	stencilDesc.SampleDesc.Count = 1u;
	stencilDesc.SampleDesc.Quality = 0u;
	stencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	hr = pDevice->CreateTexture2D(&stencilDesc, nullptr, &pDepthStencil);
	if (FAILED(hr)) { Error::Log(hr, "Failed to create exture 2d in graphics.cpp"); return false; }

	//create depth stencil view
	CD3D11_DEPTH_STENCIL_VIEW_DESC DSVdesc = {};
	DSVdesc.Format = DXGI_FORMAT_D32_FLOAT;
	DSVdesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DSVdesc.Texture2D.MipSlice = 0u;
	hr = pDevice->CreateDepthStencilView(pDepthStencil.Get(), nullptr, &pDSV);
	if (FAILED(hr)) { Error::Log(hr, "Failed to create texture 2d in depth stencil view"); return false; }

	pContext->OMSetRenderTargets(1u, pRtv.GetAddressOf(), pDSV.Get());

	//create viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pContext->RSSetViewports(1u, &vp);

	cam3D.SetPosition(0.0f, 0.0f, -2.0f);
	//camera 3d
	cam3D.SetProjectionValues(70.0f, static_cast<float>(width) / static_cast<float>(height), 0.1f, 3000.0f);

	//set primitive topology
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//imgui init
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(pDevice.Get(), pContext.Get());
	ImGui::StyleColorsClassic();

	return true;
}

bool Graphics::InitScene()
{
	object.Init(pDevice.Get(), pContext.Get(), "Models\\nano_textured\\nanosuit.obj");

	return true;
}

void Graphics::BeginFrame() noexcept
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	//check out app.cpp render frame func for desc
	float bgColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	pContext->ClearRenderTargetView(pRtv.Get(), bgColor);
	pContext->ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void Graphics::EndFrame() noexcept
{
	//imgui window init
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	//check out app.cpp render frame func for desc
	pChain->Present(1u, 0u);
}

bool Graphics::SceneGraph()
{
	object.Draw(cam3D);

	FPSCounter();

	return true;
}

void Graphics::FPSCounter()
{
	//fps counter
	static int fpsCounter = 0;
	fpsCounter += 1;
	std::string fps;
	if (timer.GetMilisecondsElapsed() > 1000.0f)
	{
		fps = "FPS: " + std::to_string(fpsCounter) + "\n";
		fpsCounter = 0;
		timer.Restart();
	}
	OutputDebugStringA(fps.c_str());
}