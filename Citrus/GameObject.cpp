#include "GameObject.h"

static bool wireframeEnabled;
static bool depthEnabled;
static bool fogEnabled;
static bool backfaceCulling;
static bool frontfaceCulling;
static float pos[3] = { 0,0,0 };
static float rot[3] = { 0,0,0 };
static float scale[3] = { 0.1f,0.1f,0.1f };

bool GameObject::init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, std::string filepath, int width, int height, bool hasMaterial)
{
	this->pDevice = pDevice;
	this->pContext = pContext;
	this->width = width;
	this->height = height;
	//normal mapping vertex shader initialize
	pVSNormal.Init(L"VSNormalMap.cso", pDevice);
	pPSNormal.Init(L"PSNormalMap.cso", pDevice);
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
	//gets data form vertex shader with input layout
	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Texcoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	//create vertex shader i.l. and bind
	pLayout = std::make_unique<InputLayout>(pDevice, ied, &pVS);
	//model initialize
	if(hasMaterial)
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

	//create pasteurizer 
	CD3D11_RASTERIZER_DESC pRasterizerDesc(D3D11_DEFAULT);
	pRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pRasterizerDesc.CullMode = D3D11_CULL_NONE;
	hr = pDevice->CreateRasterizerState(&pRasterizerDesc, &pRasterizer);
	if (FAILED(hr)) { Error::Log(hr, "Failed to create pasteurizer state"); return false; }

	//create pasteurizer 
	CD3D11_RASTERIZER_DESC pRasterizerDescBack(D3D11_DEFAULT);
	pRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pRasterizerDesc.CullMode = D3D11_CULL_BACK;
	hr = pDevice->CreateRasterizerState(&pRasterizerDescBack, &pRasterizerBack);
	if (FAILED(hr)) { Error::Log(hr, "Failed to create pasteurizer state"); return false; }

	//create pasteurizer 
	CD3D11_RASTERIZER_DESC pRasterizerDescFront(D3D11_DEFAULT);
	pRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pRasterizerDesc.CullMode = D3D11_CULL_FRONT;
	hr = pDevice->CreateRasterizerState(&pRasterizerDescFront, &pRasterizerFront);
	if (FAILED(hr)) { Error::Log(hr, "Failed to create pasteurizer state"); return false; }

	//create pasteurizer description 
	CD3D11_RASTERIZER_DESC pRasterizerDescWireframe(D3D11_DEFAULT);
	pRasterizerDescWireframe.FillMode = D3D11_FILL_WIREFRAME;
		pRasterizerDescWireframe.CullMode = D3D11_CULL_NONE;
	hr = pDevice->CreateRasterizerState(&pRasterizerDescWireframe, &pRasterizerWireframe);
	if (FAILED(hr)) { Error::Log(hr, "Failed to create pasteurizer state"); return false; }

	//matrices constant buffer initialize
	matrices_buffer = std::make_unique<CBuffer<matrices>>();
	matrices_buffer->Init(pDevice, pContext);

	//Filters Initialize
	pDepthBuffer.Init(pDevice, pContext);
	pWireframe.Init(pDevice, pContext);
	pFog.Init(pDevice, pContext);

    return true;
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

void GameObject::draw(Camera3D cam)
{
	//bind rasterizers
	pContext->RSSetState(pRasterizer.Get());
	if (backfaceCulling)
		pContext->RSSetState(pRasterizerBack.Get());
	else if (frontfaceCulling)
		pContext->RSSetState(pRasterizerFront.Get());

	//bind sampler
	pContext->PSSetSamplers(0u, 1u, st.GetAddressOf());
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

	//Classic Object UI Creation
	ui->ClassicUI(&pModel, directory.substr(directory.find_last_of("\\") + 1), pos, rot, scale);
	if (is_rendered)
	matrices_buffer->data.camera_pos = cam.GetPositionFloat3();
	matrices_buffer->MapData();
	matrices_buffer->VSBind(pContext, 1u, 1u);

	//if depth checkbox true draw depth buffer filter
	if (depthEnabled)
	{
		cam.SetProjectionValues(70.0f, static_cast<float>(width) / static_cast<float>(height)
			, 100.0f, 999999.0f * 999999.0f);
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
	is_rendered = true;
}

Model* GameObject::GetMesh()
{
	return &pModel;
}