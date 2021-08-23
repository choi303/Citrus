#include "Texture.h"
#include "Converter.h"

Texture::Texture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const std::string& filepath, unsigned int slot)
	:
	slot(slot)
{
	//decide texture format
	std::string extension = filepath.substr(filepath.find_last_of(".") + 1);
	if (extension == "dds" || extension == "DDS")
	{
		HRESULT hr = E_FAIL;
		DirectX::ScratchImage pScratch;
		DirectX::TexMetadata pData;
		hr = DirectX::LoadFromDDSFile(Converter::ToWide(filepath).c_str(), DirectX::DDS_FLAGS_NONE, &pData, pScratch);
		if (FAILED(hr)) { Error::Log(hr, "Failed to create dds texture from file\ncheck out description"); }

		const DirectX::Image* pImage = pScratch.GetImage(0, 0, 0);
		assert(pImage);

		CD3D11_TEXTURE2D_DESC textureDesc(pImage->format, static_cast<UINT>(pImage->width), static_cast<UINT>(pImage->height));
		textureDesc.MipLevels = 1;
		ID3D11Texture2D* p2DTexture = nullptr;
		D3D11_SUBRESOURCE_DATA subResourceData = {};
		subResourceData.pSysMem = pImage->pixels;
		subResourceData.SysMemPitch = static_cast<UINT>(pImage->rowPitch);
		hr = pDevice->CreateTexture2D(&textureDesc, &subResourceData, &p2DTexture);
		if (FAILED(hr)) { Error::Log(hr, "Failed to create texture2d"); }

		m_resource = static_cast<ID3D11Texture2D*>(p2DTexture);
		CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE2D, textureDesc.Format);
		hr = pDevice->CreateShaderResourceView(m_resource.Get(), &srvDesc, &m_view);
		if (FAILED(hr)) { Error::Log(hr, "Failed to create shader resource view"); }
	}
	else if (extension == "tga" || extension == "TGA")
	{
		HRESULT hr = E_FAIL;
		DirectX::ScratchImage pScratch;
		DirectX::TexMetadata pData;
		hr = DirectX::LoadFromTGAFile(Converter::ToWide(filepath).c_str(), &pData, pScratch);
		if (FAILED(hr)) { Error::Log(hr, "Failed to load tga texture from file"); }

		const DirectX::Image* pImage = pScratch.GetImage(0, 0, 0);
		assert(pImage);

		CD3D11_TEXTURE2D_DESC textureDesc(pImage->format, static_cast<UINT>(pImage->width), static_cast<UINT>(pImage->height));
		textureDesc.MipLevels = 1;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		ID3D11Texture2D* p2DTexture = nullptr;
		D3D11_SUBRESOURCE_DATA subResourceData{};
		subResourceData.pSysMem = pImage->pixels;
		subResourceData.SysMemPitch = static_cast<UINT>(pImage->rowPitch);
		hr = pDevice->CreateTexture2D(&textureDesc, &subResourceData, &p2DTexture);
		if (FAILED(hr)) { Error::Log(hr, "Failed to create texture2d"); }

		m_resource = static_cast<ID3D11Texture2D*>(p2DTexture);
		CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE2D, textureDesc.Format);
		hr = pDevice->CreateShaderResourceView(m_resource.Get(), &srvDesc, &m_view);
		if (FAILED(hr)) { Error::Log(hr, "Failed to create shader resource view"); }
	}
	else if (extension == "hdr" || extension == "HDR")
	{
		HRESULT hr = E_FAIL;
		DirectX::ScratchImage pScratch;
		DirectX::TexMetadata pData;
		hr = DirectX::LoadFromHDRFile(Converter::ToWide(filepath).c_str(), &pData, pScratch);
		if (FAILED(hr)) { Error::Log(hr, "Failed to load hdr texture from file"); }

		const DirectX::Image* pImage = pScratch.GetImage(0, 0, 0);
		assert(pImage);

		CD3D11_TEXTURE2D_DESC textureDesc(pImage->format, static_cast<UINT>(pImage->width), static_cast<UINT>(pImage->height));
		textureDesc.MipLevels = 1;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		ID3D11Texture2D* p2DTexture = nullptr;
		D3D11_SUBRESOURCE_DATA subResourceData{};
		subResourceData.pSysMem = pImage->pixels;
		subResourceData.SysMemPitch = static_cast<UINT>(pImage->rowPitch);
		hr = pDevice->CreateTexture2D(&textureDesc, &subResourceData, &p2DTexture);
		if (FAILED(hr)) { Error::Log(hr, "Failed to create texture2d"); }

		m_resource = static_cast<ID3D11Texture2D*>(p2DTexture);
		CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE2D, textureDesc.Format);
		hr = pDevice->CreateShaderResourceView(m_resource.Get(), &srvDesc, &m_view);
		if (FAILED(hr)) { Error::Log(hr, "Failed to create shader resource view"); }
	}
	else
	{
		HRESULT hr = E_FAIL;
		DirectX::ScratchImage pScratch;
		DirectX::TexMetadata pData;
		hr = DirectX::LoadFromWICFile(Converter::ToWide(filepath).c_str(), DirectX::WIC_FLAGS_IGNORE_SRGB, &pData, pScratch);
		if (FAILED(hr)) { Error::Log(hr, "Failed to load wic texture from file"); }

		const DirectX::Image* pImage = pScratch.GetImage(0, 0, 0);
		assert(pImage);

		CD3D11_TEXTURE2D_DESC textureDesc(pImage->format, static_cast<UINT>(pImage->width), static_cast<UINT>(pImage->height));
		textureDesc.MipLevels = 0;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		ID3D11Texture2D* p2DTexture = nullptr;
		hr = pDevice->CreateTexture2D(&textureDesc, nullptr, &p2DTexture);
		if (FAILED(hr)) { Error::Log(hr, "Failed to create texture2d"); }

		m_resource = static_cast<ID3D11Texture2D*>(p2DTexture);

		pContext->UpdateSubresource(m_resource.Get(), 0u, nullptr, reinterpret_cast<UINT*>(pImage->pixels), static_cast<UINT>(pImage->rowPitch), 0);

		CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE2D, textureDesc.Format);
		hr = pDevice->CreateShaderResourceView(m_resource.Get(), &srvDesc, &m_view);
		if (FAILED(hr)) { Error::Log(hr, "Failed to create shader resource view"); }
	}

	pContext->GenerateMips(m_view.Get());
}

ID3D11ShaderResourceView* Texture::GetShaderResourceView() const
{
	return m_view.Get();
}

ID3D11ShaderResourceView** Texture::GetShaderResourceViewAddress()
{
	return m_view.GetAddressOf();
}

void Texture::Bind(ID3D11DeviceContext* pContext)
{
	pContext->PSSetShaderResources(slot, 1u, m_view.GetAddressOf());
}