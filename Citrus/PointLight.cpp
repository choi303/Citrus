#include "PointLight.h"

static float Spos[3] = { 0,1.0f, -1.0 };
static float Srot[3] = { 0,0,0 };
static float Sscale[3] = { -0.0f, -0.0f, -0.0f };
static float diffuseIntensity = 1.0f;
static float ambientIntensity = 0.6f;
static float specularIntensity = 1.0f;
static BOOL normalMappingEnabled = TRUE;

bool PointLight::Init(ID3D11Device* device, ID3D11DeviceContext* context)
{
	this->device = device;
	this->context = context;
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
	tex = std::make_unique<Texture>(device, context, "images\\bill.png");
	//create and initialize constant buffer of ligth
	cblight = std::make_unique<CBuffer<Light>>();
	cblight->Init(device, context);
	//create and initialize constant buffer of ligth specular
	cblightspec = std::make_unique<CBuffer<LightSpec>>();
	cblightspec->Init(device, context);
	//model initialize
	lightmodel.InitNoMtl("models\\light.fbx", device, context);

    return true;
}

void PointLight::Draw(Camera3D cam)
{
	il->Bind(context);	//input layout bind
	vs.Bind(context);	//vertex shader bind
	ps.Bind(context);	//pixel shader bind
	tex->Bind(context); //textrue bind
	cblight->data.lightpos = lightmodel.GetPos();		//constant buffer initialize
	cblight->data.lightIntensity = diffuseIntensity;			//constant buffer initialize
	cblight->data.ambientIntensity = ambientIntensity;	//constant buffer initialize
	cblight->data.normalMappingEnabled = normalMappingEnabled;	//constant buffer initialize
	cblight->data.specularIntensity = specularIntensity;	//constant buffer initialize
	cblight->MapData();	//apply data
	cblightspec->data.lightpos = lightmodel.GetPos();
	cblightspec->data.lightIntensity = diffuseIntensity;
	cblightspec->data.ambientIntensity = ambientIntensity;
	cblightspec->data.specularIntensity = specularIntensity;
	cblight->MapData();
	//point light ui creation
	ui->PointLight(&lightmodel, "Point Light", Spos, Srot, Sscale, &diffuseIntensity, &ambientIntensity, &normalMappingEnabled,
		&specularIntensity);
	//model render
	lightmodel.Render(cam);
}

void PointLight::BindCB() const
{
	cblight->PSBind(context, 0u, 1u);	//constant buffer bind
}

void PointLight::BindCBSpec() const
{
	cblightspec->PSBind(context, 0u, 1u);
}

float PointLight::GetAmbientIntensity()
{
	return ambientIntensity;
}

float PointLight::SetAmbientIntensity(float value)
{
	return ambientIntensity = value;
}

float PointLight::GetDiffuseIntensity()
{
	return diffuseIntensity;
}

float PointLight::SetDiffuseIntensity(float value)
{
	return diffuseIntensity = value;
}

float PointLight::GetSpecularIntensity()
{
	return specularIntensity;
}

float PointLight::SetSpecularIntensity(float value)
{
	return specularIntensity = value;
}

float PointLight::GetObjectPositionX()
{
	return Spos[2];
}

float PointLight::GetObjectPositionY()
{
	return Spos[1];
}

float PointLight::GetObjectPositionZ()
{
	return Spos[0];
}

void PointLight::SetObjectPosition(float x, float y, float z)
{
	Spos[0] = x;
	Spos[1] = y;
	Spos[2] = z;
}

BOOL PointLight::GetNormalMapEnabled()
{
	return normalMappingEnabled;
}

BOOL PointLight::SetNormalMapEnabled(BOOL value)
{
	return normalMappingEnabled = value;
}