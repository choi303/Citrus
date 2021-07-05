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
	GameObject object;
	GameObject nanosuit;
	SkyBox pSkyBox;
	CPUInfo pCPU;
	PointLight pPointLight;
	bool vsync = true;
public:
	wrl::ComPtr<ID3D11Device> pDevice;
	wrl::ComPtr<ID3D11DeviceContext> pContext;
	wrl::ComPtr<IDXGISwapChain> pChain = nullptr;
	wrl::ComPtr<ID3D11RenderTargetView> pRtv = nullptr;
	wrl::ComPtr<ID3D11DepthStencilView> pDSV = nullptr;
	wrl::ComPtr<ID3D11DepthStencilState> pDepthState = nullptr;
	wrl::ComPtr<IDXGIAdapter> pAdapter;
private:
	std::unique_ptr<Texture> sphereTex;
	std::unique_ptr<Texture> envTex;
	int width;
	int height;
};