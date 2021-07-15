#pragma once
#include "GfxBase.h"

class DepthStencil;

namespace wrl = Microsoft::WRL;
class RenderTarget
{
public:
	RenderTarget(ID3D11Device* pDevice, 
		ID3D11DeviceContext* pContext, int width,
		int height);
	void BindAsTexture(ID3D11DeviceContext* pContext, UINT slot) const noexcept;
	void BindAsTarget(ID3D11DeviceContext* pContext) const noexcept;
	void BindAsTarget(ID3D11DeviceContext* pContext,
		ID3D11DepthStencilView* view) const noexcept;
	void Clear(ID3D11DeviceContext* pContext, float col[]) const noexcept;
private:
	wrl::ComPtr<ID3D11ShaderResourceView> pTextureView;
	wrl::ComPtr<ID3D11RenderTargetView> pTargetView;
};

