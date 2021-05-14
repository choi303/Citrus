#include "Model.h"

bool Model::Init(const std::string& filePath, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    this->pDevice = pDevice;
    this->pContext = pContext;

    if (!LoadMesh(filePath))
        Error::Log("Failed to load mesh in model.cpp");

    return true;
}

bool Model::InitWithMtl(const std::string& filePath, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    this->pDevice = pDevice;
    this->pContext = pContext;

    if (!LoadMeshWithMtl(filePath))
        Error::Log("Failed to load mesh in model.cpp");

    return true;
}

void Model::Render(Camera3D cam)
{
    //bind the constant buffer to pipline stage 
    std::unique_ptr<CBuffer<Transformation>> cb = std::make_unique<CBuffer<Transformation>>();
    cb->Init(pDevice, pContext);
    cb->data.world = XMMatrixIdentity() * XMMatrixTranslation(pos.x, pos.y, pos.z) * XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z) *
        XMMatrixScaling(scale.x, scale.y, scale.z);
    cb->data.view = cam.GetViewMatrix();
    cb->data.proj = cam.GetProjectionMatrix();
    cb->MapData();
    cb->VSBind(pContext, 0u, 1u);

    //draw mesh lise
    for (int i = 0; i < meshes.size(); i++)
    {
        meshes[i].DrawMesh(pContext);
    }
}

XMFLOAT3 Model::SetPos(float x, float y, float z)
{
    pos.x = x;
    pos.y = y;
    pos.z = z;
    return pos;
}

XMFLOAT3 Model::SetRot(float x, float y, float z)
{
    rot.x = x;
    rot.y = y;
    rot.z = z;
    return rot;
}

XMFLOAT3 Model::SetScale(float x, float y, float z)
{
    scale.x = x;
    scale.y = y;
    scale.z = z;
    return scale;
}

XMFLOAT3 Model::GetPos()
{
    return pos;
}

XMFLOAT3 Model::GetRot()
{
    return rot;
}

XMFLOAT3 Model::GetScale()
{
    return scale;
}

aiString Model::GetTexturePath()
{
    return textureName;
}

Texture Model::GetTex()
{
    return text;
}

bool Model::LoadMesh(const std::string& filePath)
{
    Assimp::Importer importer;

    const aiScene* pScene = importer.ReadFile(filePath, //read file from file
        aiProcess_Triangulate |
        aiProcess_ConvertToLeftHanded |
    aiProcess_JoinIdenticalVertices);   //adding sime flags for optimize

    if (pScene == nullptr)
        return false;

    LoadNodes(pScene->mRootNode, pScene); //load mesh

    return true;
}

bool Model::LoadMeshWithMtl(const std::string& filePath)
{
    Assimp::Importer importer;

    const aiScene* pScene = importer.ReadFile(filePath, //read file from file
        aiProcess_Triangulate |
        aiProcess_ConvertToLeftHanded |
        aiProcess_JoinIdenticalVertices);   //adding sime flags for optimize

    if (pScene == nullptr)
        return false;

    LoadNodes(pScene->mRootNode, pScene, pScene->mMaterials); //load mesh

    return true;
}

void Model::LoadNodes(aiNode* pNode, const aiScene* pScene, const aiMaterial* const* pMaterials)
{
    for (size_t i = 0; i < pNode->mNumMeshes; i++)
    {
        //process every mesh in the vector array
        aiMesh* mesh = pScene->mMeshes[pNode->mMeshes[i]];
        if (mesh->mMaterialIndex >= 0) //if model has a material file load model's materials
        {
            using namespace std::string_literals;
            const aiMaterial& mtl = *pMaterials[mesh->mMaterialIndex];
            //get only diffuse texture's names of model
            mtl.GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &textureName);
            std::string a = textureName.C_Str();
            a += "\n";
            OutputDebugStringA(a.c_str());
            //init that textures
            text.Init(pDevice, WICTexture::FromFile("Models\\nano_textured\\"s + textureName.C_Str()));
            text.Bind(pContext);
        }

        meshes.push_back(Mesh::ProcessMeshData(mesh, pScene, pContext, pDevice));
    }

    for (size_t i = 0; i < pNode->mNumChildren; i++)
    {
        LoadNodes(pNode->mChildren[i], pScene, pScene->mMaterials);
    }
}

void Model::LoadNodes(aiNode* pNode, const aiScene* pScene)
{
    for (size_t i = 0; i < pNode->mNumMeshes; i++)
    {
        //process every mesh in the vector array
        aiMesh* mesh = pScene->mMeshes[pNode->mMeshes[i]];
        meshes.push_back(Mesh::ProcessMeshData(mesh, pScene, pContext, pDevice));
    }

    for (size_t i = 0; i < pNode->mNumChildren; i++)
    {
        LoadNodes(pNode->mChildren[i], pScene);
    }
}