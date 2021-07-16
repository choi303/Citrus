#include "DepthStencil.h"
#include "Converter.h"

DepthStencil::DepthStencil(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, int width, int height,
	UINT msaaQuality, bool msaaEnabled)
{
	//create depth stencil texture
	wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	if (msaaEnabled)
		descDepth.SampleDesc.Count = msaaQuality;
	else
		descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0u;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	HRESULT hr = pDevice->CreateTexture2D(&descDepth,
		nullptr, &pDepthStencil);
	if (FAILED(hr)) { Error::Log(hr, "Failed to create texture 2d in depth stencil.cpp"); }

	//create target view of depth stencil texture
	hr = pDevice->CreateDepthStencilView(pDepthStencil.Get(), nullptr, &pDepthStencilView);
	if (FAILED(hr)) { Error::Log(hr, "Failed to create depth stencil view in depth stencil.cpp"); }
}

void DepthStencil::BindAsDepthStencil(ID3D11DeviceContext* pContext) const noexcept
{
	pContext->OMSetRenderTargets(0, nullptr,
		pDepthStencilView.Get());
}

void DepthStencil::Clear(ID3D11DeviceContext* pContext) const noexcept
{
	pContext->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 
		0u);
}

void DepthStencil::BindSwapBuffer(ID3D11DeviceContext*
	pContext, ID3D11RenderTargetView* pRtv) noexcept
{
	pContext->OMSetRenderTargets(1u, &pRtv,
		this->pDepthStencilView.Get());
}
