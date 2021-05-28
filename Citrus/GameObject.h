#pragma once
#include "GfxBase.h"

class GameObject
{
public:
	GameObject() = default;
	GameObject(GameObject&) = default;
	bool Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, std::string filepath);
	void Draw(Camera3D cam);
	~GameObject() = default;
private:
	Model pModel;
	VertexShader pVS;
	PixelShader pPS;
	PointLight pointlight;
private:
	ID3D11Device* pDevice;
	ID3D11DeviceContext* pContext;
	std::unique_ptr<InputLayout> pLayout;
	std::unique_ptr<UI> ui;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> st;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizer = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizerWireframe = nullptr;
};

