#pragma once
#include "Model.h"
#include "Shaders.h"
#include "InputLayout.h"
#include "UI.h"
#include "Texture.h"

struct Light
{
	XMFLOAT3 lightpos;
	float lightIntensity;
	float ambientIntensity;
	BOOL normalMappingEnabled;
	float specularIntensity;
};

struct LightSpec
{
	XMFLOAT3 lightpos;
	float lightIntensity;
	float ambientIntensity;
	float specularIntensity;
	float pad[2];
};

class PointLight
{
public:
	PointLight() = default;
	bool Init(ID3D11Device* device, ID3D11DeviceContext* context);
	void Draw(Camera3D cam);
	void BindCB() const;
	void BindCBSpec() const;
	~PointLight() = default;
public:
	static float GetAmbientIntensity();
	static float SetAmbientIntensity(float value);
	static float GetDiffuseIntensity();
	static float SetDiffuseIntensity(float value);
	static float GetSpecularIntensity();
	static float SetSpecularIntensity(float value);
	static float GetObjectPositionX();
	static float GetObjectPositionY();
	static float GetObjectPositionZ();
	static void SetObjectPosition(float x, float y, float z);
	static BOOL GetNormalMapEnabled();
	static BOOL SetNormalMapEnabled(BOOL value);
private:
	Model lightmodel;
	VertexShader vs;
	PixelShader ps;
	std::unique_ptr<InputLayout> il;
	std::unique_ptr<Texture> tex;
	std::unique_ptr<UI> ui;
	std::unique_ptr<CBuffer<Light>> cblight;
	std::unique_ptr<CBuffer<LightSpec>> cblightspec;
private:
	ID3D11Device* device;
	ID3D11DeviceContext* context;
};