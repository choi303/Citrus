#include "CubeTexture.h"
#include "Converter.h"

CubeTexture::CubeTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const std::string& filepath, unsigned int slot)
	:
	slot(slot)
{
	//decide texture format
	std::string extension = filepath.substr(filepath.find_last_of(".") + 1);
	if (extension == "dds")
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
	else if (extension == "tga")
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
	else
	{

		HRESULT hr = E_FAIL;
		DirectX::ScratchImage pScratch;
		DirectX::TexMetadata pData;

		hr = DirectX::LoadFromWICFile(Converter::ToWide(filepath).c_str(), DirectX::WIC_FLAGS_IGNORE_SRGB, &pData, pScratch);
		if (FAILED(hr)) { Error::Log(hr, "Failed to load wic texture from file"); }

		const DirectX::Image* pImage = pScratch.GetImage(0, 0, 0);
		assert(pImage);

		CD3D11_TEXTURE2D_DESC textureDesc(DXGI_FORMAT_B8G8R8A8_UNORM, static_cast<UINT>(pImage->width), static_cast<UINT>(pImage->height));
		textureDesc.MipLevels = 0;
		textureDesc.ArraySize = 6;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE | D3D11_RESOURCE_MISC_GENERATE_MIPS;
		ID3D11Texture2D* p2DTexture = nullptr;
		hr = pDevice->CreateTexture2D(&textureDesc, nullptr, &p2DTexture);
		if (FAILED(hr)) { Error::Log(hr, "Failed to create texture2d"); }

		m_resource = static_cast<ID3D11Texture2D*>(p2DTexture);
		//create sky box render target view
		D3D11_RENDER_TARGET_VIEW_DESC skybox_rtvDesc;
		skybox_rtvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		skybox_rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		skybox_rtvDesc.Texture2DArray.MipSlice = 0;
		skybox_rtvDesc.Texture2DArray.ArraySize = 1;
		for (int i = 0; i < 6; ++i)
		{
			skybox_rtvDesc.Texture2DArray.FirstArraySlice = i;
			hr = pDevice->CreateRenderTargetView(m_resource.Get(), &skybox_rtvDesc, &pSkyboxRTV[i]);
		}

		D3D11_TEXTURE2D_DESC depthTexDesc;
		depthTexDesc.Width = pImage->width;
		depthTexDesc.Height = pImage->height;
		depthTexDesc.MipLevels = 1;
		depthTexDesc.ArraySize = 1;
		depthTexDesc.SampleDesc.Count = 1;
		depthTexDesc.SampleDesc.Quality = 0;
		depthTexDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
		depthTexDesc.CPUAccessFlags = 0;
		depthTexDesc.MiscFlags = 0;
		
		
		pContext->UpdateSubresource(m_resource.Get(), 0u, nullptr, (UINT*)pImage->pixels, (UINT)pImage->rowPitch, 0);

		CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURECUBE, textureDesc.Format);
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = -1;
		hr = pDevice->CreateShaderResourceView(m_resource.Get(), &srvDesc, &m_view);
		if (FAILED(hr)) { Error::Log(hr, "Failed to create shader resource view"); }
	}

	pContext->GenerateMips(m_view.Get());
}

ID3D11ShaderResourceView* CubeTexture::GetShaderResourceView() const
{
	return m_view.Get();
}

ID3D11ShaderResourceView** CubeTexture::GetShaderResourceViewAddress()
{
	return m_view.GetAddressOf();
}

void CubeTexture::Bind(ID3D11DeviceContext* pContext)
{
	pContext->OMSetRenderTargets(1u, pSkyboxRTV->GetAddressOf(), nullptr);
	pContext->PSSetShaderResources(slot, 1u, m_view.GetAddressOf());
}