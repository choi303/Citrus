#pragma once
#include "GfxBase.h"

struct param
{
	BOOL blurEnabled;
	float blurIntensity;
	float pad[2];
};

struct fxaa
{
	XMFLOAT4 rcpFrame;
};

struct SSAOBuffer
{
	float totalStrength;
	float base;
	float area;
	float fallOff;
	float radius;
	XMMATRIX proj;
};

class FSQuad
{
public:
	FSQuad(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, int width, int height);
	FSQuad(FSQuad& rhs) {}
	~FSQuad() {}

	void draw(ID3D11DeviceContext* pContext, Camera3D cam);
	static bool* GetBlurEnabled();
	static bool* GetFxaaEnabled();
	static BOOL* GetSSAOEnabled();
	static bool SetFxaaEnabled(bool value);
	static float* GetBlurIntensity();
	static float* GetFallOff();
	static float* GetTotalStrength();
	static float* GetArea();
	static float* GetRadius();
	static float* GetBase();
	static bool SetBlurEnabled(bool value);
	static float SetBlurIntensity(float value);
	static float SetArea(float value);
	static float SetBase(float value);
	static float SetRadius(float value);
	static float SetTotalStrength(float value);
	static float SetFallOff(float value);
	static BOOL SetSSAOEnabled(BOOL value);
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
	std::unique_ptr<CBuffer<fxaa>> fxaaCBuffer;
	std::unique_ptr<CBuffer<SSAOBuffer>> ssaoCBuffer;
	VertexShader pVSBlur;
	PixelShader pPSBlur;
	VertexShader pVSFxaa;
	PixelShader pPSFxaa;
	VertexShader pVSSsao;
	PixelShader pPSSsao;
	int width;
	int height;
};

