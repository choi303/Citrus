#pragma once
#include "GfxBase.h"

struct FogBuffer
{
	XMFLOAT4 fogColor;
};

struct FogFactorBuffer
{
	float fogStart;
	float fogEnd;
	XMFLOAT2 pad;
};

class Fog
{
public:
	Fog() = default;
	void Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Draw();
	static XMFLOAT4* GetFogColor();
	static float* GetFogStart();
	static float* GetFogEnd();
	~Fog() = default;
private:
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	std::unique_ptr<InputLayout> pLayout;
	std::unique_ptr<CBuffer<FogBuffer>> pFogCBuffer;
	std::unique_ptr<CBuffer<FogFactorBuffer>> pFogFactorCBuffer;
	VertexShader pVS;
	PixelShader pPS;
};

