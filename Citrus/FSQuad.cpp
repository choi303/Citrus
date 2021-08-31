#include "FSQuad.h"

static bool blurEnabled;
static float blurIntensity = 9.0f;
static bool fxaaEnabled;
static BOOL ssaoEnabled;
static float totalStrength = 1.5;
static float base = 0.223;
static float area = 0.0000000000001;
static float fallOff = 0.000000001;
static float radius = 0.007;
static float exposure = 1.690f;
static float gamma = 0.750f;
static float bloomIntensity = 1.0f;
static BOOL toneMappingEnabled;
static BOOL autoExposureEnabled;
static bool bloomRender;
static BOOL bloomEnabled;

FSQuad::FSQuad(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, int width, int height)
{
	this->width = width;
	this->height = height;
	//init shaders
	pVSBlur.Init(L"VSFullScreenEffect.cso", pDevice);
	pPSBlur.Init(L"PSFullScreenEffect.cso", pDevice);
	pVSFxaa.Init(L"VSFXAA.cso", pDevice);
	pPSFxaa.Init(L"PSFXAA.cso", pDevice);
	pVSSsao.Init(L"VS_SSAO.cso", pDevice);
	pPSSsao.Init(L"PS_SSAO.cso", pDevice);
	pVSBloom.Init(L"VSBloom.cso", pDevice);
	pPSBloom.Init(L"PSBloom.cso", pDevice);
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
	ssaoCBuffer = std::make_unique<CBuffer<SSAOBuffer>>();
	ssaoCBuffer->Init(pDevice, pContext);
	bloomCBuffer = std::make_unique<CBuffer<bloomBuffer>>();
	bloomCBuffer->Init(pDevice, pContext);
}

void FSQuad::draw(ID3D11DeviceContext* pContext, Camera3D cam)
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
	if (fxaaEnabled && !blurEnabled)
	{
		pContext->IASetInputLayout(0);
		pVSFxaa.Bind(pContext);
		pPSFxaa.Bind(pContext);
	}
	//bind ssao shaders
	if (ssaoEnabled)
	{
		pContext->IASetInputLayout(0);
		pVSSsao.Bind(pContext);
		pPSSsao.Bind(pContext);
		ssaoCBuffer->data.area = area;
		ssaoCBuffer->data.totalStrength = totalStrength;
		ssaoCBuffer->data.fallOff = fallOff;
		ssaoCBuffer->data.base = base;
		ssaoCBuffer->data.radius = radius;
		ssaoCBuffer->MapData();
		ssaoCBuffer->PSBind(pContext, 2, 1);
	}
	//param cbuffer values bind
	paramCBuffer->data.blurEnabled = blurEnabled;
	paramCBuffer->data.blurIntensity = blurIntensity;
	paramCBuffer->data.exposure = exposure;
	paramCBuffer->data.gamma = gamma;
	paramCBuffer->data.toneMappingEnabled = toneMappingEnabled;
	paramCBuffer->data.bloomEnabled = bloomEnabled;
	paramCBuffer->data.autoExposureEnabled = autoExposureEnabled;
	paramCBuffer->MapData();
	paramCBuffer->PSBind(pContext, 0, 1);
	//fxaa cbuffer values bind
	fxaaCBuffer->data.rcpFrame = XMFLOAT4(1.0f / width, 1.0f / height, 0.0f, 0.0f);
	fxaaCBuffer->MapData();
	fxaaCBuffer->PSBind(pContext, 1, 1);
	if (bloomRender)
	{
		//input layout bind
		pLayout->Bind(pContext);

		//shaders bind
		pVSBloom.Bind(pContext);
		pPSBloom.Bind(pContext);

		//cbuffer bind
		bloomCBuffer->data.bloomIntensity = bloomIntensity;
		bloomCBuffer->MapData();
		bloomCBuffer->PSBind(pContext, 0, 1);
	}
	//draw quad
	pContext->DrawIndexed(pIndexBuffer->BufferSize(),
		0, 0);
}

bool* FSQuad::GetBlurEnabled()
{
	return &blurEnabled;
}

BOOL* FSQuad::GetAutoExposureEnabled()
{
	return &autoExposureEnabled;
}

float* FSQuad::GetBlurIntensity()
{
	return &blurIntensity;
}

bool FSQuad::SetBlurEnabled(bool value)
{
	return blurEnabled = value;
}

bool FSQuad::SetBloomRenderEnabled(bool value)
{
	return bloomRender = value;
}

BOOL FSQuad::SetToneMappingEnabled(BOOL value)
{
	return toneMappingEnabled = value;
}

BOOL FSQuad::SetBloomEnabled(BOOL value)
{
	return bloomEnabled = value;
}

BOOL FSQuad::SetAutoExposureEnabled(BOOL value)
{
	return autoExposureEnabled = value;
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

BOOL* FSQuad::GetSSAOEnabled()
{
	return &ssaoEnabled;
}

BOOL* FSQuad::GetToneMappingEnabled()
{
	return &toneMappingEnabled;
}

BOOL* FSQuad::GetBloomEnabled()
{
	return &bloomEnabled;
}

BOOL FSQuad::SetSSAOEnabled(BOOL value)
{
	return ssaoEnabled = value;
}

float* FSQuad::GetFallOff()
{
	return &fallOff;
}

float* FSQuad::GetTotalStrength()
{
	return &totalStrength;
}

float* FSQuad::GetArea()
{
	return &area;
}

float* FSQuad::GetRadius()
{
	return &radius;
}

float* FSQuad::GetBase()
{
	return &base;
}

float FSQuad::SetArea(float value)
{
	return area = value;
}

float FSQuad::SetBase(float value)
{
	return base = value;
}

float FSQuad::SetRadius(float value)
{
	return radius = value;
}

float FSQuad::SetTotalStrength(float value)
{
	return totalStrength = value;
}

float FSQuad::SetFallOff(float value)
{
	return fallOff = value;
}

float* FSQuad::GetExposure()
{
	return &exposure;
}

float* FSQuad::GetGamma()
{
	return &gamma;
}

float* FSQuad::GetBloomIntensity()
{
	return &bloomIntensity;
}

float FSQuad::SetExposure(float value)
{
	return exposure = value;
}

float FSQuad::SetGamma(float value)
{
	return gamma = value;
}

float FSQuad::SetBloomIntensity(float value)
{
	return bloomIntensity = value;
}
