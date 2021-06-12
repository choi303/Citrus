#pragma once
#include "GfxBase.h"

struct ScreenSizeBuffer
{
	float screenWidth;
	float screenHeight;
	XMFLOAT2 padding;
};

class Blur
{
public:
	Blur() = default;
	void Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const int width, const int height);
	void Draw();
	~Blur() = default;
private:
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	std::unique_ptr<InputLayout> pLayout;
	VertexShader pVS;
	PixelShader pPS;
	std::unique_ptr<CBuffer<ScreenSizeBuffer>> pBlurCBuffer;
	int width;
	int height;
};

