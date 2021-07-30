#include "DepthStencil.h"
#include "Converter.h"

DepthStencil::DepthStencil(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, int width, int height,
	UINT msaaQuality, bool msaaEnabled, Usage usage)
	:
	width(width),
	height(height)
{

	//create depth stencil texture
	wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = MapUsageTypeless(usage);
	if (msaaEnabled)
		descDepth.SampleDesc.Count = msaaQuality;
	else
		descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0u;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	HRESULT hr = pDevice->CreateTexture2D(&descDepth,
		nullptr, &pDepthStencil);
	if (FAILED(hr)) { Error::Log(hr, "Failed to create texture 2d in depth stencil.cpp"); }

	//create target view of depth stencil texture
	D3D11_DEPTH_STENCIL_VIEW_DESC descView = {};
	descView.Format = MapUsageTyped(usage);
	descView.Flags = 0;
	descView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	descView.Texture2D.MipSlice = 0;

	//create target view of depth stencil texture
	hr = pDevice->CreateDepthStencilView(pDepthStencil.Get(), &descView, &pDepthStencilView);
	if (FAILED(hr)) { Error::Log(hr, "Failed to create depth stencil view in depth stencil.cpp"); }

	//create shader resource view to take depth texture
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = MapUsageColored(usage);
	srvDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DMS;
	srvDesc.Texture2D.MipLevels = descDepth.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	hr = pDevice->CreateShaderResourceView(pDepthStencil.Get(), &srvDesc, pSRV.GetAddressOf());
	if (FAILED(hr)) { Error::Log(hr, "Failed to create shader resource view in depth stencil.cpp"); }
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

void DepthStencil::BindTexture(ID3D11DeviceContext* pContext, UINT slot) const noexcept
{
	pContext->PSSetShaderResources(slot, 1, pSRV.GetAddressOf());
}

ID3D11ShaderResourceView* DepthStencil::GetTexture() const
{
	return pSRV.Get();
}

DXGI_FORMAT DepthStencil::MapUsageTypeless(DepthStencil::Usage usage)
{
	switch (usage)
	{
	case DepthStencil::Usage::DepthStencil:
		return DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS;
	case DepthStencil::Usage::ShadowDepth:
		return DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
	}
	Error::Log("Depth stencil usage format is null or wrong.");
}

DXGI_FORMAT DepthStencil::MapUsageTyped(DepthStencil::Usage usage)
{
	switch (usage)
	{
	case DepthStencil::Usage::DepthStencil:
		return DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
	case DepthStencil::Usage::ShadowDepth:
		return DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
	}
	Error::Log("Depth stencil usage format is null or wrong.");
}

DXGI_FORMAT DepthStencil::MapUsageColored(DepthStencil::Usage usage)
{
	switch (usage)
	{
	case DepthStencil::Usage::DepthStencil:
		return DXGI_FORMAT::DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	case DepthStencil::Usage::ShadowDepth:
		return DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
	}
	Error::Log("Depth stencil usage format is null or wrong.");
}