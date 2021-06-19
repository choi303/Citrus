#include "RenderTexture.h"

bool RenderTexture::Initialize(ID3D11Device* pDevice, int textureWidth, int textureHeight)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT result;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;


	// Initialize the render target texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Setup the render target texture description.
	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// Create the render target texture.
	result = pDevice->CreateTexture2D(&textureDesc, NULL, m_renderTargetTexutre.GetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the render target view.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	result = pDevice->CreateRenderTargetView(m_renderTargetTexutre.Get(), &renderTargetViewDesc, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	result = pDevice->CreateShaderResourceView(m_renderTargetTexutre.Get(), &shaderResourceViewDesc, &m_shaderResourceView);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void RenderTexture::SetRenderTarget(ID3D11DeviceContext* pContext, ID3D11DepthStencilView* pDSV)
{
	pContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), pDSV);
	return;
}

void RenderTexture::ClearRenderTarget(ID3D11DeviceContext* pContext, ID3D11DepthStencilView* pDSV, float r, float g, float b, float a)
{
	float color[4];


	// Setup the color to clear the buffer to.
	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = a;

	// Clear the back buffer.
	pContext->ClearRenderTargetView(m_renderTargetView.Get(), color);

	// Clear the depth buffer.
	pContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

ID3D11ShaderResourceView* RenderTexture::GetShaderResourceView()
{
    return m_shaderResourceView.Get();
}

ID3D11ShaderResourceView** RenderTexture::GetShaderResourceViewAddress()
{
	return m_shaderResourceView.GetAddressOf();
}