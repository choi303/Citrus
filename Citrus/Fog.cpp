#include "Fog.h"

//create variables for imgui window
static XMFLOAT4 fogColor;
static float fogStart = 1.0f;
static float fogEnd = 50.0f;

void Fog::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	//set local device and context to reference device and context
	this->pDevice = pDevice;
	this->pContext = pContext;
	//shader initialize
	pVS.Init(L"VSFog.cso", pDevice);
	pPS.Init(L"PSFog.cso", pDevice);
	//layout init and creation
	const std::vector<D3D11_INPUT_ELEMENT_DESC> fog_ied =
	{
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Texcoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	pLayout = std::make_unique<InputLayout>(pDevice, fog_ied, &pVS);
	//creating constant buffer for fog
	pFogCBuffer = std::make_unique<CBuffer<FogBuffer>>();
	//another constant buffer for fog settings (start, end)
	pFogFactorCBuffer = std::make_unique<CBuffer<FogFactorBuffer>>();
	//initialize the buffers
	pFogCBuffer->Init(pDevice, pContext);
	pFogFactorCBuffer->Init(pDevice, pContext);
}

void Fog::Draw()
{
	//set values and apply the values
	pFogCBuffer->data.fogColor = fogColor;
	pFogCBuffer->MapData();
	//bind fog buffer
	pFogCBuffer->PSBind(pContext.Get(), 0u, 1u);
	//set values and apply the values
	pFogFactorCBuffer->data.fogStart = fogStart;
	pFogFactorCBuffer->data.fogEnd = fogEnd;
	pFogFactorCBuffer->MapData();
	//bind fog factor buffer
	pFogFactorCBuffer->VSBind(pContext.Get(), 1u, 1u);
	//bind the input layout
	pLayout->Bind(pContext.Get());
	//bind shaders
	pVS.Bind(pContext.Get());
	pPS.Bind(pContext.Get());
}

XMFLOAT4* Fog::GetFogColor()
{
	//return fog colors
	return &fogColor;
}

float* Fog::GetFogStart()
{
	//return fog start
	return &fogStart;
}

float* Fog::GetFogEnd()
{
	//return fog end
	return &fogEnd;
}

float Fog::SetFogStart(float value)
{
	//set fog start to reference value
	return fogStart = value;
}

float Fog::SetFogEnd(float value)
{
	//set fog end to reference value
	return fogEnd = value;
}

float Fog::SetColorX(float value)
{
	//set color x to reference value
	return fogColor.x = value;
}

float Fog::SetColorY(float value)
{
	//set color y to reference value
	return fogColor.y = value;
}

float Fog::SetColorZ(float value)
{
	//set color z to reference value
	return fogColor.z = value;
}

float Fog::SetColorW(float value)
{
	//set color w to reference value
	return fogColor.w = value;
}