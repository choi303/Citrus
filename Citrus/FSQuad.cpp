#include "FSQuad.h"

static bool blurEnabled;
static float blurIntensity = 9.0f;
static bool fxaaEnabled;

FSQuad::FSQuad(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, int width, int height)
{
	this->width = width;
	this->height = height;
	//init shaders
	pVSBlur.Init(L"VSFullScreenEffect.cso", pDevice);
	pPSBlur.Init(L"PSFullScreenEffect.cso", pDevice);
	pVSFxaa.Init(L"VSFXAA.cso", pDevice);
	pPSFxaa.Init(L"PSFXAA.cso", pDevice);
	//init and create input layout
	const std::vector<D3D11_INPUT_ELEMENT_DESC> fs_ied
		=
	{
		{"Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0,
		D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	pLayout = std::make_unique<InputLayout>(
		pDevice, fs_ied, &pVSBlur
		);

	//vertices
	vertices.push_back(vertex(XMFLOAT2(-1, 1)));
	vertices.push_back(vertex(XMFLOAT2(1, 1)));
	vertices.push_back(vertex(XMFLOAT2(-1, -1)));
	vertices.push_back(vertex(XMFLOAT2(1, -1)));

	//create vertex buffer
	pVertexBuffer = std::make_unique<VertexBuffer<vertex>>();
	pVertexBuffer->Init(pDevice, vertices.data(), vertices.size());

	//create index buffer
	std::vector<signed int> indices = { 0,1,2,1,3,2 };
	pIndexBuffer = std::make_unique<IndexBuffer>();
	pIndexBuffer->Init(pDevice, indices, indices.size());

	//constant buffer initialize
	paramCBuffer = std::make_unique<CBuffer<param>>();
	paramCBuffer->Init(pDevice, pContext);
	fxaaCBuffer = std::make_unique<CBuffer<fxaa>>();
	fxaaCBuffer->Init(pDevice, pContext);
}

void FSQuad::draw(ID3D11DeviceContext* pContext)
{
	//vertex and index buffer bind
	pVertexBuffer->Bind(pContext);
	pIndexBuffer->Bind(pContext);
	//input layout bind
	pLayout->Bind(pContext);
	//shaders bind
	pVSBlur.Bind(pContext);
	pPSBlur.Bind(pContext);
	//bind fxaa shaders
	if (fxaaEnabled)
	{
		pContext->IASetInputLayout(0);
		pVSFxaa.Bind(pContext);
		pPSFxaa.Bind(pContext);
	}
	//param cbuffer values bind
	paramCBuffer->data.blurEnabled = blurEnabled;
	paramCBuffer->data.blurIntensity = blurIntensity;
	paramCBuffer->MapData();
	paramCBuffer->PSBind(pContext, 0, 1);
	//fxaa cbuffer values bind
	fxaaCBuffer->data.rcpFrame = XMFLOAT4(1.0f / width, 1.0f / height, 0.0f, 0.0f);
	fxaaCBuffer->MapData();
	fxaaCBuffer->PSBind(pContext, 1, 1);
	//draw quad
	pContext->DrawIndexed(pIndexBuffer->BufferSize(),
		0, 0);
}

bool* FSQuad::GetBlurEnabled()
{
	return &blurEnabled;
}

float* FSQuad::GetBlurIntensity()
{
	return &blurIntensity;
}

bool FSQuad::SetBlurEnabled(bool value)
{
	return blurEnabled = value;
}

float FSQuad::SetBlurIntensity(float value)
{
	return blurIntensity = value;
}

bool* FSQuad::GetFxaaEnabled()
{
	return &fxaaEnabled;
}

bool FSQuad::SetFxaaEnabled(bool value)
{
	return fxaaEnabled = value;
}