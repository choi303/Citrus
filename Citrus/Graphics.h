#pragma once
#include "GameObject.h"
#include "SkyBox.h"
#include "CPUInfo.h"
#include "GridMap.h"
#include "DepthStencil.h"
#include "RenderTarget.h"
#include "FSQuad.h"
#include "DirectionalLight.h"

class Graphics
{
public:
	Graphics() {}
	Graphics(const Graphics&) {}
	~Graphics() {}
	bool InitializeGraphics(HWND hwnd, const int width, const int height);
	bool InitDxBase(HWND hwnd);
	bool InitScene();
public:
	void BeginFrame() const noexcept;
	void EndFrame() const noexcept;
	bool SceneGraph(Camera3D cam3D);
	void Render();
	DXGI_ADAPTER_DESC GetAdapterDesc() const;
	Camera3D cam3D;
	Timer timer;
	GameObject object2;
	SkyBox pSkyBox;
	CPUInfo pCPU;
	std::unique_ptr<DirectionalLight> pDirectLight;
	GridMap gridMap;
	std::unique_ptr<DepthStencil> ds;
	std::unique_ptr<DepthStencil> dsShadow;
	std::unique_ptr<RenderTarget> rt;
	std::unique_ptr<FSQuad> quad;
	bool vsync = true;
	UINT msaaQuality = 1;
	bool msaaEnabled;
public:
	wrl::ComPtr<ID3D11Device> pDevice;
	wrl::ComPtr<ID3D11DeviceContext> pContext;
	wrl::ComPtr<IDXGISwapChain> pChain = nullptr;
	wrl::ComPtr<ID3D11RenderTargetView> pRtv = nullptr;
	wrl::ComPtr<ID3D11DepthStencilView> pDSV = nullptr;
	wrl::ComPtr<ID3D11DepthStencilState> pDepthState = nullptr;
	wrl::ComPtr<IDXGIAdapter> pAdapter;
private:
	int width;
	int height;
};