#include "PbrSphere.h"

static XMFLOAT3 albedo = XMFLOAT3(1.0, 0.0, 0.0);
static float metallic = 0.7f;
static float roughness = 0.3f;
static float ao = 0.2f;
static float pos[3] = { 0,0,0 };
static float rot[3] = { 0,0,0 };
static float scale[3] = { 0.1f,0.1f,0.1f };

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
	pModel.InitPbr("Models\\pbrGun\\Cerberus_LP.FBX.fbx", pDevice, pContext);

	//cbuffer initalize
	pbrCBuffer = std::make_unique<CBuffer<PbrBuffer>>();
	pbrCBuffer->Init(pDevice, pContext);

	//initialize textures
	pAlbedo = std::make_shared<Texture>(pDevice, pContext, "Models\\pbr_gun\\Cerberus_A.tga.png", 0);
	pNormal = std::make_shared<Texture>(pDevice, pContext, "Models\\pbr_gun\\Cerberus_N.tga.png", 1);
	pMetallic = std::make_shared<Texture>(pDevice, pContext, "Models\\pbr_gun\\Cerberus_M.tga.png", 2);
	pRoughness = std::make_shared<Texture>(pDevice, pContext, "Models\\pbr_gun\\Cerberus_R.tga.png", 3);
	pAO = std::make_shared<Texture>(pDevice, pContext, "Models\\pbr_gun\\Cerberus_AO.tga.png", 4);
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
	pbrCBuffer->PSBind(pContext.Get(), 9, 1);

	//bind shades
	pVS.Bind(pContext.Get());
	pPS.Bind(pContext.Get());

	//draw ui
	UI::PbrUI(&pModel, pos, rot, scale, &albedo, &metallic, &roughness, &ao, pContext.Get(),
		pAlbedo, pNormal, pMetallic, pRoughness, pAO);

	//bind textures
	pAlbedo->Bind(pContext.Get());
	pNormal->Bind(pContext.Get());
	pMetallic->Bind(pContext.Get());
	pRoughness->Bind(pContext.Get());
	pAO->Bind(pContext.Get());

	//draw model
	pModel.Render(cam);
}
