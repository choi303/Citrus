#include "DepthBuffer.h"

void DepthBuffer::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	this->pDevice = pDevice;
	this->pContext = pContext;
	pVS.Init(L"VSDepthBuffer.cso", pDevice);
	pPS.Init(L"PSDepthBuffer.cso", pDevice);
	const std::vector<D3D11_INPUT_ELEMENT_DESC> depth_ied =
	{
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Texcoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	pLayout = std::make_unique<InputLayout>(pDevice, depth_ied, &pVS);
}

void DepthBuffer::Draw()
{
	pLayout->Bind(pContext.Get());
	pVS.Bind(pContext.Get());
	pPS.Bind(pContext.Get());
}
