#pragma once
#include "GfxBase.h"

struct ColorBuffer
{
	XMFLOAT3 wireColor;
	float pad;
};

class Wireframe
{
public:
	Wireframe() = default;
	void Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Draw();
	static XMFLOAT3* GetWireColor();
	~Wireframe() = default;
private:
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	std::unique_ptr<InputLayout> pLayout;
	VertexShader pVS;
	PixelShader pPS;
	std::unique_ptr<CBuffer<ColorBuffer>> pColorCBuffer;
};

