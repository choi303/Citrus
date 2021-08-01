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
static float pad;

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
	directLightSettings.OpenFileRead("directional_light_settings.txt");
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
	directLightSettings.CloseFile();
}

void DirectionalLight::BindCB(Camera3D cam)
{
	mLightCam.SetRotation(lightDirection.x, lightDirection.y, lightDirection.z);
	mLightCam.SetPosition(0,0, (cam.GetPositionFloat3().z + (-500)));
	mLightBuffer->data.diffuseColor = diffuseColor;
	mLightBuffer->data.lightDirection = lightDirection;
	mLightBuffer->data.ambientColor = ambientColor;
	mLightBuffer->data.ambientIntensity = ambientIntensity;
	mLightBuffer->data.normalMapEnabled = normalMapEnabled;
	mLightBuffer->data.diffuseIntensityC = diffuseIntensityC;
	mLightBuffer->data.specularIntensityC = specularIntensityC;
	mLightBuffer->data.reflectionEnabled = reflectionEnabled;
	mLightBuffer->data.reflectionIntensity = reflectionIntensity;
	mLightBuffer->data.biasC = bias;
	mLightBuffer->MapData();
	mLightBuffer->PSBind(mContext.Get(), 0, 1);
	mShadowCBuffer->data.shadowView = mLightCam.GetViewMatrix();
	mShadowCBuffer->data.shadowProj = mLightCam.GetProjectionMatrix();
	mShadowCBuffer->MapData();
	mShadowCBuffer->VSBind(mContext.Get(), 2, 1);
	mUI.DirectionalLigth(&diffuseColor, &lightDirection, &ambientColor, &ambientIntensity, &normalMapEnabled,
		&specularIntensityC, &diffuseIntensityC, &reflectionEnabled, &reflectionIntensity, &bias);
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