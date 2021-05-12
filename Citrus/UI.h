#pragma once
#include "Model.h"

class UI
{
public:
	UI() = default;
	void ClassicUI(Model* model, std::string uiTitle, float pos[3], float rot[3], float scale[3]);
	void PointLight(Model* model, std::string uiTitle, float pos[3], float rot[3], float scale[3], float* Intensity);
	~UI() = default;
};

