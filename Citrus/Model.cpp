#include "Model.h"

bool Model::Init(const std::string& file_path, ID3D11Device* p_device, ID3D11DeviceContext* p_context)
{
    this->pDevice = p_device;
    this->pContext = p_context;
    this->path = file_path;
    textures.push_back(Texture(pDevice, pContext, "Images\\spruit_sunrise_4k.hdr", 3));

    if (!LoadMesh(file_path))
        Error::Log("Failed to load mesh.");

    return true;
}

bool Model::InitNoMtl(const std::string& file_path, ID3D11Device* p_device, ID3D11DeviceContext* p_context)
{
    this->pDevice = p_device;
    this->pContext = p_context;

    if (!LoadMeshNoMtl(file_path))
        Error::Log("Failed to load mesh.");

    return true;
}

void Model::Render(const Camera3D cam)
{
    //bind the constant buffer to pipeline stage
    cb = std::make_unique<CBuffer<transformation>>();
    cb->Init(pDevice, pContext);
    cb->data.world = XMMatrixIdentity() * XMMatrixTranslation(pos.x, pos.y, pos.z) * XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z) *
        XMMatrixScaling(scale.x, scale.y, scale.z);
    cb->data.view = cam.GetViewMatrix();
    cb->data.proj = cam.GetProjectionMatrix();
    cb->MapData();
    cb->VSBind(pContext, 0u, 1u);

    //draw mesh list
    for (int i = 0; i < meshes.size(); i++)
    {
        meshes[i].DrawMesh(pContext);
    }
}

XMFLOAT3 Model::SetPos(const float x, const float y, const float z)
{
    pos.x = x;
    pos.y = y;
    pos.z = z;
    return pos;
}

XMFLOAT3 Model::AdjustPos(float x, float y, float z)
{
    pos.x += x;
    pos.y += y;
    pos.z += z;
    return pos;
}

XMFLOAT3 Model::AdjustRot(float roll, float pitch, float yaw)
{
    rot.x += roll;
    rot.y += pitch;
    rot.z += yaw;
    return rot;
}

XMFLOAT3 Model::SetRot(const float x, const float y, const float z)
{
    rot.x = x;
    rot.y = y;
    rot.z = z;
    return rot;
}

XMFLOAT3 Model::SetScale(const float x, const float y, const float z)
{
    scale.x = x;
    scale.y = y;
    scale.z = z;
    return scale;
}

XMFLOAT3 Model::GetPos() const
{
    return pos;
}

XMFLOAT3 Model::GetRot() const
{
    return rot;
}

XMFLOAT3 Model::GetScale() const
{
    return scale;
}

XMMATRIX Model::GetWorld() const
{
    return cb->data.world;
}

bool Model::GetHasNormal() const
{
    return hasNormalmap;
}

bool Model::GetHasEmessive() const
{
    return hasEmessiveMap;
}

bool Model::GetHasTexture() const
{
    return hasTexture;
}

bool Model::GetHasParallaxMap() const
{
    return hasParallaxMap;
}

const std::string& Model::GetPath() const
{
    return path;
}

void Model::Destroy() const
{
    for (int i = 0; i < meshes.size(); i++)
    {
        meshes[i].Destroy();
    }
}

bool Model::LoadMesh(const std::string& file_path)
{
    Assimp::Importer importer;

    pScene = importer.ReadFile(file_path, //read file from file
        aiProcess_Triangulate |
        aiProcess_ConvertToLeftHanded |
        aiProcess_JoinIdenticalVertices |
        aiProcess_CalcTangentSpace |
        aiProcess_GenNormals |
        aiProcess_FixInfacingNormals |
        aiProcess_PreTransformVertices |
        aiProcess_OptimizeMeshes 
    );   //adding some flags for optimize

    if (pScene == nullptr)
        return false;

    LoadNodes(pScene->mRootNode, pScene, pScene->mMaterials); //load mesh

    return true;
}

bool Model::LoadMeshNoMtl(const std::string& file_path)
{
    Assimp::Importer importer;

    const aiScene* p_scene = importer.ReadFile(file_path, //read file from file
        aiProcess_Triangulate |
        aiProcess_ConvertToLeftHanded |
        aiProcess_JoinIdenticalVertices |
        aiProcess_FixInfacingNormals |
        aiProcess_PreTransformVertices |
        aiProcess_OptimizeMeshes 
    );   //adding some flags for optimize

    if (p_scene == nullptr)
        return false;

    LoadNodesNoMtl(p_scene->mRootNode, p_scene); //load mesh

    return true;
}

