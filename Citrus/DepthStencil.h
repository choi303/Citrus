#pragma once
#include <d3d11.h>
#include <wrl.h>
#include "Error.h"
#include <DirectXTex.h>

namespace wrl = Microsoft::WRL;
class DepthStencil
{
public:
	enum class Usage
	{
		DepthStencil,
		ShadowDepth,
	};
public:
	DepthStencil(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pContext,
		int width, int height, UINT msaaQuality, bool msaaEnabled, Usage usage);
	void BindAsDepthStencil(ID3D11DeviceContext* 
		pContext) const noexcept;
	void Clear(ID3D11DeviceContext* pContext) const noexcept;
	void BindSwapBuffer(ID3D11DeviceContext* pContext,
		ID3D11RenderTargetView* pRtv) noexcept;
	void BindTexture(ID3D11DeviceContext* pContext, UINT slot) const noexcept;
	ID3D11ShaderResourceView* GetTexture() const;
private:
	DXGI_FORMAT MapUsageTypeless(DepthStencil::Usage usage);
	DXGI_FORMAT MapUsageTyped(DepthStencil::Usage usage);
	DXGI_FORMAT MapUsageColored(DepthStencil::Usage usage);
public:
	wrl::ComPtr<ID3D11DepthStencilView> pDepthStencilView;
	wrl::ComPtr<ID3D11ShaderResourceView> pSRV;
	int width;
	int height;
};