#include "Fog.h"

static XMFLOAT4 fogColor = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
static float fogStart = 1.0f;
static float fogEnd = 1.0f;

void Fog::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	this->pDevice = pDevice;
	this->pContext = pContext;
	pVS.Init(L"VSFog.cso", pDevice);
	pPS.Init(L"PSFog.cso", pDevice);
	const std::vector<D3D11_INPUT_ELEMENT_DESC> fog_ied =
	{
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Texcoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	pLayout = std::make_unique<InputLayout>(pDevice, fog_ied, &pVS);
	pFogCBuffer = std::make_unique<CBuffer<FogBuffer>>();
	pFogFactorCBuffer = std::make_unique<CBuffer<FogFactorBuffer>>();
	pFogCBuffer->Init(pDevice, pContext);
	pFogFactorCBuffer->Init(pDevice, pContext);
}

void Fog::Draw()
{
	pFogCBuffer->data.fogColor = fogColor;
	pFogCBuffer->MapData();
	pFogCBuffer->PSBind(pContext.Get(), 0u, 1u);
	pFogFactorCBuffer->data.fogStart = fogStart;
	pFogFactorCBuffer->data.fogEnd = fogEnd;
	pFogFactorCBuffer->MapData();
	pFogFactorCBuffer->VSBind(pContext.Get(), 1u, 1u);
	pLayout->Bind(pContext.Get());
	pVS.Bind(pContext.Get());
	pPS.Bind(pContext.Get());
}

XMFLOAT4* Fog::GetFogColor()
{
	return &fogColor;
}

float* Fog::GetFogStart()
{
	return &fogStart;
}

float* Fog::GetFogEnd()
{
	return &fogEnd;
}
