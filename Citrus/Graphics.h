#pragma once
#include "GameObject.h"
#include "SkyBox.h"
#include "CPUInfo.h"

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
	bool SceneGraph();
	DXGI_ADAPTER_DESC GetAdapterDesc() const;
	Camera3D cam3D;
	Timer timer;
	SkyBox pSkyBox;
	GameObject pObject;
	CPUInfo pCPU;
	PointLight pPointLight;
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