#pragma once
#include "GfxBase.h"

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
	bool Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, std::string filepath);
	bool HasNormal() const;
	void Draw(Camera3D cam);
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
};