void Model::LoadNodes(aiNode* p_node, const aiScene* p_scene, const aiMaterial* const* p_materials)
{
    const std::string textureDirectory = path.substr(0, path.find_last_of("\\") + 1);

    for (size_t i = 0; i < p_node->mNumMeshes; i++)
    {
        //process every mesh in the vector array
        pMesh = p_scene->mMeshes[p_node->mMeshes[i]];
        const aiMaterial& mtl = *p_materials[pMesh->mMaterialIndex];

        if (pMesh->mMaterialIndex >= 0) //if model has a material file load model's materials
        {
            //get textures from the model
            //init that textures
            if (mtl.GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &textureName) == aiReturn_SUCCESS)
            {
                textures.push_back(Texture(pDevice, pContext, textureDirectory + textureName.C_Str()));
                hasTexture = true;
            }
            if (mtl.GetTexture(aiTextureType::aiTextureType_SPECULAR, 0, &textureName) == aiReturn_SUCCESS)
            {
                textures.push_back(Texture(pDevice, pContext, textureDirectory + textureName.C_Str(), 1));
            }
            else
            {
                float shininess = 45.0f;
                mtl.Get(AI_MATKEY_SHININESS, shininess);
                if(hasTexture)
                textures.push_back(Texture(pDevice, pContext, textureDirectory + textureName.C_Str(), 1));
            }

            if (mtl.GetTexture(aiTextureType::aiTextureType_NORMALS, 0, &textureName) == aiReturn_SUCCESS)
            {
                textures.push_back(Texture(pDevice, pContext, textureDirectory + textureName.C_Str(), 2));
                hasNormalmap = true;
            }

            if (mtl.GetTexture(aiTextureType::aiTextureType_REFLECTION, 0, &textureName) == aiReturn_SUCCESS)
            {
                textures.push_back(Texture(pDevice, pContext, textureDirectory + textureName.C_Str(), 3));
            }

            if (mtl.GetTexture(aiTextureType::aiTextureType_EMISSIVE, 0, &textureName) == aiReturn_SUCCESS)
            {
                textures.push_back(Texture(pDevice,
                    pContext, textureDirectory + textureName.C_Str(), 5));
                hasEmessiveMap = true;
            }

            if (mtl.GetTexture(aiTextureType::aiTextureType_DISPLACEMENT, 0, &textureName) == aiReturn_SUCCESS)
            {
                textures.push_back(Texture(pDevice, pContext,
                    textureDirectory + textureName.C_Str(), 6));
                hasParallaxMap = true;
            }
        }
        
        meshes.push_back(ProcessMeshData(pMesh, p_scene));
    }

    for (size_t i = 0; i < p_node->mNumChildren; i++)
    {
        LoadNodes(p_node->mChildren[i], p_scene, p_scene->mMaterials);
    }
}

void Model::LoadNodesNoMtl(aiNode* p_node, const aiScene* p_scene)
{
    for (size_t i = 0; i < p_node->mNumMeshes; i++)
    {
        //process every mesh in the vector array
        aiMesh* mesh = p_scene->mMeshes[p_node->mMeshes[i]];
        meshes.push_back(ProcessMeshDataNoMtl(mesh, p_scene));
    }

    for (size_t i = 0; i < p_node->mNumChildren; i++)
    {
        LoadNodesNoMtl(p_node->mChildren[i], p_scene);
    }
}

Mesh Model::ProcessMeshData(aiMesh* p_mesh, const aiScene* p_scene) 
{
    std::vector<vertex> vertices;
    std::vector<signed int> indices;

   //Get vertices
   for (unsigned int i = 0; i < p_mesh->mNumVertices; i++)
   {
       vertex vertex;

       //loading model vertices
       vertex.pos.x = p_mesh->mVertices[i].x;
       vertex.pos.y = p_mesh->mVertices[i].y;
       vertex.pos.z = p_mesh->mVertices[i].z;

       //also get normals from model for light calculates and another lots of things
       vertex.n.x = p_mesh->mNormals[i].x;
       vertex.n.y = p_mesh->mNormals[i].y;
       vertex.n.z = p_mesh->mNormals[i].z;

       //determine texture coordinates
       if (p_mesh->mTextureCoords[0])
       {
           vertex.tex.x = static_cast<float>(p_mesh->mTextureCoords[0][i].x);
           vertex.tex.y = static_cast<float>(p_mesh->mTextureCoords[0][i].y);
       }

   	   //loading tangent data of model
       if (hasNormalmap)
       {
           vertex.tangent.x = p_mesh->mTangents[i].x;
           vertex.tangent.y = p_mesh->mTangents[i].y;
           vertex.tangent.z = p_mesh->mTangents[i].z;

           vertex.binormal.x = p_mesh->mBitangents[i].x;
           vertex.binormal.y = p_mesh->mBitangents[i].y;
           vertex.binormal.z = p_mesh->mBitangents[i].z;
       }

       vertices.push_back(vertex); //add the vertices data to vertices vector
   }

   //Get indices
   for (unsigned int i = 0; i < p_mesh->mNumFaces; i++)
   {
       const aiFace face = p_mesh->mFaces[i]; //get face count from mesh

       for (unsigned int j = 0; j < face.mNumIndices; j++)
           indices.push_back(face.mIndices[j]);    //add the indices data to indices vector
   }

   return Mesh(pDevice, pContext, vertices, indices, textures);  //bind data to index and vertex buffers
    
}

Mesh Model::ProcessMeshDataNoMtl(aiMesh* p_mesh, const aiScene* p_scene) const
{
    // Data to fill
    std::vector<vertex> verticesL;
    std::vector<signed int> indicesL;

    //Get vertices
    for (unsigned int i = 0; i < p_mesh->mNumVertices; i++)
    {
        vertex vertex;

        vertex.pos.x = p_mesh->mVertices[i].x;
        vertex.pos.y = p_mesh->mVertices[i].y;
        vertex.pos.z = p_mesh->mVertices[i].z;

        //also get normals from model for light calculates and another lots of things
        vertex.n.x = p_mesh->mNormals[i].x;
        vertex.n.y = p_mesh->mNormals[i].y;
        vertex.n.z = p_mesh->mNormals[i].z;

        //determine texture coordinates
        if (p_mesh->mTextureCoords[0])
        {
            vertex.tex.x = static_cast<float>(p_mesh->mTextureCoords[0][i].x);
            vertex.tex.y = static_cast<float>(p_mesh->mTextureCoords[0][i].y);
        }

        verticesL.push_back(vertex); //add the vertices data to vertices vector
    }

    //Get indices
    for (unsigned int i = 0; i < p_mesh->mNumFaces; i++)
    {
        const aiFace face = p_mesh->mFaces[i]; //get face count from mesh

        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indicesL.push_back(face.mIndices[j]);    //add the indices data to indices vector
    }

    return Mesh(pDevice, pContext, verticesL, indicesL);  //bind data to index and vertex buffers
}