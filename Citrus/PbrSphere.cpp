#include "PbrSphere.h"

static XMFLOAT3 albedo = XMFLOAT3(1.0, 0.0, 0.0);
static float metallic = 0.7f;
static float roughness = 0.3f;
static float ao = 0.2f;

PbrSphere::PbrSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	this->pDevice = pDevice;
	this->pContext = pContext;

	//init shaders
	pVS.Init(L"VS_PBR.cso", pDevice);
	pPS.Init(L"PS_PBR.cso", pDevice);

	//init input layout
	const std::vector<D3D11_INPUT_ELEMENT_DESC> pbr_ied =
	{
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Texcoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Tangent", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Binormal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	pLayout = std::make_unique<InputLayout>(
		pDevice, pbr_ied, &pVS);

	//init model
	pModel.Init("Models\\sphere_hq.obj", pDevice, pContext);

	//cbuffer initalize
	pbrCBuffer = std::make_unique<CBuffer<PbrBuffer>>();
	pbrCBuffer->Init(pDevice, pContext);
}

void PbrSphere::Draw(Camera3D cam)
{
	//bind layout
	pLayout->Bind(pContext.Get());

	//bind cbuffers
	pbrCBuffer->data.albedo = albedo;
	pbrCBuffer->data.metallic = metallic;
	pbrCBuffer->data.roughness = roughness;
	pbrCBuffer->data.ao = ao;
	pbrCBuffer->MapData();
	pbrCBuffer->PSBind(pContext.Get(), 3, 1);

	//bind shaders
	pVS.Bind(pContext.Get());
	pPS.Bind(pContext.Get());

	//draw ui
	UI::PbrUI(&albedo, &metallic, &roughness, &ao);

	//draw model
	pModel.Render(cam);
}
