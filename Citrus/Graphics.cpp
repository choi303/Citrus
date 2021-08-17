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
	if (msaaEnabled)
		sd.SampleDesc.Count = msaaQuality;
	else
		sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hwnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	UINT debug_flag = 0;
#ifdef _DEBUG
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
		hr = D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_REFERENCE,
			nullptr,
			debug_flag,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			&sd,
			&pChain,
			&pDevice,
			nullptr,
			&pContext);
		if (FAILED(hr))
		{
			Error::Log(hr, "");
		}
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
	stencilDesc.SampleDesc.Count = sd.SampleDesc.Count;
	stencilDesc.SampleDesc.Quality = 0u;
	stencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	hr = pDevice->CreateTexture2D(&stencilDesc, nullptr, &pDepthStencil);
	if (FAILED(hr)) { Error::Log(hr, "Failed to create texture 2d in graphics.cpp"); return false; }

	//create depth stencil view
	CD3D11_DEPTH_STENCIL_VIEW_DESC DSVdesc = {};
	DSVdesc.Format = stencilDesc.Format;
	DSVdesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
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
	cam3D.SetProjectionValues(70.0f, static_cast<float>(width) / static_cast<float>(height), 0.1f, 99999.0f * 99999.0f, false);

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
	//Depth Stencil(s) initialize
	ds = std::make_unique<DepthStencil>(pDevice.Get(),
		pContext.Get(), width, height, msaaQuality,
		msaaEnabled, DepthStencil::Usage::DepthStencil);

	dsShadow = std::make_unique<DepthStencil>(pDevice.Get(),
		pContext.Get(), width, height, msaaQuality,
		msaaEnabled, DepthStencil::Usage::ShadowDepth);

	dsDepth = std::make_unique<DepthStencil>(pDevice.Get(),
		pContext.Get(), width, height, msaaQuality,
		msaaEnabled, DepthStencil::Usage::ShadowDepth);

	dsNoise = std::make_unique<DepthStencil>(pDevice.Get(),
		pContext.Get(), width, height, msaaQuality,
		msaaEnabled, DepthStencil::Usage::ShadowDepth);

	//Render Taret(s) initialize
	rtDepth = std::make_unique<RenderTarget>(pDevice.Get(),
		pContext.Get(), width, height, msaaQuality,
		msaaEnabled, RenderTarget::Usage::AO);

	rt = std::make_unique<RenderTarget>(pDevice.Get(),
		pContext.Get(), width, height, msaaQuality,
		msaaEnabled, RenderTarget::Usage::Default);

	rtNoise = std::make_unique<RenderTarget>(pDevice.Get(),
		pContext.Get(), width, height, msaaQuality,
		msaaEnabled, RenderTarget::Usage::Noise);

	//Full screen quad(s) initialize
	quad = std::make_unique<FSQuad>(pDevice.Get(),
		pContext.Get(), width, height);

	//Light(s) initialize
	pDirectLight = std::make_unique<DirectionalLight>(pDevice.Get(), pContext.Get(),
		width, height);

	//Object(s) initialize
	gridMap.init(pDevice.Get(), pContext.Get());

	pSkyBox.Init(pDevice.Get(), pContext.Get());

	pObject.init(pDevice.Get(), pContext.Get(), "Models\\gobber\\GoblinX.obj", width, height, true);
	pObject.GetMesh()->SetPos(0.0f, 5.0f, 0.0f);
	pObject.GetMesh()->SetScale(2.0f, 2.0f, 2.0f);
	pObject2.init(pDevice.Get(), pContext.Get(), "Models\\brick_wall\\brick_wall.obj", width, height, true);
	pObject2.GetMesh()->SetRot(1.560f, 0.0f, 0.0f);
	pObject2.GetMesh()->SetScale(100.0f, 100.0f, 100.0f);

	//Particle(s) initialize
	mParticle.Initialize(pDevice.Get(), "Images\\star.dds", pContext.Get());

	mFire = std::make_unique<Fire>(pDevice.Get(), pContext.Get());

	return true;
}

void Graphics::BeginFrame() const noexcept
{
	//Imgui frame initialize
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void Graphics::EndFrame() const noexcept
{
	//imgui window init
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	if (vsync)
		pChain->Present(1u, 0u);
	else
		pChain->Present(0u, 0u);
}

bool Graphics::SceneGraph(Camera3D cam3D)
{
	pCPU.Frame();
	gridMap.draw(cam3D);
	dsShadow->BindTexture(pContext.Get(), 4);

	//set primitive topology
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Drawing Objects
	pSkyBox.Draw(cam3D);
	pDirectLight->BindCB(cam3D);
	pObject.draw(cam3D);
	pObject2.draw(cam3D);
	mParticle.Render(cam3D);
	mFire->Draw(cam3D, timer.GetMilisecondsElapsed());
	//full screen pass
	if (!*GameObject::GetWireframeEnabled())
	{
		pContext->OMSetRenderTargets(1u, pRtv.GetAddressOf(), nullptr);
		rt->BindAsTexture(pContext.Get(), 0);
		rtDepth->BindAsTexture(pContext.Get(), 1);
		rtNoise->BindAsTexture(pContext.Get(), 3);
		quad->draw(pContext.Get(), cam3D);
	}
	return true;
}

void Graphics::Render()
{
	//Particle(s) frame
	mParticle.Frame(timer.GetMilisecondsElapsed());

	//depth pass from light view (shadow map)
	ID3D11RenderTargetView* rtv[1] = { 0 };
	pContext->OMSetRenderTargets(1, rtv, dsShadow->pDepthStencilView.Get());
	dsShadow->Clear(pContext.Get());
	GameObject::SetFrontCull(true);
	SceneGraph(pDirectLight->GetLightCamera());
	GameObject::SetFrontCull(false);

	//depth pass
	rtDepth->BindAsTarget(pContext.Get(), dsDepth->pDepthStencilView.Get());
	dsDepth->Clear(pContext.Get());
	GameObject::SetDepthBufferEnabled(TRUE);
	SceneGraph(cam3D);
	GameObject::SetDepthBufferEnabled(FALSE);

	//random noise texture render
	rtNoise->BindAsTarget(pContext.Get(), dsNoise->pDepthStencilView.Get());
	dsNoise->Clear(pContext.Get());
	SceneGraph(cam3D);

	UI::SetCanRendered(true);

	rt->BindAsTarget(pContext.Get(), ds->pDepthStencilView.Get());
	ds->Clear(pContext.Get());
	if (*GameObject::GetWireframeEnabled())
	{
		pContext->OMSetRenderTargets(1u, pRtv.GetAddressOf(), pDSV.Get());
		float bgColor[] = { 0.0f, 0.0f, 0.1f, 1.0f };
		pContext->ClearRenderTargetView(pRtv.Get(), bgColor);
		pContext->ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH
			| D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
	SceneGraph(cam3D);
}

DXGI_ADAPTER_DESC Graphics::GetAdapterDesc() const
{
	//return adapter description
	return pAdapterDesc;
}
