#include "PointLight.h"

static float Spos[3] = { 0,100.0f,-20.0 };
static float Srot[3] = { 0,0,0 };
static float Sscale[3] = { 0.1f, 0.1f, 0.1f };
static float intensity = 250.0f;

bool PointLight::Init(ID3D11Device* device, ID3D11DeviceContext* context)
{
    if(!vs.Init(L"TextureVS.cso", device))
    {Error::Log("Failed to initialize phong vs shader"); return false; }

    if (!ps.Init(L"TexturePS.cso", device))
        {Error::Log("Failed to initialize phong ps shader"); return false; }

	//gets data from vertex shader with input layout
	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Texcoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	//create i.l. and bind
	il = std::make_unique<InputLayout>(device, ied, &vs);

	tex = std::make_unique<Texture>(device, context, "images\\seamless_grass.jpg");

	cblight = std::make_unique<CBuffer<Light>>();
	cblight->Init(device, context);

	lightmodel.InitNoMtl("models\\light.fbx", device, context);

    return true;
}

void PointLight::Draw(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Camera3D cam)
{
	il->Bind(pContext);
	vs.Bind(pContext);
	ps.Bind(pContext);
	tex->Bind(pContext);
	cblight->data.lightpos = lightmodel.GetPos();
	cblight->data.lightIntensity = intensity;
	cblight->MapData();
	ui->PointLight(&lightmodel, "Point Light", Spos, Srot, Sscale, &intensity);
	lightmodel.Render(cam);
}

void PointLight::BindCB(ID3D11DeviceContext* pContext)
{
	cblight->PSBind(pContext, 0u, 1u);
}

XMFLOAT3 PointLight::GetPos()
{
	return lightmodel.GetPos();
}