#pragma once
#include "GfxBase.h"

class FSQuad
{
private:
	struct param
	{
		BOOL blurEnabled;
		float blurIntensity;
		float exposure;
		float gamma;
		BOOL toneMappingEnabled;
		BOOL bloomEnabled;
		BOOL autoExposureEnabled;
		float pad;
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
		float pad[3];
	};

	struct bloomBuffer
	{
		float bloomIntensity;
		float pad[3];
	};

	struct ssrBuffer
	{
		XMMATRIX view;
		XMMATRIX proj;
		XMMATRIX invProj;
		XMFLOAT3 camPos;
		float minRaySteps;
		float reflectivity;
		float pad[3];
	};
public:
	FSQuad(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, int width, int height);
	FSQuad(FSQuad& rhs) {}
	~FSQuad() {}

	void draw(ID3D11DeviceContext* pContext, Camera3D cam);
	static bool* GetBlurEnabled();
	static BOOL* GetAutoExposureEnabled();
	static bool* GetFxaaEnabled();
	static BOOL* GetSSAOEnabled();
	static BOOL* GetSSREnabled();
	static BOOL* GetToneMappingEnabled();
	static BOOL* GetBloomEnabled();
	static BOOL* GetKuwaharaEnabled();
	static bool SetFxaaEnabled(bool value);
	static float* GetBlurIntensity();
	static float* GetFallOff();
	static float* GetReflectivity();
	static float* GetTotalStrength();
	static float* GetArea();
	static float* GetRadius();
	static float* GetMinRaySteps();
	static float* GetBase();
	static float* GetExposure();
	static float* GetGamma();
	static float* GetBloomIntensity();
	static bool SetBlurEnabled(bool value);
	static bool SetBloomRenderEnabled(bool value);
	static BOOL SetToneMappingEnabled(BOOL value);
	static BOOL SetKuwaharaEnabled(BOOL value);
	static BOOL SetBloomEnabled(BOOL value);
	static BOOL SetAutoExposureEnabled(BOOL value);
	static float SetBlurIntensity(float value);
	static float SetArea(float value);
	static float SetBase(float value);
	static float SetRadius(float value);
	static float SetTotalStrength(float value);
	static float SetFallOff(float value);
	static float SetExposure(float value);
	static float SetGamma(float value);
	static float SetBloomIntensity(float value);
	static float SetMinRaySteps(float value);
	static float SetReflectivity(float value);
	static BOOL SetSSAOEnabled(BOOL value);
	static BOOL SetSSREnabled(BOOL value);
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
	std::unique_ptr<CBuffer<bloomBuffer>> bloomCBuffer;
	std::unique_ptr<CBuffer<ssrBuffer>> ssrCBuffer;
	VertexShader pVSBlur;
	PixelShader pPSBlur;
	VertexShader pVSFxaa;
	PixelShader pPSFxaa;
	VertexShader pVSSsao;
	PixelShader pPSSsao;
	VertexShader pVSBloom;
	PixelShader pPSBloom;
	VertexShader pVSKuwahara;
	PixelShader pPSKuwahara;
	VertexShader pVSsr;
	PixelShader pPSSsr;
	int width;
	int height;
};

