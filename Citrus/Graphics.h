#pragma once
#include "GameObject.h"

class Graphics
{
public:
	Graphics() {}
	bool InitializeGraphics(HWND hwnd, const int width, const int height);
	bool InitDxBase(HWND hwnd, const int width, const int height);
	bool InitScene();
	Graphics(Graphics&) = delete;
	~Graphics() {}
public:
	void BeginFrame() noexcept;
	void EndFrame() noexcept;
	bool SceneGraph();
	Camera3D cam3D;
private:
	void FPSCounter();
protected:
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pChain = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRtv = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV = nullptr;
private:
	Timer timer;
	GameObject object;
};
