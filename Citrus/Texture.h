#pragma once
#include <d3d11.h>
#include "WICTexture.h"
#include <wrl.h>
#include "Error.h"

namespace wrl = Microsoft::WRL;
class Texture
{
public:
	Texture()=default;
	bool Init(ID3D11Device* pDevice, const WICTexture& wicmap);
	void Bind(ID3D11DeviceContext* pContext);
	~Texture() = default;
public:
	wrl::ComPtr<ID3D11ShaderResourceView> m_resource;
};