#pragma once
#include "CBuffer.h"
#include "Mesh.h"
#include "Camera3D.h"
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include "imgui\imgui.h"

using namespace DirectX;

struct transformation
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX proj;
};

class Model
{
public:
	Model() {}
	bool Init(const std::string& file_path, ID3D11Device* p_device, ID3D11DeviceContext* p_context);
	bool InitNoMtl(const std::string& file_path, ID3D11Device* p_device, ID3D11DeviceContext* p_context);
	bool InitPbr(const std::string& file_path, ID3D11Device* p_device, ID3D11DeviceContext* p_context);
	void Render(const Camera3D cam);
	XMFLOAT3 SetPos(const float x, const float y, const float z);
	XMFLOAT3 AdjustPos(float x, float y, float z);
	XMFLOAT3 AdjustRot(float roll, float pitch, float yaw);
	XMFLOAT3 SetRot(const float x, const float y, const float z);
	XMFLOAT3 SetScale(const float x, const float y, const float z);
	XMFLOAT3 GetPos() const;
	XMFLOAT3 GetRot() const;
	XMFLOAT3 GetScale() const;
	XMMATRIX GetWorld() const;
	bool GetHasNormal() const;
	bool GetHasEmessive() const;
	bool GetHasTexture() const;
	bool GetHasParallaxMap() const;
	const std::string& GetPath() const;
	void Destroy() const;
	std::vector<Texture> textures;
	~Model() = default;
private:
	std::vector<Mesh> meshes;
	bool LoadMesh(const std::string& file_path);
	bool LoadMeshNoMtl(const std::string& file_path);
	bool LoadMeshPbr(const std::string& file_path);
	void LoadNodes(aiNode* p_node, const aiScene* p_scene, const aiMaterial* const* p_materials);
	void LoadNodesNoMtl(aiNode* p_node, const aiScene* p_scene);
	void LoadNodesPbr(aiNode* p_node, const aiScene* p_scene);
	Mesh ProcessMeshData(aiMesh* p_mesh, const aiScene* p_scene);
	Mesh ProcessMeshDataNoMtl(aiMesh* p_mesh, const aiScene* p_scene) const;
	Mesh ProcessMeshDataPbr(aiMesh* p_mesh, const aiScene* p_scene) const;
	ID3D11Device* pDevice = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	CBuffer<transformation>* cbVertexshader = nullptr;
	aiMesh* pMesh;
	const aiScene* pScene;
private:
	XMFLOAT3 pos = XMFLOAT3{ 0,0,0 };
	XMFLOAT3 rot = XMFLOAT3{ 0,0,0 };
	XMFLOAT3 scale = XMFLOAT3{ 1,1,1 };
	std::unique_ptr<CBuffer<transformation>> cb;
	aiString textureName;
	std::string path;
	bool hasNormalmap = false;
	bool hasEmessiveMap = false;
	bool hasTexture = false;
	bool hasParallaxMap = false;
};