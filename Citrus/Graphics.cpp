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
		pDriverType,
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
		pDriverType = D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_REFERENCE;
		hr = D3D11CreateDeviceAndSwapChain(
			nullptr,
			pDriverType,
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
			Error::Log(hr, "Failed to create swap chain.");
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
	ImGuiIO& io = ::ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
	io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;
	io.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport; // We can set io.MouseHoveredViewport correctly (optional, not easy)
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(pDevice.Get(), pContext.Get());
	//style of imgui windows
	const auto ColorFromBytes = [](const uint8_t r, const uint8_t g, const uint8_t b)
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

	dsHDR = std::make_unique<DepthStencil>(pDevice.Get(),
		pContext.Get(), width, height, msaaQuality,
		msaaEnabled, DepthStencil::Usage::ShadowDepth);

	dsBrightness = std::make_unique<DepthStencil>(pDevice.Get(),
		pContext.Get(), width, height, msaaQuality,
		msaaEnabled, DepthStencil::Usage::ShadowDepth);

	dsBloom = std::make_unique<DepthStencil>(pDevice.Get(),
		pContext.Get(), width, height, msaaQuality,
		msaaEnabled, DepthStencil::Usage::ShadowDepth);

	dsNormal = std::make_unique<DepthStencil>(pDevice.Get(),
		pContext.Get(), width, height, msaaQuality,
		msaaEnabled, DepthStencil::Usage::ShadowDepth);

	//Render Target(s) initialize
	rtDepth = std::make_unique<RenderTarget>(pDevice.Get(),
		pContext.Get(), width, height, msaaQuality,
		msaaEnabled, RenderTarget::Usage::AO);

	rt = std::make_unique<RenderTarget>(pDevice.Get(),
		pContext.Get(), width, height, msaaQuality,
		msaaEnabled, RenderTarget::Usage::Default);

	rtNoise = std::make_unique<RenderTarget>(pDevice.Get(),
		pContext.Get(), width, height, msaaQuality,
		msaaEnabled, RenderTarget::Usage::Noise);

	rtHDR = std::make_unique<RenderTarget>(pDevice.Get(),
		pContext.Get(), width, height, msaaQuality,
		msaaEnabled, RenderTarget::Usage::HDR);

	rtBrightness = std::make_unique<RenderTarget>(pDevice.Get(),
		pContext.Get(), width, height, msaaQuality,
		msaaEnabled, RenderTarget::Usage::HDR);

	rtBloom = std::make_unique<RenderTarget>(pDevice.Get(),
		pContext.Get(), width, height, msaaQuality,
		msaaEnabled, RenderTarget::Usage::HDR);

	rtNormal = std::make_unique<RenderTarget>(pDevice.Get(),
		pContext.Get(), width, height, msaaQuality,
		msaaEnabled, RenderTarget::Usage::HDR);

	//Full screen quad(s) initialize
	quad = std::make_unique<FSQuad>(pDevice.Get(),
		pContext.Get(), width, height);

	//Light(s) initialize
	pDirectLight = std::make_unique<DirectionalLight>(pDevice.Get(), pContext.Get(),
		width, height);

	//Object(s) initialize
	gridMap.init(pDevice.Get(), pContext.Get());

	pSkyBox.Init(pDevice.Get(), pContext.Get());

	tessPlane.Init(pDevice.Get(), pContext.Get());

	pPbrSphere = std::make_unique<PbrSphere>(pDevice.Get(), pContext.Get());

	//Particle(s) initialize
	mParticle.Initialize(pDevice.Get(), "Images\\star.dds", pContext.Get());

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
	ImGuiIO& io = ::ImGui::GetIO();
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	// Update and Render additional Platform Windows
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
	if (vsync)
		pChain->Present(1u, 0u);
	else
		pChain->Present(0u, 0u);
}

bool Graphics::SceneGraph(Camera3D cam3D)
{
	//set depth stensil state
	pContext->OMSetDepthStencilState(pDepthState.Get(), 0xFF);
	pCPU.Frame();
	gridMap.draw(cam3D);
	dsShadow->BindTexture(pContext.Get(), 4);

	//set primitive topology
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Drawing Objects
	pSkyBox.Draw(cam3D);
	pDirectLight->DrawUI();
	pDirectLight->BindCB(cam3D, 0);
	for (int i = 0; i < pGameObjects.size(); i++)
	{
		pGameObjects[i]->draw(cam3D);
	}
	mParticle.Render(cam3D);
	tessPlane.Draw(cam3D);
	//set primitive topology
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDirectLight->BindCB(cam3D, 5);
	pPbrSphere->Draw(cam3D);

	//full screen pass
	if (!*GameObject::GetWireframeEnabled())
	{
		pContext->OMSetRenderTargets(1u, pRtv.GetAddressOf(), nullptr);
		rt->BindAsTexture(pContext.Get(), 0);
		rtDepth->BindAsTexture(pContext.Get(), 1);
		rtHDR->BindAsTexture(pContext.Get(), 2);
		rtNoise->BindAsTexture(pContext.Get(), 3);
		rtBrightness->BindAsTexture(pContext.Get(), 4);
		rtBloom->BindAsTexture(pContext.Get(), 5);
		rtNormal->BindAsTexture(pContext.Get(), 6);
		quad->draw(pContext.Get(), cam3D);
	}
	return true;
}

