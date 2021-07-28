#pragma once
#include "Model.h"
#include "Shaders.h"
#include "InputLayout.h"
#include "UI.h"
#include "Texture.h"
#include "SaveSystem.h"

struct Light
{
	XMFLOAT3 lightpos;
	float lightIntensity;
	float ambientIntensity;
	BOOL normalMappingEnabled;
	float specularIntensity;
	BOOL reflectionEnabled;
	float reflectionIntensity;
	XMFLOAT3 lightColor;
	float pad[2];
};

struct LightSpec
{
	XMFLOAT3 lightpos;
	float lightIntensity;
	float ambientIntensity;
	float specularIntensity;
	BOOL reflectionEnabled;
	float pad;
};

class PointLight
{
public:
	PointLight() = default;
	bool Init(ID3D11Device* device, ID3D11DeviceContext* context, int width, int height);
	void Draw(Camera3D cam);
	void BindCB() const;
	void BindCBSpec() const;
	~PointLight() = default;
public:
	static float GetAmbientIntensity();
	static float GetDiffuseIntensity();
	static float GetSpecularIntensity();
	static float GetReflectionIntensity();
	static float GetObjectPositionX();
	static float GetObjectPositionY();
	static float GetObjectPositionZ();
	static XMFLOAT3 GetLightColor();
	void SetObjectPosition(const float x, const float y, const float z);
	static BOOL GetNormalMapEnabled();
	static BOOL GetReflectionEnabled();
	Camera3D GetLightCamera();
public:
	mutable SaveSystem pointLightSetting;
	mutable std::vector<std::string> pPointLightSavedItems;
private:
	Model lightmodel;
	VertexShader vs;
	PixelShader ps;
	Camera3D lightCam;
	std::unique_ptr<InputLayout> il;
	std::unique_ptr<Texture> tex;
	std::unique_ptr<UI> ui;
	std::unique_ptr<CBuffer<Light>> cblight;
	std::unique_ptr<CBuffer<LightSpec>> cblightspec;
private:
	ID3D11Device* device;
	ID3D11DeviceContext* context;
};