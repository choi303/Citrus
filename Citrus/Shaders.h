#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <string>
#include "Error.h"

namespace wrl = Microsoft::WRL;

class VertexShader
{
public:
	VertexShader() = default;
	bool Init(const std::wstring& name, ID3D11Device* pDevice) 
	{
		//create vertex shader
		HRESULT hr = D3DReadFileToBlob(name.c_str(), &pBlob);
		if (FAILED(hr)) { Error::Log(hr, "Failed to read file to blob. (Vertex Shader)"); return false; }
		hr = pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader);
		if (FAILED(hr)) { Error::Log(hr, "Failed to create vertex shader"); return false; }

		return true;
	}

	ID3DBlob* GetBytecode() const noexcept
	{
		return pBlob.Get();
	}

	~VertexShader() = default;

	void Bind(ID3D11DeviceContext* deviceContext)
	{
		deviceContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);
	}

private:
	wrl::ComPtr<ID3D11VertexShader> pVertexShader;
	wrl::ComPtr<ID3DBlob> pBlob;
};

class PixelShader
{
public:
	PixelShader() = default;
	bool Init(const std::wstring& name,ID3D11Device* pDevice)
	{
		HRESULT hr = D3DReadFileToBlob(name.c_str(), &pBlob);
		if (FAILED(hr)) { Error::Log(hr, "Failed to read file to blob. (Pixel Shader)"); return false; }
		hr = pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader);
		if (FAILED(hr)) { Error::Log(hr, "Failed to create pixel shader"); return false; }

		return true;
	}
	~PixelShader() = default;

	void Bind(ID3D11DeviceContext* pContext)
	{
		pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);
	}

private:
	wrl::ComPtr<ID3D11PixelShader> pPixelShader;
	wrl::ComPtr<ID3DBlob> pBlob;
};

class HullShader
{
public:
	HullShader() = default;
	HullShader(HullShader& rhs) = default;
	~HullShader() = default;
	bool Init(const std::wstring& name, ID3D11Device* pDevice)
	{
		HRESULT hr = D3DReadFileToBlob(name.c_str(),
			&pBlob);
		if (FAILED(hr)) { Error::Log(hr, "Failed to read file to blob. (Hull Shader)"); return false; }
		hr = pDevice->CreateHullShader
		(pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(), nullptr,
			&pHullShader);
		if(FAILED(hr))
		{
			Error::Log(hr, "Failed to create Hull Shader");
			return false;
		}

		return true;
	}

	void Bind(ID3D11DeviceContext* pContext)
	{
		pContext->HSSetShader(pHullShader.Get(),
			nullptr, 0);
	}

private:
	wrl::ComPtr<ID3D11HullShader> pHullShader;
	wrl::ComPtr<ID3DBlob> pBlob;
};

class DomainShader
{
public:
	DomainShader() = default;
	DomainShader(DomainShader& rhs) = default;
	~DomainShader() = default;
	bool Init(const std::wstring& name, ID3D11Device* pDevice)
	{
		HRESULT hr = D3DReadFileToBlob(name.c_str(),
			&pBlob);
		if (FAILED(hr)) { Error::Log(hr, "Failed to read file to blob. (Domain Shader)"); return false; }
		hr = pDevice->CreateDomainShader
		(pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(), nullptr,
			&pDomainShader);
		if (FAILED(hr))
		{
			Error::Log(hr, "Failed to create Domain Shader");
			return false;
		}

		return true;
	}

	void Bind(ID3D11DeviceContext* pContext)
	{
		pContext->DSSetShader(pDomainShader.Get(),
			nullptr, 0);
	}

private:
	wrl::ComPtr<ID3D11DomainShader> pDomainShader;
	wrl::ComPtr<ID3DBlob> pBlob;
};