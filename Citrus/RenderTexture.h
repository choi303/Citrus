#pragma once
#include "GfxBase.h"

namespace wrl = Microsoft::WRL;
class RenderTexture
{
public:
	RenderTexture()=default;
	RenderTexture(const RenderTexture&)=default;
	~RenderTexture()=default;

	bool Initialize(ID3D11Device* pDevice, int textureWidth, int textureHeight);
	void SetRenderTarget(ID3D11DeviceContext* pContext, ID3D11DepthStencilView* pDSV);
	void ClearRenderTarget(ID3D11DeviceContext* pContext, ID3D11DepthStencilView* pDSV, float r, float g, float b, float a);
	ID3D11ShaderResourceView* GetShaderResourceView();
	ID3D11ShaderResourceView** GetShaderResourceViewAddress();
private:
	wrl::ComPtr<ID3D11Texture2D> m_renderTargetTexutre;
	wrl::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
	wrl::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
};