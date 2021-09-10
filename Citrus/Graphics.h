#pragma once
#include "GameObject.h"
#include "SkyBox.h"
#include "CPUInfo.h"
#include "GridMap.h"
#include "DepthStencil.h"
#include "RenderTarget.h"
#include "FSQuad.h"
#include "DirectionalLight.h"
#include "Particle.h"
#include "Fire.h"
#include "TessellationPlane.h"
#include "PbrSphere.h"

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
	std::vector<GameObject*> pGameObjects;
	SkyBox pSkyBox;
	CPUInfo pCPU;
	std::unique_ptr<DirectionalLight> pDirectLight;
	GridMap gridMap;
	std::unique_ptr<DepthStencil> ds;
	std::unique_ptr<DepthStencil> dsShadow;
	std::unique_ptr<DepthStencil> dsDepth;
	std::unique_ptr<DepthStencil> dsNoise;
	std::unique_ptr<DepthStencil> dsHDR;
	std::unique_ptr<DepthStencil> dsBrightness;
	std::unique_ptr<DepthStencil> dsBloom;
	std::unique_ptr<RenderTarget> rt;
	std::unique_ptr<RenderTarget> rtDepth;
	std::unique_ptr<RenderTarget> rtHDR;
	std::unique_ptr<RenderTarget> rtNoise;
	std::unique_ptr<RenderTarget> rtBrightness;
	std::unique_ptr<RenderTarget> rtBloom;
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
	int viewportWidth, viewportHeight;
	Particle mParticle;
	std::unique_ptr<Fire> mFire;
	TessellationPlane tessPlane;
	std::unique_ptr<PbrSphere> pPbrSphere;
private:
	inline void RenderDockingWindow() noexcept;

};