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
	XMFLOAT3 pad;
	float ambientIntensity;
};

class PointLight
{
public:
	PointLight() = default;
	bool Init(ID3D11Device* device, ID3D11DeviceContext* context);
	void Draw(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Camera3D cam);
	void BindCB(ID3D11DeviceContext* pContext);
	~PointLight() = default;
private:
	Model lightmodel;
	VertexShader vs;
	PixelShader ps;
	std::unique_ptr<InputLayout> il;
	std::unique_ptr<Texture> tex;
	std::unique_ptr<UI> ui;
	std::unique_ptr<CBuffer<Light>> cblight;
};