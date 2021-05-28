#include "GameObject.h"

static bool wireFrame;
static float Spos[3] = { 0,0,0 };
static float Srot[3] = { 0,0,0 };
static float Sscale[3] = { 1,1,1 };

bool GameObject::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, std::string filepath)
{
	this->pDevice = pDevice;
	this->pContext = pContext;
	//vertex shader initialize
	pVS.Init(L"PhongVS.cso", pDevice);
	//pixel shader initialize
	pPS.Init(L"PhongPSSpec.cso", pDevice);
	//gets data from vertex shader with input layout
	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Texcoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	//create i.l. and bind
	pLayout = std::make_unique<InputLayout>(pDevice, ied, &pVS);
	//point light initialize
	pointlight.Init(pDevice, pContext);
	//model initialize
	pModel.Init(filepath.c_str(), pDevice, pContext);

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

	//create rasterizer 
	CD3D11_RASTERIZER_DESC pRasterizerDesc(D3D11_DEFAULT);
	pRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pRasterizerDesc.CullMode = D3D11_CULL_BACK;
	hr = pDevice->CreateRasterizerState(&pRasterizerDesc, &pRasterizer);
	if (FAILED(hr)) { Error::Log(hr, "Failed to create rasterizer state"); return false; }

	//create rasterizer 
	CD3D11_RASTERIZER_DESC pRasterizerDescWireframe(D3D11_DEFAULT);
	pRasterizerDescWireframe.FillMode = D3D11_FILL_WIREFRAME;
	pRasterizerDescWireframe.CullMode = D3D11_CULL_BACK;
	hr = pDevice->CreateRasterizerState(&pRasterizerDescWireframe, &pRasterizerWireframe);
	if (FAILED(hr)) { Error::Log(hr, "Failed to create rasterizer state"); return false; }

    return true;
}

void GameObject::Draw(Camera3D cam)
{
	if (!wireFrame)
		pContext->RSSetState(pRasterizer.Get());
	else
		pContext->RSSetState(pRasterizerWireframe.Get());

	pointlight.Draw(pDevice, pContext, cam);

	pContext->PSSetSamplers(0u, 1u, st.GetAddressOf());
	pLayout->Bind(pContext);
	pVS.Bind(pContext);
	pPS.Bind(pContext);
	ui->ClassicUI(&pModel, "Model Settings", Spos, Srot, Sscale, &wireFrame);
	pointlight.BindCB(pContext);
	pModel.Render(cam);
}