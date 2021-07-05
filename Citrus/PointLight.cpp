#include "PointLight.h"

static float Spos[3] = { 0, 0.0f, 0.0f };
static float Srot[3] = { 0,0,0 };
static float Sscale[3] = { 10.0f, 10.0f, 10.0f };
static float diffuseIntensity = 1.0f;
static float ambientIntensity = 0.6f;
static float specularIntensity = 1.0f;
static float reflectionIntensity = 14.0f;
static BOOL normalMappingEnabled = TRUE;
static BOOL reflectionEnabled = FALSE;

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
	tex = std::make_unique<Texture>(device, context, "images\\white.png");
	//create and initialize constant buffer of ligth
	cblight = std::make_unique<CBuffer<Light>>();
	cblight->Init(device, context);
	//create and initialize constant buffer of ligth specular
	cblightspec = std::make_unique<CBuffer<LightSpec>>();
	cblightspec->Init(device, context);
	//model initialize
	lightmodel.InitNoMtl("models\\light.fbx", device, context);

	//set point light saved values
	pointLightSetting.OpenFileRead("pointlight_settings.txt");
	ambientIntensity = static_cast<float>(std::atoi(pointLightSetting.GetInfo(0).c_str()));
	diffuseIntensity = static_cast<float>(std::atoi(pointLightSetting.GetInfo(1).c_str()));
	specularIntensity = static_cast<float>(std::atoi(pointLightSetting.GetInfo(2).c_str()));
	reflectionIntensity = static_cast<float>(std::atoi(pointLightSetting.GetInfo(3).c_str()));
	lightmodel.SetPos(static_cast<float>(std::atoi(pointLightSetting.GetInfo(4).c_str())), static_cast<float>(std::atoi(pointLightSetting.GetInfo(5).c_str())),
		static_cast<float>(std::atoi(pointLightSetting.GetInfo(6).c_str())));
	if (pointLightSetting.GetInfo(7) == "1")
		normalMappingEnabled = TRUE;
	else
		normalMappingEnabled = FALSE;
	if (pointLightSetting.GetInfo(8) == "1")
		reflectionEnabled = TRUE;
	else
		reflectionEnabled = FALSE;
	pointLightSetting.CloseFile();

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
	cblight->data.reflectionEnabled = reflectionEnabled;	//constant buffer initialize
	cblight->data.reflectionIntensity = reflectionIntensity;	//constant buffer initialize
	cblight->MapData();	//apply data
	cblightspec->data.lightpos = lightmodel.GetPos();
	cblightspec->data.lightIntensity = diffuseIntensity;
	cblightspec->data.ambientIntensity = ambientIntensity;
	cblightspec->data.specularIntensity = specularIntensity;
	cblightspec->data.reflectionEnabled = reflectionEnabled;
	cblight->MapData();
	//point light ui creation
	ui->PointLight(&lightmodel, "Point Light", Spos, Srot, Sscale, &diffuseIntensity, &ambientIntensity, &normalMappingEnabled,
		&specularIntensity, &reflectionEnabled, &reflectionIntensity);
	//model render
	lightmodel.Render(cam);
}

void PointLight::BindCB() const
{
	cblight->PSBind(context, 0u, 1u);	//constant buffer bind
}

void PointLight::BindCBSpec() const
{
	cblightspec->PSBind(context, 0u, 1u);	//spec constant buffer bind
}

float PointLight::GetAmbientIntensity()
{
	return ambientIntensity; //return ambient intensity
}

float PointLight::GetDiffuseIntensity()
{
	return diffuseIntensity;	//return diffuse intenisty
}

float PointLight::GetSpecularIntensity()
{
	return specularIntensity;	//return specular intensity
}

float PointLight::GetReflectionIntensity()
{
	return reflectionIntensity;
}

float PointLight::GetObjectPositionX()
{
	return Spos[2];	//return object x position
}

float PointLight::GetObjectPositionY()
{
	return Spos[1]; //return object y position
}

float PointLight::GetObjectPositionZ()
{
	return Spos[0]; //return object z position
}

void PointLight::SetObjectPosition(const float x, const float y, const float z)
{
	lightmodel.SetPos(x, y, z);	//set object position to reference x,y,z
}

BOOL PointLight::GetNormalMapEnabled()
{
	return normalMappingEnabled;	//return normal mapping enabled as a BOOL (win32 boolean)
}

BOOL PointLight::GetReflectionEnabled()
{
	return reflectionEnabled;
}
