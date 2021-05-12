#include "Graphics.h"
#include "imgui\imgui_impl_dx11.h"
#include "imgui\imgui_impl_win32.h"
#include "imgui\imgui.h"
#include <d3dcompiler.h>

static bool wireFrame;
static float Spos[3] = { 0,0,0 };
static float Srot[3] = { 0,0,0 };
static float Sscale[3] = { 1,1,1 };
using namespace std::literals;

bool Graphics::InitializeGraphics(HWND hwnd, const int width, const int height)
{
	timer.Start();
	
	if (!InitDxBase(hwnd, width, height)) 
	{ Error::Log("Failed to init dx11 basement func."); return false; }

	if (!InitShaders()) { Error::Log("Failed to init dx11 basement func."); return false;}

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

	//create rasterizer 
	CD3D11_RASTERIZER_DESC pRasterizerDesc(D3D11_DEFAULT);
	pRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pRasterizerDesc.CullMode = D3D11_CULL_BACK;
	hr = pDevice->CreateRasterizerState(&pRasterizerDesc, &pRasterizer);
	if (FAILED(hr)) { Error::Log(hr, "Failed to create rasterizer state"); return false; }

	//create rasterizer 
	CD3D11_RASTERIZER_DESC pRasterizerDescWireframe(D3D11_DEFAULT);
	pRasterizerDescWireframe.FillMode = D3D11_FILL_WIREFRAME;
	pRasterizerDescWireframe.CullMode = D3D11_CULL_BACK;
	hr = pDevice->CreateRasterizerState(&pRasterizerDescWireframe, &pRasterizerWireframe);
	if (FAILED(hr)) { Error::Log(hr, "Failed to create rasterizer state"); return false; }

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

	//create sampler state
	CD3D11_SAMPLER_DESC sampler_desc(D3D11_DEFAULT);
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampler_desc.MinLOD = 0;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = pDevice->CreateSamplerState(&sampler_desc, &st);
	if (FAILED(hr)) { Error::Log(hr, "Failed to create sampler state"); }

	wic = std::make_unique<WICTexture>(width, height);

	model.Init("models\\nano_textured\\nanosuit.obj", pDevice.Get(), pContext.Get());
	
	pointlight.Init(pDevice.Get(), pContext.Get());
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

bool Graphics::InitShaders()
{
	pvs.Init(L"PhongVS.cso", pDevice.Get());

	pps.Init(L"PhongPS.cso", pDevice.Get());

	//gets data from vertex shader with input layout
	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Texcoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	//create i.l. and bind
	pil = std::make_unique<InputLayout>(pDevice.Get(), ied, &pvs);

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
	//bind rasterizer
	if (!wireFrame)
		pContext->RSSetState(pRasterizer.Get());
	else
		pContext->RSSetState(pRasterizerWireframe.Get());

	pointlight.Draw(pDevice.Get(), pContext.Get(), cam3D);

	pContext->PSSetSamplers(0u, 1u, st.GetAddressOf());
	pil->Bind(pContext.Get());
	pvs.Bind(pContext.Get());
	pps.Bind(pContext.Get());
	model.GetTex().Bind(pContext.Get());
	ui->ClassicUI(&model, "Model", Spos, Srot, Sscale);
	pointlight.BindCB(pContext.Get());
	model.Render(cam3D);

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

	aiString a = model.GetTexturePath();
	int qq = 0;

	return true;
}