#pragma once
#include <d3d11.h>
#include <wrl.h>
#include "Error.h"

namespace wrl = Microsoft::WRL;
template<class T>
class CBuffer
{
public:
	CBuffer() {};
	~CBuffer() {};
private:
	wrl::ComPtr<ID3D11Buffer> cb;
	wrl::ComPtr<ID3D11DeviceContext> pContext;
public:
	//data for array of elements
	T data;
	//gets buffer
	ID3D11Buffer* Get() const noexcept
	{
		return cb.Get();
	}
	//gets buffer as ptr
	ID3D11Buffer* const* GetAddressOf() const noexcept
	{
		return cb.GetAddressOf();
	}
	
	HRESULT Init(ID3D11Device* device, ID3D11DeviceContext* pContext)
	{
		if (cb.Get() != nullptr)
			cb.Reset();

		this->pContext = pContext;
		//Create constant buffer
		D3D11_BUFFER_DESC desc = {};
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.ByteWidth = static_cast<UINT>(sizeof(T) + (16 - (sizeof(T) % 16)));
		desc.StructureByteStride = 0;
		const HRESULT hr = device->CreateBuffer(&desc, nullptr, cb.GetAddressOf());
		return hr;
	}

	bool MapData()
	{
		//Mapping data
		D3D11_MAPPED_SUBRESOURCE mappedData = {};
		HRESULT hr = pContext->Map(cb.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0u, &mappedData);
		if (FAILED(hr)) { Error::Log(hr, "Failed to map data."); return false; }
		CopyMemory(mappedData.pData, &data, sizeof(T));
		pContext->Unmap(cb.Get(), 0u);
		return true;
	}

	void VSBind(ID3D11DeviceContext* pContext, UINT slot, UINT numBuffers)
	{
		pContext->VSSetConstantBuffers(slot, numBuffers, cb.GetAddressOf());
	}

	void PSBind(ID3D11DeviceContext* pContext, UINT slot, UINT numBuffers)
	{
		pContext->PSSetConstantBuffers(slot, numBuffers, cb.GetAddressOf());
	}

	void HSBind(ID3D11DeviceContext* pContext, UINT slot, UINT numBuffers)
	{
		pContext->HSSetConstantBuffers(slot, numBuffers, cb.GetAddressOf());
	}

	void DSBind(ID3D11DeviceContext* pContext, UINT slot, UINT numBuffers)
	{
		pContext->DSSetConstantBuffers(slot, numBuffers, cb.GetAddressOf());
	}
};