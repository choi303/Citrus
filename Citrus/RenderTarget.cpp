#include "RenderTarget.h"
#include "DepthStencil.h"

RenderTarget::RenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, int width, int height, UINT msaaQuality, bool msaaEnabled,
	Usage usage)
{
	if (usage == RenderTarget::Usage::Noise)
	{
		float noiseTextureFloats[192];
		for (int i = 0; i < 64; i++)
		{
			int index = i * 3;
			noiseTextureFloats[index] = RandF(-1.0f, 1.0f);
			noiseTextureFloats[index + 1] = RandF(-1.0f, 1.0f);
			noiseTextureFloats[index + 2] = 0.0f;
		}
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = noiseTextureFloats;
		data.SysMemPitch = 8 * 12;
		data.SysMemSlicePitch = 0;
		//create texture resource
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = 8;
		textureDesc.Height = 8;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = MapUsageTyped(usage);
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE
			|
			D3D11_BIND_RENDER_TARGET;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;
		wrl::ComPtr<ID3D11Texture2D> pTexture;
		HRESULT hr = pDevice->CreateTexture2D(&textureDesc,
			&data, &pTexture);
		if (FAILED(hr)) { Error::Log(hr, "Failed to create texture 2d in render target.cpp"); }

		//create the resource view on the texture
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		hr = pDevice->CreateShaderResourceView(
			pTexture.Get(), &srvDesc, &pTextureView);
		if (FAILED(hr)) { Error::Log(hr, "Failed to create shader resource view in render target.cpp"); }

		// create the target view on the texture
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = textureDesc.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
		hr = pDevice->CreateRenderTargetView(
			pTexture.Get(), &rtvDesc, &pTargetView);
		if (FAILED(hr)) { Error::Log(hr, "Failed to create render target view in render target.cpp"); }
	}
	else
	{
		//create texture resource
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = MapUsageTyped(usage);
		if (msaaEnabled)
			textureDesc.SampleDesc.Count = msaaQuality;
		else
			textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE 
			|
			D3D11_BIND_RENDER_TARGET;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;
		wrl::ComPtr<ID3D11Texture2D> pTexture;
		HRESULT hr = pDevice->CreateTexture2D(&textureDesc,
					nullptr, &pTexture);
		if (FAILED(hr)) { Error::Log(hr, "Failed to create texture 2d in render target.cpp"); }

		//create the resource view on the texture
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		hr = pDevice->CreateShaderResourceView(
			pTexture.Get(), &srvDesc, &pTextureView);
		if (FAILED(hr)) { Error::Log(hr, "Failed to create shader resource view in render target.cpp"); }

		// create the target view on the texture
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = textureDesc.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
		hr = pDevice->CreateRenderTargetView(
			pTexture.Get(), &rtvDesc, &pTargetView);
		if (FAILED(hr)) { Error::Log(hr, "Failed to create render target view in render target.cpp"); }
	}
}

void RenderTarget::BindAsTexture(ID3D11DeviceContext* pContext, UINT slot) const noexcept
{
	pContext->PSSetShaderResources(slot, 1u,
		pTextureView.GetAddressOf());
}

void RenderTarget::BindAsTarget(ID3D11DeviceContext* pContext) const noexcept
{
	pContext->OMSetRenderTargets(1,
		pTargetView.GetAddressOf(), nullptr);
}

void RenderTarget::BindAsTarget(ID3D11DeviceContext* pContext, ID3D11DepthStencilView* view) const noexcept
{
	pContext->OMSetRenderTargets(1,
		pTargetView.GetAddressOf(), view);
}

void RenderTarget::Clear(ID3D11DeviceContext* pContext, float col[]) const noexcept
{
	pContext->ClearRenderTargetView(pTargetView.Get(),
		col);
}

DXGI_FORMAT RenderTarget::MapUsageTyped(RenderTarget::Usage usage)
{
	switch (usage)
	{
	case RenderTarget::Usage::Default:
		return DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
	case RenderTarget::Usage::AO:
		return DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT;
	case RenderTarget::Usage::Noise:
		return DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT;
	case RenderTarget::Usage::HDR:
		return DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
	default:
		return DXGI_FORMAT_B8G8R8A8_UNORM;
	}
	Error::Log("Depth stencil usage format is null or wrong.");
}
