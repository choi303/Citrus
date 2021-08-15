#pragma once
#include "GfxBase.h"

class Fire
{
private:
	struct matricesBuffer
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX proj;
	};

	struct noiseBuffer
	{
		float frameTime;
		XMFLOAT3 scrollSpeeds;
		XMFLOAT3 scales;
		float padding;
	};

	struct distortionBuffer
	{
		XMFLOAT2 distortion1;
		XMFLOAT2 distortion2;
		XMFLOAT2 distortion3;
		float distortionScale;
		float distortionBias;
	};

	struct vertex
	{
		XMFLOAT3 pos;
		XMFLOAT2 tex;
	};
public:
	Fire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	Fire(Fire& rhs) = default;
	~Fire() = default;
	void Draw(Camera3D cam, float frameTime);
public:
	//get funcs (static)
	static float* GetPosX();
	static float* GetPosY();
	static float* GetPosZ();
	static float* GetScaleX();
	static float* GetScaleY();
	static float* GetScaleZ();
	static float* GetSSpeedX();
	static float* GetSSpeedY();
	static float* GetSSpeedZ();
	static float* GetScalesX();
	static float* GetScalesY();
	static float* GetScalesZ();
	static float* GetDistortion1X();
	static float* GetDistortion1Y();
	static float* GetDistortion2X();
	static float* GetDistortion2Y();
	static float* GetDistortion3X();
	static float* GetDistortion3Y();
	static float* GetDistortionScale();
	static float* GetDistortionBias();

	//set funcs (static)
	static float SetPosX(float value);
	static float SetPosY(float value);
	static float SetPosZ(float value);
	static float SetScaleX(float value);
	static float SetScaleY(float value);
	static float SetScaleZ(float value);
	static float SetSSpeedX(float value);
	static float SetSSpeedY(float value);
	static float SetSSpeedZ(float value);
	static float SetScalesX(float value);
	static float SetScalesY(float value);
	static float SetScalesZ(float value);
	static float SetDistortion1X(float value);
	static float SetDistortion1Y(float value);
	static float SetDistortion2X(float value);
	static float SetDistortion2Y(float value);
	static float SetDistortion3X(float value);
	static float SetDistortion3Y(float value);
	static float SetDistortionScale(float value);
	static float SetDistortionBias(float value);

private:
	wrl::ComPtr<ID3D11Device> mDevice;
	wrl::ComPtr<ID3D11DeviceContext> mContext;
	std::unique_ptr<InputLayout> mLayout;
	std::unique_ptr<VertexBuffer<vertex>> mVertexBuffer;
	std::unique_ptr<CBuffer<matricesBuffer>> mMatricesBuffer;
	std::unique_ptr<CBuffer<noiseBuffer>> mNoiseBuffer;
	std::unique_ptr<CBuffer<distortionBuffer>> mDistortionBuffer;
	std::vector<vertex> mVertices;
	std::unique_ptr<Texture> mTexture;
	std::unique_ptr<Texture> mNoise;
	std::unique_ptr<Texture> mAlpha;
	wrl::ComPtr<ID3D11BlendState> mBlendOn;
	wrl::ComPtr<ID3D11BlendState> mBlendOff;
	VertexShader mVS;
	PixelShader mPS;
};

