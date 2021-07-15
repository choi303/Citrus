#pragma once
#include "Model.h"
#include <DirectXMath.h>

class Graphics;
class App;

class UI
{
public:
	UI() = default;

	void ClassicUI(Model* model, std::string uiTitle, float pos[3], float rot[3], float scale[3]) const;
	
	void PointLight(Model* model, const std::string uiTitle, float pos[3], float rot[3], float scale[3], float* Intensity,
		float* ambientIntensity, BOOL* normalMappingEnabled, float* specularIntensity, BOOL* reflectionEnabled, float* reflectionIntensity) const;
	
	static void DeveloperUI(std::string adapter_name, const std::string cpu_usage,
		const std::string fps, Camera3D* cam3d,
		bool* wireframeEnabled, XMFLOAT3* wireColor, bool* fogEnabled, XMFLOAT4* fogColor, float* fogStart,
		float* fogEnd, bool* vsync, bool* gridMapEnabled, XMFLOAT3* gridMapColor, Graphics* gfx, HWND hwnd, App* app, bool* msaaEnabled,
		bool* blurEnabled, float* blurIntensity);

	static void ToolBar(bool* gridMapEnabled, bool* wireframeEnabled, bool* fogEnabled,
		bool* depthBufferEnabled, bool* blurEnabled, bool* msaaEnabled, App* app);
	~UI() = default;
};

