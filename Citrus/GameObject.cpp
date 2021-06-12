#include "GameObject.h"

static bool wireFrame;
static bool depthEnabled = true;
static float pos[3] = { 0,0,0 };
static float rot[3] = { 0,0,0 };
static float scale[3] = { 0.1f,0.1f,0.1f };

bool GameObject::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, std::string filepath)
{
	this->pDevice = pDevice;
	this->pContext = pContext;
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
	pModel.Init(filepath.c_str(), pDevice, pContext);
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

	//create pasteurizer description 
	CD3D11_RASTERIZER_DESC pRasterizerDescWireframe(D3D11_DEFAULT);
	pRasterizerDescWireframe.FillMode = D3D11_FILL_WIREFRAME;
	pRasterizerDescWireframe.CullMode = D3D11_CULL_NONE;
	hr = pDevice->CreateRasterizerState(&pRasterizerDescWireframe, &pRasterizerWireframe);
	if (FAILED(hr)) { Error::Log(hr, "Failed to create pasteurizer state"); return false; }

	//matrices constant buffer initialize
	matrices_buffer = std::make_unique<CBuffer<matrices>>();
	matrices_buffer->Init(pDevice, pContext);

	pDepthBuffer.Init(pDevice, pContext);

    return true;
}

bool GameObject::HasNormal() const
{
	return pModel.GetHasNormal();
}

bool* GameObject::GetDepthBufferEnabled()
{
	return &depthEnabled;
}

void GameObject::Draw(Camera3D cam)
{
	//bind rasterizers
	if (!wireFrame)
		pContext->RSSetState(pRasterizer.Get());
	else
		pContext->RSSetState(pRasterizerWireframe.Get());
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

	ui->ClassicUI(&pModel, directory.substr(directory.find_last_of("\\") + 1), pos, rot, scale, &wireFrame);
	if (is_rendered)
	matrices_buffer->data.camera_pos = cam.GetPositionFloat3();
	matrices_buffer->MapData();
	matrices_buffer->VSBind(pContext, 1u, 1u);

	if (depthEnabled)
	{
		pDepthBuffer.Draw();
	}

	pModel.Render(cam);
	is_rendered = true;
}

Model* GameObject::GetMesh()
{
	return &pModel;
}