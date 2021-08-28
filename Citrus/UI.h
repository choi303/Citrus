#pragma once
#include "Model.h"
#include <DirectXMath.h>

class Graphics;
class App;
class GameObject;

class UI
{
public:
	UI() = default;

	void ClassicUI(GameObject* gameObject, std::string uiTitle, float pos[3], float rot[3], float scale[3],
		bool& isDestroyed) const;
	
	void PointLight(Model* model, const std::string uiTitle, float pos[3], float rot[3], float scale[3], float* Intensity,
		float* ambientIntensity, BOOL* normalMappingEnabled, float* specularIntensity, BOOL* reflectionEnabled, float* reflectionIntensity, XMFLOAT3* lightColor) const;
	
	static void DeveloperUI(std::string adapter_name, const std::string cpu_usage,
		const std::string fps, Camera3D* cam3d,
		bool* wireframeEnabled, XMFLOAT3* wireColor, bool* fogEnabled, XMFLOAT4* fogColor, float* fogStart,
		float* fogEnd, bool* vsync, bool* gridMapEnabled, XMFLOAT3* gridMapColor, Graphics* gfx, HWND hwnd, App* app, bool* msaaEnabled,
		bool* blurEnabled, float* blurIntensity, BOOL* ssaoEnabled, float* totalStrength, float* base,
		float* area, float* fallOff, float* radius, float* exposure, float* gamma, BOOL* toneMappingEnabled,
		float* bloomIntensity, BOOL* bloomEnabled);

	static void ToolBar(bool* gridMapEnabled, bool* wireframeEnabled, bool* fogEnabled,
		bool* depthBufferEnabled, bool* blurEnabled, bool* msaaEnabled, App* app, bool* fxaaEnabled,
		bool* backfaceCulling, bool* frontfaceCulling, App* rApp, BOOL* alphaClip, BOOL* ssaoEnabled,
		BOOL* toneMappingEnabled, BOOL* bloomEnabled, std::vector<GameObject*>& pGameObjects, 
		ID3D11Device* pDevice, ID3D11DeviceContext* pContext, int width, int height);

	static void SetCanRendered(bool value);
	static void SetVisiblity(bool value);
	static bool GetVisiblity() noexcept;

	static void ParticleUI(std::string uiTitle, float* mParticleDeviationX, float* mParticleDeviationY, float* mParticleDeviationZ,
		float* mParticleVelocity, float* mParticleVelocityVariation, float* mParticleSize, float* mAccumulatedTime,
		int* mCurrentParticleCount, float* lifeTime, bool* isKilled, bool* isLifetime, float* posX, float* posY,
		float* posZ);

	void DirectionalLigth(XMFLOAT4* diffuseColor,
		XMFLOAT3* lightDirection, XMFLOAT4* ambientColor, float* ambientIntensity,
		BOOL* normalMapEnabled, float* specularIntensity, float* diffuseIntensity,
		BOOL* reflectionEnabled, float* reflectionIntensity, float* bias, BOOL* pcfEnabled,
		BOOL* emessiveEnabled, float* emessiveIntensity);

	static void FireUI(float* posX, float* posY, float* posZ, float* sSpeedX, float* sSpeedY, float* sSpeedZ,
		float* scalesX, float* scalesY, float* scalesZ, float* distortion1X, float* distortion1Y,
		float* distortion2X, float* distortion2Y, float* distortion3X, float* distortion3Y,
		float* distortionScale, float* distortionBias, float* scaleX, float* scaleY, float* scaleZ);

	~UI() = default;
};

