#pragma once
#include <wrl.h>
#include <d3d11.h>
#include "Error.h"
#include "Shaders.h"
#include <vector>

namespace wrl = Microsoft::WRL;
class InputLayout
{
public:
	InputLayout(ID3D11Device* pDevice,
		const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout,
		VertexShader* vs)
	{
		HRESULT hr;
		//create input layout
		hr = pDevice->CreateInputLayout(layout.data(), (UINT)layout.size(),
			vs->GetBytecode()->GetBufferPointer(),
			vs->GetBytecode()->GetBufferSize(),
			&pil);
		if (FAILED(hr)) { Error::Log(hr, "Failed to create input layout."); }
	}
	~InputLayout() = default;

	void Bind(ID3D11DeviceContext* pContext)
	{
		pContext->IASetInputLayout(pil.Get());
	}
private:
	wrl::ComPtr<ID3D11InputLayout> pil;
};