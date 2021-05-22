#pragma once
#include <d3d11.h>
#include <wrl.h>
#include "Error.h"
#include <DirectXTex.h>

namespace wrl = Microsoft::WRL;
class Texture
{
public:
	Texture() = default;
	Texture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const std::string& filepath);
	ID3D11ShaderResourceView* GetShaderResourceView();
	ID3D11ShaderResourceView** GetShaderResourceViewAddress();
	void Bind(ID3D11DeviceContext* pContext);
	~Texture() = default;
public:
	wrl::ComPtr<ID3D11ShaderResourceView> m_view;
	wrl::ComPtr<ID3D11Resource> m_resource;
};