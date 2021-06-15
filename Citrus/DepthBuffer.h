#pragma once
#include "GfxBase.h"

class DepthBuffer
{
public:
	DepthBuffer() = default;
	DepthBuffer(const DepthBuffer&) {}
	void Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Draw();
	~DepthBuffer()=default;
private:
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	std::unique_ptr<InputLayout> pLayout;
	VertexShader pVS;
	PixelShader pPS;
};