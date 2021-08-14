#pragma once
#include "GfxBase.h"

class Particle
{
private:
	struct ParticleType
	{
		float positionX, positionY, positionZ;
		float red, green, blue;
		float velocity;
		bool active;
	};

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT4 color;
	};

	struct MatrixBuffer
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX proj;
	};
public:
	Particle();
	Particle(const Particle&)=default;
	~Particle()=default;

	bool Initialize(ID3D11Device* pDevice, std::string rPath, ID3D11DeviceContext* pContext);
	void Shutdown();
	bool Frame(float frameTime);
	void Render(Camera3D cam);

	ID3D11ShaderResourceView* GetTexture();
	int GetIndexCount();

private:
	bool LoadTexture();
	bool InitializeParticleSystem();
	void ShutdownParticleSystem();
	bool InitializeBuffers();
	void ShutdownBuffers();
	void EmitParticles(float frameTime);
	void UpdateParticles(float frameTime);
	void KillParticles();
	bool UpdateBuffers();
	void RenderBuffers(Camera3D cam);
	
public:
	//get varibles (static)
	static float* GetDeviationX();
	static float* GetDeviationY();
	static float* GetDeviationZ();
	static float* GetParticleVelocity();
	static float* GetParticleVelocityVariation();
	static float* GetParticleSize();
	static float* GetParticlesPerSecond();
	static int* GetParticleMax();
	static int* GetCurrentParticleCount();
	static float* GetAccumulatedTime();
	static float* GetLifeTime();
	static bool* GetIsKilled();
	static bool* GetIsLifeTime();
	static float* GetPosX();
	static float* GetPosY();
	static float* GetPosZ();

	//set varibles (static)
	static float SetDeviationX(float value);
	static float SetDeviationY(float value);
	static float SetDeviationZ(float value);
	static float SetParticleVelocity(float value);
	static float SetParticleVelocityVariation(float value);
	static float SetParticleSize(float value);
	static float SetParticlesPerSecond(float value);
	static int SetParticleMax(int value);
	static int SetCurrentParticleCount(int value);
	static float SetAccumulatedTime(float value);
	static float SetLifeTime(float value);
	static bool SetIsKilled(bool value);
	static bool SetIsLifeTime(bool value);
	static float SetPosX(float value);
	static float SetPosY(float value);
	static float SetPosZ(float value);

private:
	std::unique_ptr<Texture> mTexture;
	ParticleType* mParticleList;
	int mVertexCount, mIndexCount;
	VertexType* mVertices;
	ID3D11Buffer* mVertexBuffer, * mIndexBuffer;
	VertexShader mVS;
	PixelShader mPS;
	std::unique_ptr<InputLayout> mLayout;
	std::unique_ptr<CBuffer<MatrixBuffer>> fxCBuffer;
	wrl::ComPtr<ID3D11BlendState> mBlendOn;
	wrl::ComPtr<ID3D11BlendState> mBlendOff;
	wrl::ComPtr<ID3D11Device> mDevice;
	wrl::ComPtr<ID3D11DeviceContext> mContext;
	HRESULT hr;
	std::string mPath;
};

