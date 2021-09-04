#pragma once
#include "GfxBase.h"

class TessellationPlane
{
private:
	struct TessellationBuffer
	{
		float tessellationAmount;
		float pad[3];
	};

	struct MatrixBuffer
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct vertex
	{
		XMFLOAT3 pos;
		XMFLOAT2 tex;
	};
public:
	TessellationPlane() = default;
	TessellationPlane(TessellationPlane& rhs) = default;
	~TessellationPlane() = default;

	const bool Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) noexcept;
	void Draw(Camera3D cam);
private:
	wrl::ComPtr<ID3D11Device> pDevice;
	wrl::ComPtr<ID3D11DeviceContext> pContext;
	std::unique_ptr<InputLayout> pLayoutVS;
	std::unique_ptr<CBuffer<TessellationBuffer>> tessCBuffer;
	std::unique_ptr<CBuffer<MatrixBuffer>> matCBuffer;
	std::vector<vertex> mVertices;
	std::unique_ptr<VertexBuffer<vertex>> mVertexBuffer;
	HullShader pHS;
	DomainShader pDS;
	VertexShader pVS;
	PixelShader pPS;
};

