#include "SkyBox.h"

void SkyBox::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	//set local device and context to reference device and context
	this->pDevice = pDevice;
	this->pContext = pContext;
	//init shaders
	pVS.Init(L"VSSkyBox.cso", pDevice);
	pPS.Init(L"PSSkyBox.cso", pDevice);
	//init and create input layout
	const std::vector<D3D11_INPUT_ELEMENT_DESC> skybox_ied =
	{
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Texcoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	pLayout = std::make_unique<InputLayout>(pDevice, skybox_ied, &pVS);
	//Create a skybox texture
	pTexture = std::make_unique<Texture>(pDevice, pContext, "Images\\SkyBox.png");
	//init sphere model for spherical skybox
	sphere.InitNoMtl("Models\\sphere_hq.obj", pDevice, pContext);

	//create pasteurizer
	CD3D11_RASTERIZER_DESC pRasterizerDesc(D3D11_DEFAULT);
	pRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pRasterizerDesc.CullMode = D3D11_CULL_FRONT;
	HRESULT hr = pDevice->CreateRasterizerState(&pRasterizerDesc, &pRasterizer);
	if (FAILED(hr)) { Error::Log(hr, "Failed to create pasteurizer state"); return; }
	//set sphere scale a large value
	sphere.SetScale(9999.0f, 9999.0f, 9999.0f);
}

void SkyBox::Draw(Camera3D cam)
{
	//bind rasterizer
	pContext->RSSetState(pRasterizer.Get());
	//bind skybox texture
	pTexture->Bind(pContext.Get());
	//bind input layout
	pLayout->Bind(pContext.Get());
	//bind shaders
	pVS.Bind(pContext.Get());
	pPS.Bind(pContext.Get());
	//render sphere mesh
	sphere.Render(cam);
}

void SkyBox::SetPos(XMFLOAT3 pos, float deltaTime)
{
	//set sky box to reference pos value and multiply with deltaTime
	sphere.SetPos(pos.x * 0.0001f, pos.y * 0.0001f, pos.z * 0.0001f);
}
