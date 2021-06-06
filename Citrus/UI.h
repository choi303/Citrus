#pragma once
#include "Model.h"

class UI
{
public:
	UI() = default;
	void ClassicUI(Model* model, std::string uiTitle, float pos[3], float rot[3], float scale[3], bool* wireframe) const;
	void PointLight(Model* model, std::string uiTitle, float pos[3], float rot[3], float scale[3], float* Intensity,
		float* ambientIntenisty, BOOL* normalMappingEnabled, float* specularlIntensity) const;
	static void DeveloperUI(const std::string fps, Camera3D* cam3d);
	~UI() = default;
};

