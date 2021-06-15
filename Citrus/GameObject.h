#pragma once
#include "GfxBase.h"
#include "DepthBuffer.h"
#include "Blur.h"
#include "Wireframe.h"
#include "Fog.h"

struct matrices
{
	XMFLOAT3 camera_pos;
	float pad;
};

class GameObject
{
public:
	GameObject() = default;
	GameObject(GameObject&) = default;
	bool Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, std::string filepath, int width, int height);
	bool HasNormal() const;
	static bool* GetDepthBufferEnabled();
	static bool* GetBlurEnabled();
	static bool* GetWireframeEnabled();
	static bool* GetFogEnabled();
	static XMFLOAT3* GetWireColor();
	static XMFLOAT4* GetFogColor();
	static float* GetFogStart();
	static float* GetFogEnd();
	static bool SetDepthBufferEnabled(bool value);
	static bool SetBlurEnabled(bool value);
	static bool SetFogEnabled(bool value);
	static bool SetWireframeEnabled(bool value);
	static float SetFogStart(float value);
	static float SetFogEnd(float value);
	void Draw(Camera3D cam);
	Model* GetMesh();
	~GameObject() = default;
private:
	Model pModel;
	VertexShader pVSNormal;
	VertexShader pVS;
	PixelShader pPS;
	PixelShader pPSNormal;
private:
	ID3D11Device* pDevice;
	ID3D11DeviceContext* pContext;
	std::unique_ptr<InputLayout> pLayoutNormal;
	std::unique_ptr<InputLayout> pLayout;
	std::unique_ptr<CBuffer<matrices>> matrices_buffer;
	std::unique_ptr<UI> ui;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> st;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizer = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizerWireframe = nullptr;
	std::string directory;
	bool is_rendered = false;
	DepthBuffer pDepthBuffer;
	Blur pBlur;
	Wireframe pWireframe;
	Fog pFog;
};

