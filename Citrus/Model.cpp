#include "Model.h"

bool Model::Init(const std::string& filePath, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    this->pDevice = pDevice;
    this->pContext = pContext;
    this->path = filePath.c_str();

    if (!LoadMesh(filePath))
        Error::Log("Failed to load mesh.");

    return true;
}

bool Model::InitNoMtl(const std::string& filePath, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    this->pDevice = pDevice;
    this->pContext = pContext;

    if (!LoadMeshNoMtl(filePath))
        Error::Log("Failed to load mesh.");

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
    specshader.Bind(pContext);

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

bool Model::LoadMesh(const std::string& filePath)
{
    Assimp::Importer importer;

    const aiScene* pScene = importer.ReadFile(filePath, //read file from file
        aiProcess_Triangulate |
        aiProcess_ConvertToLeftHanded |
        aiProcess_JoinIdenticalVertices |
        aiProcess_FixInfacingNormals |
        aiProcess_PreTransformVertices
    );   //adding sime flags for optimize

    if (pScene == nullptr)
        return false;

    LoadNodes(pScene->mRootNode, pScene, pScene->mMaterials); //load mesh

    return true;
}

bool Model::LoadMeshNoMtl(const std::string& filePath)
{
    Assimp::Importer importer;

    const aiScene* pScene = importer.ReadFile(filePath, //read file from file
        aiProcess_Triangulate |
        aiProcess_ConvertToLeftHanded |
        aiProcess_JoinIdenticalVertices |
        aiProcess_FixInfacingNormals |
        aiProcess_PreTransformVertices
    );   //adding sime flags for optimize

    if (pScene == nullptr)
        return false;

    LoadNodesNoMtl(pScene->mRootNode, pScene); //load mesh

    return true;
}

void Model::LoadNodes(aiNode* pNode, const aiScene* pScene, const aiMaterial* const* pMaterials)
{
    std::string directory = path.substr(0 ,path.find_last_of("\\") + 1);
    
    
    for (size_t i = 0; i < pNode->mNumMeshes; i++)
    {
        //process every mesh in the vector array
        aiMesh* mesh = pScene->mMeshes[pNode->mMeshes[i]];
        const aiMaterial& mtl = *pMaterials[mesh->mMaterialIndex];
        using namespace std::string_literals;

        if (mesh->mMaterialIndex >= 0) //if model has a material file load model's materials
        {
            //get textures from the model
            //init that textures
            mtl.GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &textureName);
            textures.push_back(Texture(pDevice, pContext, directory + textureName.C_Str()));
            if (mtl.GetTexture(aiTextureType::aiTextureType_SPECULAR, 0, &textureName) == aiReturn_SUCCESS)
            {
                specshader.Init(L"PhongPSSpec.cso", pDevice);
                specshader.Bind(pContext);
                textures.push_back(Texture(pDevice, pContext, directory + textureName.C_Str(), 1));
            }
            else
            {
                float shiniess = 35.0f;
                mtl.Get(AI_MATKEY_SHININESS, shiniess);
                textures.push_back(Texture(pDevice, pContext, directory + textureName.C_Str(), 1));
            }
        }

        meshes.push_back(ProcessMeshData(mesh, pScene, pContext, pDevice));
    }

    for (size_t i = 0; i < pNode->mNumChildren; i++)
    {
        LoadNodes(pNode->mChildren[i], pScene, pScene->mMaterials);
    }
}

void Model::LoadNodesNoMtl(aiNode* pNode, const aiScene* pScene)
{
    for (size_t i = 0; i < pNode->mNumMeshes; i++)
    {
        //process every mesh in the vector array
        aiMesh* mesh = pScene->mMeshes[pNode->mMeshes[i]];
        meshes.push_back(ProcessMeshDataNoMtl(mesh, pScene, pContext, pDevice));
    }

    for (size_t i = 0; i < pNode->mNumChildren; i++)
    {
        LoadNodesNoMtl(pNode->mChildren[i], pScene);
    }
}

Mesh Model::ProcessMeshData(aiMesh* pMesh, const aiScene* pScene, ID3D11DeviceContext* pContext, ID3D11Device* pDevice)
{
   // Data to fill
   std::vector<Vertex> vertices;
   std::vector<signed int> indices;

   //Get vertices
   for (unsigned int i = 0; i < pMesh->mNumVertices; i++)
   {
       Vertex vertex;

       vertex.pos.x = pMesh->mVertices[i].x;
       vertex.pos.y = pMesh->mVertices[i].y;
       vertex.pos.z = pMesh->mVertices[i].z;

       //also get normals from model for light calculates and another lots of things
       vertex.n.x = pMesh->mNormals[i].x;
       vertex.n.y = pMesh->mNormals[i].y;
       vertex.n.z = pMesh->mNormals[i].z;

       //determine textur coordinates
       if (pMesh->mTextureCoords[0])
       {
           vertex.tex.x = (float)pMesh->mTextureCoords[0][i].x;
           vertex.tex.y = (float)pMesh->mTextureCoords[0][i].y;
       }

       vertices.push_back(vertex); //add the vertices data to vertices vector
   }

   //Get indices
   for (unsigned int i = 0; i < pMesh->mNumFaces; i++)
   {
       aiFace face = pMesh->mFaces[i]; //get face count from mesh

       for (unsigned int j = 0; j < face.mNumIndices; j++)
           indices.push_back(face.mIndices[j]);    //add the indices data to indices vector
   }

   return Mesh(pDevice, pContext, vertices, indices, textures);  //bind data to index and vertex buffers
    
}

Mesh Model::ProcessMeshDataNoMtl(aiMesh* pMesh, const aiScene* pScene, ID3D11DeviceContext* pContext, ID3D11Device* pDevice)
{
    // Data to fill
    std::vector<Vertex> vertices;
    std::vector<signed int> indices;

    //Get vertices
    for (unsigned int i = 0; i < pMesh->mNumVertices; i++)
    {
        Vertex vertex;

        vertex.pos.x = pMesh->mVertices[i].x;
        vertex.pos.y = pMesh->mVertices[i].y;
        vertex.pos.z = pMesh->mVertices[i].z;

        //also get normals from model for light calculates and another lots of things
        vertex.n.x = pMesh->mNormals[i].x;
        vertex.n.y = pMesh->mNormals[i].y;
        vertex.n.z = pMesh->mNormals[i].z;

        //determine textur coordinates
        if (pMesh->mTextureCoords[0])
        {
            vertex.tex.x = (float)pMesh->mTextureCoords[0][i].x;
            vertex.tex.y = (float)pMesh->mTextureCoords[0][i].y;
        }

        vertices.push_back(vertex); //add the vertices data to vertices vector
    }

    //Get indices
    for (unsigned int i = 0; i < pMesh->mNumFaces; i++)
    {
        aiFace face = pMesh->mFaces[i]; //get face count from mesh

        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);    //add the indices data to indices vector
    }

    return Mesh(pDevice, pContext, vertices, indices);  //bind data to index and vertex buffers
}