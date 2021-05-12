#include "Texture.h"

bool Texture::Init(ID3D11Device* pDevice, const WICTexture& wicmap)
{
	//create texture 2d desc
	CD3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = wicmap.GetWidth();	//get with fom wic class
	textureDesc.Height = wicmap.GetHeight(); //get height fom wic class
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0u;
	textureDesc.MiscFlags = 0u;
	D3D11_SUBRESOURCE_DATA subData = {};
	subData.pSysMem = wicmap.GetBufferPtr();	//get buffer from wic texture class
	subData.SysMemPitch = wicmap.GetWidth() * sizeof(Color);
	wrl::ComPtr<ID3D11Texture2D> pTexture;
	HRESULT hr = pDevice->CreateTexture2D(&textureDesc, &subData, pTexture.GetAddressOf());
	if (FAILED(hr)) { Error::Log(hr, "Failed to create texture 2D"); return false; }

	//create srv
	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	hr = pDevice->CreateShaderResourceView(pTexture.Get(), &srvDesc, m_resource.GetAddressOf());
	if (FAILED(hr)) { Error::Log(hr, "Failed to create shader resource view"); return false; }

	return true;
}

void Texture::Bind(ID3D11DeviceContext* pContext)
{
	pContext->PSSetShaderResources(0u, 1u, m_resource.GetAddressOf());
}