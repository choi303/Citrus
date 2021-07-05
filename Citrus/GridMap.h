#pragma once
#include "GfxBase.h"

class GridMap
{
public:
	GridMap() = default;
	GridMap(GridMap& rhs) = default;
	~GridMap() = default;

	bool init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void draw(Camera3D cam);
	static bool* getRender();
	static XMFLOAT3* getColor();
	static bool setRender(bool value);
private:
	wrl::ComPtr<ID3D11Device> pDevice;
	wrl::ComPtr<ID3D11DeviceContext> pContext;
	struct vertex
	{
		XMFLOAT3 pos;
	};
	struct color
	{
		XMFLOAT3 color;
		float pad;
	};
	struct transform
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};
	std::vector<vertex> vertices;
	std::unique_ptr<VertexBuffer<vertex>> pVertexBuffer;
	std::unique_ptr<InputLayout> pLayout;
	std::unique_ptr<CBuffer<color>> colorBuffer;
	std::unique_ptr<CBuffer<transform>> transformBuffer;
	VertexShader pVS;
	PixelShader pPS;
};

