#include "FSQuad.h"

static bool blurEnabled;
static float blurIntensity = 9.0f;

FSQuad::FSQuad(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	//init shaders
	pVS.Init(L"VSFullScreenEffect.cso", pDevice);
	pPS.Init(L"PSFullScreenEffect.cso", pDevice);
	//init and create input layout
	const std::vector<D3D11_INPUT_ELEMENT_DESC> fs_ied
		=
	{
		{"Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0,
		D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	pLayout = std::make_unique<InputLayout>(
		pDevice, fs_ied, &pVS
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
}

void FSQuad::draw(ID3D11DeviceContext* pContext)
{
	pVertexBuffer->Bind(pContext);
	pIndexBuffer->Bind(pContext);
	pVS.Bind(pContext);
	pPS.Bind(pContext);
	pLayout->Bind(pContext);
	paramCBuffer->data.blurEnabled = blurEnabled;
	paramCBuffer->data.blurIntensity = blurIntensity;
	paramCBuffer->MapData();
	paramCBuffer->PSBind(pContext, 0, 1);
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