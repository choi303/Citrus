#pragma once
#include "CBuffer.h"
#include "Mesh.h"
#include "Camera3D.h"
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include "imgui\imgui.h"
#include "Shaders.h"

using namespace DirectX;

struct Transformation
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX proj;
};

class Model
{
public:
	Model() {}
	bool Init(const std::string& filePath, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	bool InitNoMtl(const std::string& filePath, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Render(Camera3D cam);
	XMFLOAT3 SetPos(float x, float y, float z);
	XMFLOAT3 SetRot(float x, float y, float z);
	XMFLOAT3 SetScale(float x, float y, float z);
	XMFLOAT3 GetPos();
	XMFLOAT3 GetRot();
	XMFLOAT3 GetScale();
	~Model() = default;
private:
	std::vector<Mesh> meshes;
	bool LoadMesh(const std::string& filePath);
	bool LoadMeshNoMtl(const std::string& filePath);
	void LoadNodes(aiNode* pNode, const aiScene* pScene, const aiMaterial* const* pMaterials);
	void LoadNodesNoMtl(aiNode* pNode, const aiScene* pScene);
	Mesh ProcessMeshData(aiMesh* pMesh, const aiScene* pScene, ID3D11DeviceContext* pContext, ID3D11Device* pDevice);
	Mesh ProcessMeshDataNoMtl(aiMesh* pMesh, const aiScene* pScene, ID3D11DeviceContext* pContext, ID3D11Device* pDevice);
	ID3D11Device* pDevice = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	CBuffer<Transformation>* cbVertexshader = nullptr;
private:
	XMFLOAT3 pos = XMFLOAT3{ 0,0,0 };
	XMFLOAT3 rot = XMFLOAT3{ 0,0,0 };
	XMFLOAT3 scale = XMFLOAT3{ 1,1,1 };
	std::unique_ptr<Texture> text;
	std::vector<Texture> textures;
	PixelShader specshader;
	aiString textureName;
	std::string path;
};