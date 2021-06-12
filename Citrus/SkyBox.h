#pragma once
#include "GfxBase.h"

class SkyBox
{
public:
	SkyBox() = default;
	void Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Draw(Camera3D cam);
	void SetPos(XMFLOAT3 pos, float deltaTime);
	~SkyBox()=default;
private:
	Model sphere;
	VertexShader pVS;
	PixelShader pPS;
	std::unique_ptr<InputLayout> pLayout;
	std::unique_ptr<Texture> pTexture;
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizer;
};