bool Graphics::SceneGraphSSR(Camera3D cam3D)
{
	//set depth stensil state
	pContext->OMSetDepthStencilState(pDepthState.Get(), 0xFF);
	pCPU.Frame();
	gridMap.draw(cam3D);
	dsShadow->BindTexture(pContext.Get(), 4);

	//set primitive topology
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Drawing Objects
	pDirectLight->BindCB(cam3D, 0);
	for (int i = 0; i < pGameObjects.size(); i++)
	{
		pGameObjects[i]->draw(cam3D);
	}
	mParticle.Render(cam3D);
	tessPlane.Draw(cam3D);
	//set primitive topology
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDirectLight->BindCB(cam3D, 5);
	pPbrSphere->Draw(cam3D);

	//full screen pass
	if (!*GameObject::GetWireframeEnabled())
	{
		pContext->OMSetRenderTargets(1u, pRtv.GetAddressOf(), nullptr);
		rt->BindAsTexture(pContext.Get(), 0);
		rtDepth->BindAsTexture(pContext.Get(), 1);
		rtHDR->BindAsTexture(pContext.Get(), 2);
		rtNoise->BindAsTexture(pContext.Get(), 3);
		rtBrightness->BindAsTexture(pContext.Get(), 4);
		rtBloom->BindAsTexture(pContext.Get(), 5);
		rtNormal->BindAsTexture(pContext.Get(), 6);
		quad->draw(pContext.Get(), cam3D);
	}
	return true;
}

void Graphics::Render()
{
	//render docking window
	RenderDockingWindow();

	//Particle(s) frame
	mParticle.Frame(static_cast<float>(timer.GetMilisecondsElapsed()));

	//Shadow map pass from light view
	ID3D11RenderTargetView* rtv[1] = { 0 };
	pContext->OMSetRenderTargets(1, rtv, dsShadow->pDepthStencilView.Get());
	dsShadow->Clear(pContext.Get());
	GameObject::SetBackCull(true);
	SceneGraph(pDirectLight->GetLightCamera());
	GameObject::SetBackCull(false);

	//Depth pass
	rtDepth->BindAsTarget(pContext.Get(), dsDepth->pDepthStencilView.Get());
	float col[4] = { 0,0,0,1 };
	rtDepth->Clear(pContext.Get(), col);
	dsDepth->Clear(pContext.Get());
	GameObject::SetDepthBufferEnabled(TRUE);
	SceneGraphSSR(cam3D);
	GameObject::SetDepthBufferEnabled(FALSE);

	//Random noise texture render
	rtNoise->BindAsTarget(pContext.Get(), dsNoise->pDepthStencilView.Get());
	dsNoise->Clear(pContext.Get());
	SceneGraph(cam3D);

	//High dynamic range render
	rtHDR->BindAsTarget(pContext.Get(), dsHDR->pDepthStencilView.Get());
	rtHDR->Clear(pContext.Get(), col);
	dsHDR->Clear(pContext.Get());
	SceneGraph(cam3D);

	//Brightness render
	rtBrightness->BindAsTarget(pContext.Get(), dsBrightness->pDepthStencilView.Get());
	rtBrightness->Clear(pContext.Get(), col);
	dsBrightness->Clear(pContext.Get());
	pDirectLight->SetBrightnessRenderEnabled(TRUE);
	SceneGraph(cam3D);
	pDirectLight->SetBrightnessRenderEnabled(FALSE);

	//Normals render
	rtNormal->BindAsTarget(pContext.Get(), dsNormal->pDepthStencilView.Get());
	rtNormal->Clear(pContext.Get(), col);
	dsNormal->Clear(pContext.Get());
	pDirectLight->SetNormalsEnabled(TRUE);
	SceneGraphSSR(cam3D);
	pDirectLight->SetNormalsEnabled(FALSE);

	//Bloom render
	rtBloom->BindAsTarget(pContext.Get(), dsBloom->pDepthStencilView.Get());
	rtBloom->Clear(pContext.Get(), col);
	dsBloom->Clear(pContext.Get());
	FSQuad::SetBloomRenderEnabled(true);
	quad->draw(pContext.Get(), cam3D);
	FSQuad::SetBloomRenderEnabled(false);

	UI::SetCanRendered(true);
	
	//Render scene as normally
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

D3D_DRIVER_TYPE Graphics::GetDriverType() const
{
	return pDriverType;
}

inline void Graphics::RenderDockingWindow() noexcept
{
	//Creating invisible dock space for docking windows
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x - 8.0f, viewport->Pos.y + 10.0f));
	ImGui::SetNextWindowSize(ImVec2(viewport->Size.x + 15.0f, viewport->Size.y + 2.0f));
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::PopStyleVar(3);
	ImGuiID dockSpaceId = ImGui::GetID("InvisibleWindowDockSpace");
	if (ImGui::Begin("DockSpaceWindow", nullptr, windowFlags))
	{
		ImGui::DockSpace(dockSpaceId, ImVec2(static_cast<float>(width), static_cast<float>(height)), ImGuiDockNodeFlags_PassthruCentralNode);
	}
	ImGui::End();
}
