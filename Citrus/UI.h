#pragma once
#include "Model.h"
#include <DirectXMath.h>
using namespace DirectX;
class UI
{
public:
	UI() = default;
	void ClassicUI(Model* model, std::string uiTitle, float pos[3], float rot[3], float scale[3]) const;
	void PointLight(Model* model, std::string uiTitle, float pos[3], float rot[3], float scale[3], float* Intensity,
		float* ambientIntenisty, BOOL* normalMappingEnabled, float* specularlIntensity) const;
	static void DeveloperUI(std::string ms, const std::string cpu_usage, const std::string fps, Camera3D* cam3d, bool* depthBufferEnabled, bool* blurEnabled, bool* wireframeEnabled,
		XMFLOAT3* wireColor);
	~UI() = default;
};

