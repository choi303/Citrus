#pragma once
#include "Model.h"
#include <DirectXMath.h>

class UI
{
public:
	UI() = default;
	void ClassicUI(Model* model, std::string uiTitle, float pos[3], float rot[3], float scale[3]) const;
	void PointLight(Model* model, const std::string uiTitle, float pos[3], float rot[3], float scale[3], float* Intensity,
		float* ambientIntensity, BOOL* normalMappingEnabled, float* specularIntensity) const;
	static void DeveloperUI(std::string adapter_name,const std::string cpu_usage, const std::string fps, Camera3D* cam3d, bool* depthBufferEnabled, bool* blurEnabled, bool* wireframeEnabled,
		XMFLOAT3* wireColor, bool* fogEnabled, XMFLOAT4* fogColor, float* fogStart, float* fogEnd);
	~UI() = default;
};

