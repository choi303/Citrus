#pragma once
#include "GfxBase.h"
#include "PointLight.h"

class Graphics
{
public:
	Graphics() {}
	bool InitializeGraphics(HWND hwnd, const int width, const int height);
	bool InitDxBase(HWND hwnd, const int width, const int height);
	bool InitShaders();
	Graphics(Graphics&) = delete;
	~Graphics() {}
public:
	void BeginFrame() noexcept;
	void EndFrame() noexcept;
	bool SceneGraph();
	Camera3D cam3D;
protected:
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pChain = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRtv = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizer = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizerWireframe = nullptr;
private:
	//texture
	Microsoft::WRL::ComPtr<ID3D11SamplerState> st;
	Microsoft::WRL::ComPtr<ID3D11BlendState> blends;
	Timer timer;
	VertexShader pvs;
	PixelShader pps;
	Model model;
	PointLight pointlight;
	std::unique_ptr<UI> ui;
	std::unique_ptr<InputLayout> pil;
	//----
};

