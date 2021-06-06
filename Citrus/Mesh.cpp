#include "Mesh.h"

Mesh::Mesh(ID3D11Device* p_device, ID3D11DeviceContext* p_context, std::vector<vertex>& vertices, std::vector<signed int>& indices)
{
    vb = std::make_unique<VertexBuffer<vertex>>();
    HRESULT hr = vb->Init(p_device, vertices.data(), static_cast<UINT>(vertices.size()));
    vb->Bind(p_context);
    if (FAILED(hr)) { Error::Log(hr, "Failed to create vertex buffer in mesh class"); }
    ib = std::make_unique<IndexBuffer>();
    hr = ib->Init(p_device, indices, static_cast<UINT>(indices.size()));
    if (FAILED(hr)) { Error::Log(hr, "Failed to create index buffer in mesh class"); }
}

Mesh::Mesh(ID3D11Device* p_device, ID3D11DeviceContext* p_context, std::vector<vertex>& vertices, std::vector<signed int>& indices, std::vector<Texture>& textures)
{
    this->textures = textures;
    vb = std::make_unique<VertexBuffer<vertex>>();
    HRESULT hr = vb->Init(p_device, vertices.data(), static_cast<UINT>(vertices.size()));
    vb->Bind(p_context);
    if (FAILED(hr)) { Error::Log(hr, "Failed to create vertex buffer in mesh class"); }
    ib = std::make_unique<IndexBuffer>();
    hr = ib->Init(p_device, indices, static_cast<UINT>(indices.size()));
    if (FAILED(hr)) { Error::Log(hr, "Failed to create index buffer in mesh class"); }
}

void Mesh::DrawMesh(ID3D11DeviceContext* p_context) const
{
   for (unsigned int i = 0; i < textures.size(); i++)
   {
       textures[i].Bind(p_context);
   }

    vb->Bind(p_context);
    ib->Bind(p_context);
    p_context->DrawIndexed(ib->BufferSize(), 0u, 0u);
}