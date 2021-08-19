#include "DirectionalLight.h"

static XMFLOAT4 diffuseColor = XMFLOAT4(1, 1, 1, 1);
static XMFLOAT4 ambientColor = XMFLOAT4(1, 1, 1, 1);
static XMFLOAT3 lightDirection;
static float ambientIntensity;
static BOOL normalMapEnabled = TRUE;
static BOOL reflectionEnabled = TRUE;
static float specularIntensityC = 1.0f;
static float diffuseIntensityC = 1.0f;
static float reflectionIntensity = 1.0f;
static float bias = 0.0001f;
static BOOL pcfEnabled;
static BOOL alphaClip;
static BOOL normals;
static BOOL emessiveEnabled;
static float emessiveIntensity = 1.0f;

DirectionalLight::DirectionalLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, int width, int height)
	:
	mDevice(pDevice),
	mContext(pContext),
	mWidth(width),
	mHeight(height)
{
	mLightBuffer = std::make_unique<CBuffer<DirectLight>>();
	mLightBuffer->Init(pDevice, pContext);
	//Light camera 3d
	mLightCam.SetPosition(lightDirection.x, lightDirection.y, lightDirection.z);
	mLightCam.SetProjectionValues(90.0f, static_cast<float>(width) / static_cast<float>(height), 0.01f, 999.0f, true);
	mShadowCBuffer = std::make_unique<CBuffer<shadowmap>>();
	mShadowCBuffer->Init(pDevice, pContext);

	//set directional light saved values
	directLightSettings.OpenFileRead("directional_light_settings.cfg");
	SetDireciton(std::stof(directLightSettings.GetInfo(0)), std::stof(directLightSettings.GetInfo(1)),
		std::stof(directLightSettings.GetInfo(2)));
	SetAmbientIntensity(std::stof(
		directLightSettings.GetInfo(3)));
	SetDiffuseIntensity(std::stof(
		directLightSettings.GetInfo(4)));
	SetReflectionIntensity(std::stof(
		directLightSettings.GetInfo(5)));
	SetSpecularIntensity(std::stof(
		directLightSettings.GetInfo(6)));
	SetBias(std::stof(
		directLightSettings.GetInfo(7)));
	if (directLightSettings.GetInfo(8) == "1")
		SetNormalMapEnabled(TRUE);
	else
		SetNormalMapEnabled(FALSE);
	if (directLightSettings.GetInfo(9) == "1")
		SetReflectionEnabled(TRUE);
	else
		SetReflectionEnabled(FALSE);
	if (directLightSettings.GetInfo(10) == "1")
		SetPCFEnabled(TRUE);
	else
		SetPCFEnabled(FALSE);
	if (directLightSettings.GetInfo(11) == "1")
		SetAlphaCEnabled(TRUE);
	else
		SetAlphaCEnabled(FALSE);
	if (directLightSettings.GetInfo(12) == "1")
		emessiveEnabled = TRUE;
	else
		emessiveEnabled = FALSE;
	emessiveIntensity = std::stof(directLightSettings.GetInfo(13).c_str());
	directLightSettings.CloseFile();
	
	//init cbuffer
	mLightColor = std::make_unique<CBuffer<LightColor>>();
	mLightColor->Init(pDevice, pContext);

	//init shaders
	pVS.Init(L"ColorVertex.cso", mDevice.Get());
	pPS.Init(L"ColorPixel.cso", mDevice.Get());
	//init and create input layout
	const std::vector<D3D11_INPUT_ELEMENT_DESC> direct_ied
		=
	{
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Color", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	pLayout = std::make_unique<InputLayout>(pDevice, direct_ied, &pVS);

	pModel.InitNoMtl("Models\\sphere_hq.obj", pDevice, pContext);
}

void DirectionalLight::BindCB(Camera3D cam)
{
	pLayout->Bind(mContext.Get());
	pVS.Bind(mContext.Get());
	pPS.Bind(mContext.Get());
	mLightColor->data.color = diffuseColor;
	mLightColor->MapData();
	mLightColor->PSBind(mContext.Get(), 4, 1);
	pModel.SetScale(50.0f, 50.0f, 50.0f);
	pModel.SetPos(45.0f, 5.0f, -90.0f);
	pModel.SetRot(lightDirection.x, lightDirection.y, lightDirection.z);
	pModel.Render(cam);
	mLightCam.SetPosition(lightDirection.x, lightDirection.y, lightDirection.z + (-50));
	mLightCam.SetRotation(lightDirection.x, lightDirection.y, lightDirection.z + (-50));
	mLightBuffer->data.diffuseColor = diffuseColor;
	mLightBuffer->data.lightDirection = lightDirection;
	mLightBuffer->data.ambientColor = ambientColor;
	mLightBuffer->data.ambientIntensity = ambientIntensity;
	mLightBuffer->data.normalMapEnabled = normalMapEnabled;
	mLightBuffer->data.diffuseIntensityC = diffuseIntensityC;
	mLightBuffer->data.specularIntensityC = specularIntensityC;
	mLightBuffer->data.reflectionEnabled = reflectionEnabled;
	mLightBuffer->data.reflectionIntensity = reflectionIntensity;
	mLightBuffer->data.pcfEnabled = pcfEnabled;
	mLightBuffer->data.biasC = bias;
	mLightBuffer->data.alphaClip = alphaClip;
	mLightBuffer->data.normals = normals;
	mLightBuffer->data.emessiveEnabled = emessiveEnabled;
	mLightBuffer->data.emessiveIntensity = emessiveIntensity;
	mLightBuffer->MapData();
	mLightBuffer->PSBind(mContext.Get(), 0, 1);
	mShadowCBuffer->data.shadowView = mLightCam.GetViewMatrix();
	mShadowCBuffer->data.shadowProj = mLightCam.GetProjectionMatrix();
	mShadowCBuffer->MapData();
	mShadowCBuffer->VSBind(mContext.Get(), 2, 1);
	mUI.DirectionalLigth(&diffuseColor, &lightDirection, &ambientColor, &ambientIntensity, &normalMapEnabled,
		&specularIntensityC, &diffuseIntensityC, &reflectionEnabled, &reflectionIntensity, &bias, &pcfEnabled,
		&emessiveEnabled, &emessiveIntensity);
}

Camera3D DirectionalLight::GetLightCamera()
{
	return mLightCam;
}

XMFLOAT3 DirectionalLight::GetDirection() const noexcept
{
	return lightDirection;
}

void DirectionalLight::SetDireciton(XMFLOAT3 value)
{
	lightDirection = value;
}

void DirectionalLight::SetDireciton(float x, float y,
	float z)
{
	lightDirection = XMFLOAT3(x,y,z);
}

float DirectionalLight::GetAmbientIntensity() const noexcept
{
	return ambientIntensity;
}

void DirectionalLight::SetAmbientIntensity(float value)
{
	ambientIntensity = value;
}

BOOL DirectionalLight::GetNormalMapEnabled() const noexcept
{
	return normalMapEnabled;
}

void DirectionalLight::SetNormalMapEnabled(BOOL value)
{
	normalMapEnabled = value;
}

float DirectionalLight::GetSpecularIntensity() const noexcept
{
	return specularIntensityC;
}

void DirectionalLight::SetSpecularIntensity(float value)
{
	specularIntensityC = value;
}

float DirectionalLight::GetDiffuseIntensity() const noexcept
{
	return diffuseIntensityC;
}

void DirectionalLight::SetDiffuseIntensity(float value)
{
	diffuseIntensityC = value;
}

float DirectionalLight::GetBias() const noexcept
{
	return bias;
}

void DirectionalLight::SetBias(float value)
{
	bias = value;
}

float DirectionalLight::GetReflectionIntensity() const noexcept
{
	return reflectionIntensity;
}

void DirectionalLight::SetReflectionIntensity(float value)
{
	reflectionIntensity = value;
}

BOOL DirectionalLight::GetReflectionEnabled() const noexcept
{
	return reflectionEnabled;
}

void DirectionalLight::SetReflectionEnabled(BOOL value)
{
	reflectionEnabled = value;
}

void DirectionalLight::SetPCFEnabled(BOOL value)
{
	pcfEnabled = value;
}

BOOL DirectionalLight::GetPCFEnabled() const noexcept
{
	return pcfEnabled;
}

BOOL* DirectionalLight::GetAlphaCEnabled()
{
	return &alphaClip;
}

void DirectionalLight::SetAlphaCEnabled(BOOL value)
{
	alphaClip = value;
}

void DirectionalLight::SetNormalsEnabled(BOOL value)
{
	normals = value;
}

BOOL* DirectionalLight::GetEmessiveEnabled()
{
	return &emessiveEnabled;
}

float DirectionalLight::GetEmessiveIntensity() const noexcept
{
	return emessiveIntensity;
}