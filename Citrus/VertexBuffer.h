#pragma once
#include <d3d11.h>
#include <wrl.h>
#include "Error.h"
#include <vector>
#include <iostream>

namespace wrl = Microsoft::WRL;

template<class T>
class VertexBuffer
{
public:
	VertexBuffer() {};
	~VertexBuffer() {};
private:
	wrl::ComPtr<ID3D11Buffer> vb;
	std::shared_ptr<UINT> stride;
	UINT buffersize = 0;
	UINT offset = 0;
public:
	//get buffer func
	ID3D11Buffer* Get() const
	{
		return vb.Get();
	}
	//get buffer address or as ptr
	ID3D11Buffer* const* GetAddressOf() const noexcept	
	{
		return vb.GetAddressOf();
	}
	//returns buffer size
	UINT BufferSize() const noexcept
	{
		return buffersize;
	}
	//returns stride
	UINT Stride() const noexcept
	{
		return *stride.get();
	}
	//returns stride as ptr
	const UINT * StridePtr() const noexcept
	{
		return stride.get();
	}

	HRESULT Init(ID3D11Device* pDevice, T* data, UINT vertices)
	{
		if (vb.Get() != nullptr)
			vb.Reset();

		buffersize = vertices;
		stride = std::make_shared<UINT>(sizeof(T));
		//create VERTEX buffer
		D3D11_BUFFER_DESC vbd = {};
		vbd.ByteWidth = sizeof(T) * vertices;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.Usage = D3D11_USAGE_DEFAULT;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0u;
		D3D11_SUBRESOURCE_DATA vsd = {};
		vsd.pSysMem = data;
		const HRESULT hr = pDevice->CreateBuffer(&vbd, &vsd, &vb);
		if (FAILED(hr)) { Error::Log(hr, "Failed to create buffer vertex"); return false; }

		return hr;
	}
	//bind vertex buffer to input assembler
	void Bind(ID3D11DeviceContext* pContext) const noexcept
	{
		pContext->IASetVertexBuffers(0u, 1u, vb.GetAddressOf(), StridePtr(), &offset);
	}
};