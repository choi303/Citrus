#pragma once
#include "GfxBase.h"

class PbrSphere
{
private:
	struct PbrBuffer
	{
		XMFLOAT3 albedo;
		float metallic;
		float roughness;
		float ao;
		float pad[2];
	};
public:
	PbrSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	PbrSphere(PbrSphere& rhs) = default;
	~PbrSphere() = default;

	void Draw(Camera3D cam);
private:
	std::unique_ptr<InputLayout> pLayout;
	wrl::ComPtr<ID3D11Device> pDevice;
	wrl::ComPtr<ID3D11DeviceContext> pContext;
	Model pModel;
	std::unique_ptr<CBuffer<PbrBuffer>> pbrCBuffer;
	VertexShader pVS;
	PixelShader pPS;
	std::unique_ptr<Texture> pAlbedo;
	std::unique_ptr<Texture> pNormal;
	std::unique_ptr<Texture> pMetallic;
	std::unique_ptr<Texture> pRoughness;
	std::unique_ptr<Texture> pAO;
};

