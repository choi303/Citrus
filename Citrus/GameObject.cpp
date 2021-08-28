#include "GameObject.h"

static bool wireframeEnabled;
static bool depthEnabled;
static bool fogEnabled;
static bool backfaceCulling;
static bool frontfaceCulling;
static bool frontCull;
static float pos[3] = { 0,0,0 };
static float rot[3] = { 0,0,0 };
static float scale[3] = { 0.1f,0.1f,0.1f };

GameObject::GameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, std::string filepath, int width, int height, bool hasMaterial)
{
	this->pDevice = pDevice;
	this->pContext = pContext;
	this->width = width;
	this->height = height;
	//normal mapping shaders initialize
	pVSNormal.Init(L"VSDirectShadows.cso", pDevice);
	pPSNormal.Init(L"PSDirectShadows.cso", pDevice);
	//emssive mapping shaders initalize
	pVSEmessive.Init(L"VSDirectShadowsEmessive.cso", pDevice);
	pPSEmessive.Init(L"PSDirectShadowsEmessive.cso", pDevice);
	//gets data from normal mapping vertex shader with input layout
	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied_normal =
	{
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Texcoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Tangent", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Binormal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	//create normal mapping i.l. and bind
	pLayoutNormal = std::make_unique<InputLayout>(pDevice, ied_normal, &pVSNormal);
	//create vertex shader
	pVS.Init(L"PhongVS.cso", pDevice);
	pPS.Init(L"PhongPSSpec.cso", pDevice);
	pVSLit.Init(L"VSLit.cso", pDevice);
	pPSLit.Init(L"PSLit.cso", pDevice);
	//gets data form vertex shader with input layout
	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Texcoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	//create vertex shader i.l. and bind
	pLayout = std::make_unique<InputLayout>(pDevice, ied, &pVS);
	pLayoutLit = std::make_unique<InputLayout>(pDevice, ied, &pVSLit);
	//model initialize
	if (hasMaterial)
		pModel.Init(filepath.c_str(), pDevice, pContext);
	else
		pModel.InitNoMtl(filepath.c_str(), pDevice, pContext);
	this->directory = filepath;

	//create sampler state
	CD3D11_SAMPLER_DESC sampler_desc(D3D11_DEFAULT);
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampler_desc.MinLOD = 0;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
	HRESULT hr = pDevice->CreateSamplerState(&sampler_desc, &st);
	if (FAILED(hr)) { Error::Log(hr, "Failed to create sampler state"); }

	CD3D11_SAMPLER_DESC pcfDesc(D3D11_DEFAULT);
	pcfDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	pcfDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	pcfDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	pcfDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	pcfDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	pcfDesc.BorderColor[0] = 1.0f;
	pcfDesc.BorderColor[1] = 1.0f;
	pcfDesc.BorderColor[2] = 1.0f;
	pcfDesc.BorderColor[3] = 1.0f;
	hr = pDevice->CreateSamplerState(&pcfDesc, &pcfSam);
	if (FAILED(hr)) { Error::Log(hr, "Failed to create sampler state"); }

	D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = pDevice->CreateSamplerState(&samplerDesc, &ssam);
	if (FAILED(hr)) { Error::Log(hr, "Failed to create sampler state"); }

	//create pasteurizer 
	CD3D11_RASTERIZER_DESC pRasterizerDesc(D3D11_DEFAULT);
	pRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pRasterizerDesc.CullMode = D3D11_CULL_NONE;
	hr = pDevice->CreateRasterizerState(&pRasterizerDesc, &pRasterizer);
	if (FAILED(hr)) { Error::Log(hr, "Failed to create pasteurizer state"); }

	//create pasteurizer 
	CD3D11_RASTERIZER_DESC pRasterizerDescBack(D3D11_DEFAULT);
	pRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pRasterizerDesc.CullMode = D3D11_CULL_BACK;
	hr = pDevice->CreateRasterizerState(&pRasterizerDescBack, &pRasterizerBack);
	if (FAILED(hr)) { Error::Log(hr, "Failed to create pasteurizer state"); }

	//create pasteurizer 
	CD3D11_RASTERIZER_DESC pRasterizerDescFront(D3D11_DEFAULT);
	pRasterizerDescFront.FillMode = D3D11_FILL_SOLID;
	pRasterizerDescFront.CullMode = D3D11_CULL_FRONT;
	hr = pDevice->CreateRasterizerState(&pRasterizerDescFront, &pRasterizerFront);
	if (FAILED(hr)) { Error::Log(hr, "Failed to create pasteurizer state"); }

	//create pasteurizer description 
	CD3D11_RASTERIZER_DESC pRasterizerDescWireframe(D3D11_DEFAULT);
	pRasterizerDescWireframe.FillMode = D3D11_FILL_WIREFRAME;
	pRasterizerDescWireframe.CullMode = D3D11_CULL_NONE;
	hr = pDevice->CreateRasterizerState(&pRasterizerDescWireframe, &pRasterizerWireframe);
	if (FAILED(hr)) { Error::Log(hr, "Failed to create pasteurizer state"); }

	//matrices constant buffer initialize
	matrices_buffer = std::make_unique<CBuffer<matrices>>();
	matrices_buffer->Init(pDevice, pContext);

	//Filters Initialize
	pDepthBuffer.Init(pDevice, pContext);
	pWireframe.Init(pDevice, pContext);
	pFog.Init(pDevice, pContext);
}

bool GameObject::HasNormal() const
{
	//if has a normal return true else return false
	return pModel.GetHasNormal();
}

bool* GameObject::GetDepthBufferEnabled()
{
	//if depth is enabled return true else return false
	return &depthEnabled;
}

bool* GameObject::GetWireframeEnabled()
{
	//if wireframe filter is enabled than return true else return false
	return &wireframeEnabled;
}

bool* GameObject::GetFogEnabled()
{
	//if fog filter is enabled than return true else return false
	return &fogEnabled;
}

bool* GameObject::GetBackCulling()
{
	return &backfaceCulling;
}

bool* GameObject::GetFrontCulling()
{
	return &frontfaceCulling;
}

XMFLOAT3* GameObject::GetWireColor()
{
	//return wire Color
	return Wireframe::GetWireColor();
}

XMFLOAT4* GameObject::GetFogColor()
{	
	//return fog Color
	return Fog::GetFogColor();
}

float* GameObject::GetFogStart()
{
	//return fog start value
	return Fog::GetFogStart();
}

float* GameObject::GetFogEnd()
{
	//return fog end value
	return Fog::GetFogEnd();
}

bool GameObject::SetDepthBufferEnabled(bool value)
{
	//set depth buffer enabled to value
	return depthEnabled = value;
}

bool GameObject::SetFogEnabled(bool value)
{
	//set fog enabled to value
	return fogEnabled = value;
}

bool GameObject::SetWireframeEnabled(bool value)
{
	//set wireframe enabled to value
	return wireframeEnabled = value;
}

bool GameObject::SetFrontCull(bool value)
{
	return frontCull = value;
}

float GameObject::SetFogStart(float value)
{
	//set fog start to value
	return Fog::SetFogStart(value);
}

float GameObject::SetFogEnd(float value)
{
	//set fog end to value
	return Fog::SetFogEnd(value);
}

std::string GameObject::GetName() const
{
	return directory.substr(directory.find_last_of("\\") + 1);
}

bool& GameObject::GetIsDestroyed() const
{
	return isDestroyed;
}

void GameObject::draw(Camera3D cam)
{
	pContext->RSSetState(pRasterizerFront.Get());
	if (!frontCull)
	{
		//bind rasterizers
		pContext->RSSetState(pRasterizer.Get());
		if (backfaceCulling)
			pContext->RSSetState(pRasterizerBack.Get());
		else if (frontfaceCulling)
			pContext->RSSetState(pRasterizerFront.Get());
	}
	//bind sampler
	pContext->PSSetSamplers(0u, 1u, st.GetAddressOf());
	pContext->PSSetSamplers(1u, 1u, ssam.GetAddressOf());
	pContext->PSSetSamplers(2u, 1u, pcfSam.GetAddressOf());
	//if model has not any textures then bind no texture shaders
	if (!pModel.GetHasTexture())
	{
		pLayout->Bind(pContext);
		pVS.Bind(pContext);
		pPS.Bind(pContext);
	}
	//if model not has normal texture and just set shaders
	if (!pModel.GetHasNormal())
	{
		pLayout->Bind(pContext);
		pVS.Bind(pContext);
		pPS.Bind(pContext);
	}
	//if has a normal map and set normal mapping shaders
	else
	{
		pLayoutNormal->Bind(pContext);
		pVSNormal.Bind(pContext);
		pPSNormal.Bind(pContext);
	}

	//if model has a emessive map and set emssive shaders
	if (pModel.GetHasEmessive())
	{
		pVSEmessive.Bind(pContext);
		pPSEmessive.Bind(pContext);
	}

	//Classic Object UI Creation
	ui->ClassicUI(this, directory.substr(directory.find_last_of("\\") + 1), pos, rot, scale,
		isDestroyed);
	if (isRendered)
	matrices_buffer->data.camera_pos = cam.GetPositionFloat3();
	matrices_buffer->MapData();
	matrices_buffer->VSBind(pContext, 1u, 1u);

	//if depth checkbox true draw depth buffer filter
	if (depthEnabled)
	{
		cam.SetProjectionValues(70.0f, static_cast<float>(width) / static_cast<float>(height)
			, 1.0f, 999999.0f * 999999.0f, false);
		pDepthBuffer.Draw();
	}

	//if wire frame checkbox is true than draw wire frame filter
	if (wireframeEnabled)
	{
		pContext->RSSetState(pRasterizerWireframe.Get());
		pWireframe.Draw();
	}

	//if fog checkbox is true than draw fog filter
	if (fogEnabled)
	{
		pFog.Draw();
	}

	//Render Model
	pModel.Render(cam);
	isRendered = true;
}

void GameObject::Destroy() const noexcept
{
	pModel.Destroy();
	isDestroyed = true;
}

Model* GameObject::GetMesh()
{
	return &pModel;
}
