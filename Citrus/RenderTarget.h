#pragma once
#include "GfxBase.h"

class DepthStencil;

namespace wrl = Microsoft::WRL;
class RenderTarget
{
public:
	enum class Usage
	{
		Default,
		AO,
		Noise,
		HDR,
	};
public:
	RenderTarget(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pContext, int width,
		int height, UINT msaaQuality, bool msaaEnabled,
		Usage usage);
	void BindAsTexture(ID3D11DeviceContext* pContext, UINT slot) const noexcept;
	void BindAsTarget(ID3D11DeviceContext* pContext) const noexcept;
	void BindAsTarget(ID3D11DeviceContext* pContext,
		ID3D11DepthStencilView* view) const noexcept;
	void Clear(ID3D11DeviceContext* pContext, float col[]) const noexcept;
	DXGI_FORMAT MapUsageTyped(RenderTarget::Usage usage);
private:
	wrl::ComPtr<ID3D11ShaderResourceView> pTextureView;
	wrl::ComPtr<ID3D11RenderTargetView> pTargetView;
private:
	static float RandF()
	{
		return (float)(rand()) / (float)RAND_MAX;
	}

	// Returns random float in [a, b).
	static float RandF(float a, float b)
	{
		return a + RandF() * (b - a);
	}
};

