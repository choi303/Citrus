#include "PointLight.h"

static float Spos[3] = { 0,10.0f,-10.0 };
static float Srot[3] = { 0,0,0 };
static float Sscale[3] = { 0.1f, 0.1f, 0.1f };
static float intensity = 1.0f;
static float ambientIntensity = 0.6f;

bool PointLight::Init(ID3D11Device* device, ID3D11DeviceContext* context)
{
	//vertex shader initialize
	vs.Init(L"TextureVS.cso", device);
	//pixel shader initialize
	ps.Init(L"TexturePS.cso", device);

	//gets data from vertex shader with input layout
	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Texcoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	//create i.l. and bind
	il = std::make_unique<InputLayout>(device, ied, &vs);
	//create texture
	tex = std::make_unique<Texture>(device, context, "images\\seamless_grass.jpg");
	//create and initialize constant buffer of ligth
	cblight = std::make_unique<CBuffer<Light>>();
	cblight->Init(device, context);
	//model initialize
	lightmodel.InitNoMtl("models\\light.fbx", device, context);

    return true;
}

void PointLight::Draw(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Camera3D cam)
{
	il->Bind(pContext);	//input layout bind
	vs.Bind(pContext);	//vertex shader bind
	ps.Bind(pContext);	//pixel shader bind
	tex->Bind(pContext); //textrue bind
	cblight->data.lightpos = lightmodel.GetPos();		//constant buffer initialize
	cblight->data.lightIntensity = intensity;			//constant buffer initialize
	cblight->data.ambientIntensity = ambientIntensity;	//constant buffer initialize
	cblight->MapData();	//apply data
	//point light ui creation
	ui->PointLight(&lightmodel, "Point Light", Spos, Srot, Sscale, &intensity, &ambientIntensity);
	//model render
	lightmodel.Render(cam);
}

void PointLight::BindCB(ID3D11DeviceContext* pContext)
{
	cblight->PSBind(pContext, 0u, 1u);	//constant buffer bind
}