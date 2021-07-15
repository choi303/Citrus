#pragma once
#include "GfxBase.h"

struct param
{
	BOOL blurEnabled;
	float blurIntensity;
	float pad[2];
};

class FSQuad
{
public:
	FSQuad(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	FSQuad(FSQuad& rhs) {}
	~FSQuad() {}

	void draw(ID3D11DeviceContext* pContext);
	static bool* GetBlurEnabled();
	static float* GetBlurIntensity();
	static bool SetBlurEnabled(bool value);
	static float SetBlurIntensity(float value);
private:
	struct vertex
	{
		XMFLOAT2 pos;
	};
	std::vector<vertex> vertices;
	std::unique_ptr<VertexBuffer<vertex>> pVertexBuffer;
	std::unique_ptr<IndexBuffer> pIndexBuffer;
	std::unique_ptr<InputLayout> pLayout;
	std::unique_ptr<CBuffer<param>> paramCBuffer;
	VertexShader pVS;
	PixelShader pPS;
};

