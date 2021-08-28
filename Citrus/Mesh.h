#pragma once
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include <DirectXMath.h>
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include "Texture.h"

struct vertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 tex;
	DirectX::XMFLOAT3 n;
	DirectX::XMFLOAT3 tangent;
	DirectX::XMFLOAT3 binormal;
};

class Mesh
{
public:
	Mesh(ID3D11Device* p_device, ID3D11DeviceContext* p_context, std::vector<vertex>& vertices, std::vector<signed int>& indices);
	Mesh(ID3D11Device* p_device, ID3D11DeviceContext* p_context, std::vector<vertex>& vertices, std::vector<signed int>& indices,
		std::vector<Texture>& textures);
	void DrawMesh(ID3D11DeviceContext* p_context) const;
	void Destroy() const noexcept;
private:
	std::unique_ptr<VertexBuffer<vertex>> vb;
	std::unique_ptr<IndexBuffer> ib;
	mutable std::vector<Texture> textures;
};

