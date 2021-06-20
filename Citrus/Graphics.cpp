#include "Graphics.h"
#include "imgui\imgui_impl_dx11.h"
#include "imgui\imgui_impl_win32.h"

DXGI_ADAPTER_DESC pAdapterDesc;
bool Graphics::InitializeGraphics(HWND hwnd, const int width, const int height)
{
	timer.Start();
	pCPU.Init();
	this->width = width;
	this->height = height;
	if (!InitDxBase(hwnd))
	{
		Error::Log("Failed to init dx11 basement func."); return false;
	}

	if (!InitScene())
	{
		Error::Log("Failed to initialize scene."); return false;
	}

	return true;
}

bool Graphics::InitDxBase(HWND hwnd)
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

	UINT debug_flag = 0;
#ifndef _DEBUG
	debug_flag = D3D11_CREATE_DEVICE_DEBUG;
#endif
	HRESULT hr;
	hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		debug_flag,
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
	hr = pDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, &pRtv);
	if (FAILED(hr)) { Error::Log(hr, "Failed to create RTV."); return false; }

	//create depth stencil state
	CD3D11_DEPTH_STENCIL_DESC dsDesc(D3D11_DEFAULT);
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	hr = pDevice->CreateDepthStencilState(&dsDesc, &pDepthState);
	if (FAILED(hr)) { Error::Log(hr, "Failed to create depths stencil state check out description."); return false; }

	//create depth stencil texture
	wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
	CD3D11_TEXTURE2D_DESC stencilDesc = {};
	stencilDesc.Width = width;
	stencilDesc.Height = height;
	stencilDesc.MipLevels = 1u;
	stencilDesc.ArraySize = 1u;
	stencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	stencilDesc.SampleDesc.Count = 1u;
	stencilDesc.SampleDesc.Quality = 0u;
	stencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	hr = pDevice->CreateTexture2D(&stencilDesc, nullptr, &pDepthStencil);
	if (FAILED(hr)) { Error::Log(hr, "Failed to create texture 2d in graphics.cpp"); return false; }

	//create depth stencil view
	CD3D11_DEPTH_STENCIL_VIEW_DESC DSVdesc = {};
	DSVdesc.Format = stencilDesc.Format;
	DSVdesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DSVdesc.Texture2D.MipSlice = 0u;
	hr = pDevice->CreateDepthStencilView(pDepthStencil.Get(), &DSVdesc, &pDSV);
	if (FAILED(hr)) { Error::Log(hr, "Failed to create texture 2d in depth stencil view"); return false; }

	pContext->OMSetRenderTargets(1u, pRtv.GetAddressOf(), pDSV.Get());

	//create viewport
	D3D11_VIEWPORT vp;
	vp.Width = static_cast<FLOAT>(width);
	vp.Height = static_cast<FLOAT>(height);
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pContext->RSSetViewports(1u, &vp);

	//read adapter data (video card data)
	Microsoft::WRL::ComPtr<IDXGIFactory> pFactory;
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(pFactory.GetAddressOf()));
	if (FAILED(hr)) { Error::Log(hr, "Failed to create dxgi factory."); return false; }
	pFactory->EnumAdapters(0u, pAdapter.GetAddressOf());
	pAdapter->GetDesc(&pAdapterDesc);

	cam3D.SetPosition(0.0f, 0.0f, -2.0f);
	//camera 3d
	cam3D.SetProjectionValues(70.0f, static_cast<float>(width) / static_cast<float>(height), 1.0f, 99999.0f);

	//set primitive topology
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//imgui init
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(pDevice.Get(), pContext.Get());
	//style of imgui windows
	constexpr auto ColorFromBytes = [](const uint8_t r, const uint8_t g, const uint8_t b)
	{
		return ImVec4(static_cast<float>(r) / 255.0f, static_cast<float>(g) / 255.0f, static_cast<float>(b) / 255.0f, 1.0f);
	};

	auto& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;

	const ImVec4 bgColor = ColorFromBytes(37, 37, 38);
	const ImVec4 lightBgColor = ColorFromBytes(82, 82, 85);
	const ImVec4 veryLightBgColor = ColorFromBytes(90, 90, 95);

	const ImVec4 panelColor = ColorFromBytes(51, 51, 55);
	const ImVec4 panelHoverColor = ColorFromBytes(29, 151, 236);
	const ImVec4 panelActiveColor = ColorFromBytes(0, 119, 200);

	const ImVec4 textColor = ColorFromBytes(255, 255, 255);
	const ImVec4 textDisabledColor = ColorFromBytes(151, 151, 151);
	const ImVec4 borderColor = ColorFromBytes(78, 78, 78);

	colors[ImGuiCol_Text] = textColor;
	colors[ImGuiCol_TextDisabled] = textDisabledColor;
	colors[ImGuiCol_TextSelectedBg] = panelActiveColor;
	colors[ImGuiCol_WindowBg] = bgColor;
	colors[ImGuiCol_ChildBg] = bgColor;
	colors[ImGuiCol_PopupBg] = bgColor;
	colors[ImGuiCol_Border] = borderColor;
	colors[ImGuiCol_BorderShadow] = borderColor;
	colors[ImGuiCol_FrameBg] = panelColor;
	colors[ImGuiCol_FrameBgHovered] = panelHoverColor;
	colors[ImGuiCol_FrameBgActive] = panelActiveColor;
	colors[ImGuiCol_TitleBg] = bgColor;
	colors[ImGuiCol_TitleBgActive] = bgColor;
	colors[ImGuiCol_TitleBgCollapsed] = bgColor;
	colors[ImGuiCol_MenuBarBg] = panelColor;
	colors[ImGuiCol_ScrollbarBg] = panelColor;
	colors[ImGuiCol_ScrollbarGrab] = lightBgColor;
	colors[ImGuiCol_ScrollbarGrabHovered] = veryLightBgColor;
	colors[ImGuiCol_ScrollbarGrabActive] = veryLightBgColor;
	colors[ImGuiCol_CheckMark] = panelActiveColor;
	colors[ImGuiCol_SliderGrab] = panelHoverColor;
	colors[ImGuiCol_SliderGrabActive] = panelActiveColor;
	colors[ImGuiCol_Button] = panelColor;
	colors[ImGuiCol_ButtonHovered] = panelHoverColor;
	colors[ImGuiCol_ButtonActive] = panelHoverColor;
	colors[ImGuiCol_Header] = panelColor;
	colors[ImGuiCol_HeaderHovered] = panelHoverColor;
	colors[ImGuiCol_HeaderActive] = panelActiveColor;
	colors[ImGuiCol_Separator] = borderColor;
	colors[ImGuiCol_SeparatorHovered] = borderColor;
	colors[ImGuiCol_SeparatorActive] = borderColor;
	colors[ImGuiCol_ResizeGrip] = bgColor;
	colors[ImGuiCol_ResizeGripHovered] = panelColor;
	colors[ImGuiCol_ResizeGripActive] = lightBgColor;
	colors[ImGuiCol_PlotLines] = panelActiveColor;
	colors[ImGuiCol_PlotLinesHovered] = panelHoverColor;
	colors[ImGuiCol_PlotHistogram] = panelActiveColor;
	colors[ImGuiCol_PlotHistogramHovered] = panelHoverColor;
	colors[ImGuiCol_DragDropTarget] = bgColor;
	colors[ImGuiCol_NavHighlight] = bgColor;
	colors[ImGuiCol_Tab] = bgColor;
	colors[ImGuiCol_TabActive] = panelActiveColor;
	colors[ImGuiCol_TabUnfocused] = bgColor;
	colors[ImGuiCol_TabUnfocusedActive] = panelActiveColor;
	colors[ImGuiCol_TabHovered] = panelHoverColor;

	style.WindowRounding = 0.0f;
	style.ChildRounding = 0.0f;
	style.FrameRounding = 0.0f;
	style.GrabRounding = 0.0f;
	style.PopupRounding = 0.0f;
	style.ScrollbarRounding = 0.0f;
	style.TabRounding = 0.0f;
	//-------------------------------

	return true;
}

bool Graphics::InitScene()
{
	pPointLight.Init(pDevice.Get(), pContext.Get());
	pObject.Init(pDevice.Get(), pContext.Get(), "Models\\sponza\\sponza.obj", width, height, true);
	pSkyBox.Init(pDevice.Get(), pContext.Get());
	pObject.GetMesh()->SetScale(0.1f, 0.1f, 0.1f);

	return true;
}

void Graphics::BeginFrame() const noexcept
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	//check out app.cpp render frame func for desc
	float bgColor[] = { 0.0f, 0.0f, 0.1f, 1.0f };
	pContext->ClearRenderTargetView(pRtv.Get(), bgColor);
	pContext->ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Graphics::EndFrame() const noexcept
{
	//imgui window init
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	//check out app.cpp render frame func for desc
	pChain->Present(1u, 0u);
}

bool Graphics::SceneGraph()
{
	pContext->OMSetDepthStencilState(pDepthState.Get(), 0);
	pCPU.Frame();
	pPointLight.Draw(cam3D);
	pPointLight.BindCB();
	pObject.Draw(cam3D);
	pSkyBox.Draw(cam3D);
	return true;
}

DXGI_ADAPTER_DESC Graphics::GetAdapterDesc() const
{
	return pAdapterDesc;
}
