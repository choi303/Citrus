#include "Mesh.h"

aiString textureFileName;
 Texture tex;

Mesh::Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, std::vector<Vertex>& vertices, std::vector<signed int>& indices)
{
    vb = std::make_unique<VertexBuffer<Vertex>>();
    HRESULT hr = vb->Init(pDevice, vertices.data(), (UINT)vertices.size());
    vb->Bind(pContext);
    if (FAILED(hr)) { Error::Log(hr, "Failed to create vertex buffer in mesh class"); }
    ib = std::make_unique<IndexBuffer>();
    hr = ib->Init(pDevice, indices, (UINT)indices.size());
    if (FAILED(hr)) { Error::Log(hr, "Failed to create index buffer in mesh class"); }
}

void Mesh::DrawMesh(ID3D11DeviceContext* pContext) const
{
    vb->Bind(pContext);
    ib->Bind(pContext);
    pContext->DrawIndexed(ib->BufferSize(), 0u, 0u);
}

Mesh Mesh::ProcessMeshData(aiMesh* pMesh, const aiScene* pScene, ID3D11DeviceContext* pContext, ID3D11Device* pDevice,
    const aiMaterial* const* pMaterials)
{
    // Data to fill
    std::vector<Vertex> vertices;
    std::vector<signed int> indices;

    if (pMesh->mMaterialIndex >= 0)
    {
        using namespace std::string_literals;
        const aiMaterial& mtl = *pMaterials[pMesh->mMaterialIndex];
        mtl.GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &textureFileName);
        std::string a = textureFileName.C_Str();
        a += "\n";
        OutputDebugStringA(a.c_str());
        tex.Init(pDevice, WICTexture::FromFile("Models\\nano_textured\\"s + textureFileName.C_Str()));
        tex.Bind(pContext);
    }

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
        aiFace face = pMesh->mFaces[i];

        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);    //add the indices data to indices vector
    }

    return Mesh(pDevice, pContext, vertices, indices);  //bind data to index and vertex buffers
}

aiString Mesh::GetTexturePath()
{
    return textureFileName;
}

Texture Mesh::GetTex()
{
    return tex;
}
