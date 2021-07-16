#pragma once
#include <d3d11.h>
#include <wrl.h>
#include "Error.h"
#include <DirectXTex.h>

namespace wrl = Microsoft::WRL;
class DepthStencil
{
public:
	DepthStencil(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pContext,
		int width, int height, UINT msaaQuality, bool msaaEnabled);
	void BindAsDepthStencil(ID3D11DeviceContext* 
		pContext) const noexcept;
	void Clear(ID3D11DeviceContext* pContext) const noexcept;
	void BindSwapBuffer(ID3D11DeviceContext* pContext,
		ID3D11RenderTargetView* pRtv) noexcept;
public:
	wrl::ComPtr<ID3D11DepthStencilView> pDepthStencilView;
};