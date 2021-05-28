#include "Mesh.h"

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

Mesh::Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, std::vector<Vertex>& vertices, std::vector<signed int>& indices, std::vector<Texture>& textures)
{
    this->textures = textures;
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
   for (unsigned int i = 0; i < textures.size(); i++)
   {
       textures[i].Bind(pContext);
   }

    vb->Bind(pContext);
    ib->Bind(pContext);
    pContext->DrawIndexed(ib->BufferSize(), 0u, 0u);
}