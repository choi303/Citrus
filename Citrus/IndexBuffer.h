#pragma once
#include <d3d11.h>
#include "Error.h"
#include <wrl.h>
#include <vector>

namespace wrl = Microsoft::WRL;

class IndexBuffer
{
public:
	IndexBuffer() { }
	~IndexBuffer() { }
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> ib;
	UINT buffersize = 0;
public:
	//get buffer func
	ID3D11Buffer* Get() const
	{
		return ib.Get();
	}
	//get buffer adress or as ptr 
	ID3D11Buffer* const* GetAddressOf() const
	{
		return ib.GetAddressOf();
	}
	//returns buffer size
	UINT BufferSize() const
	{
		return buffersize;
	}
	//binds index buffer to input assembler 
	void Bind(ID3D11DeviceContext* pContext) const noexcept
	{
		pContext->IASetIndexBuffer(ib.Get(), DXGI_FORMAT_R32_UINT, 0u);
	}
	
	HRESULT Init(ID3D11Device* device, std::vector<signed int>& data, UINT indices)
	{
		if (ib.Get() != nullptr)
			ib.Reset();
		
		buffersize = indices;
		//create index buffer
		D3D11_BUFFER_DESC ibd;
		ZeroMemory(&ibd, sizeof(ibd));
		ibd.Usage = D3D11_USAGE_DEFAULT;
		ibd.ByteWidth = indices * sizeof(signed int);
		ibd.StructureByteStride = sizeof(signed int);
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		ibd.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA ibs;
		ibs.pSysMem = data.data();
		HRESULT hr = device->CreateBuffer(&ibd, &ibs, &ib);
		if(FAILED(hr)) {Error::Log(hr, "Failed to create index buffer"); }
		return hr;
	}
};