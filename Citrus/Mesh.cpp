#include "Mesh.h"

Mesh::Mesh(ID3D11Device* p_device, ID3D11DeviceContext* p_context, std::vector<vertex>& vertices, std::vector<signed int>& indices)
{
    //create a vertex buffer
    vb = std::make_unique<VertexBuffer<vertex>>();
    //init the vertex buffer
    HRESULT hr = vb->Init(p_device, vertices.data(), static_cast<UINT>(vertices.size()));
    //bind vertex buffer
    vb->Bind(p_context);
    if (FAILED(hr)) { Error::Log(hr, "Failed to create vertex buffer in mesh class"); }
    //create an index buffer
    ib = std::make_unique<IndexBuffer>();
    //init the index buffer
    hr = ib->Init(p_device, indices, static_cast<UINT>(indices.size()));
    //checking for errors
    if (FAILED(hr)) { Error::Log(hr, "Failed to create index buffer in mesh class"); }
}

Mesh::Mesh(ID3D11Device* p_device, ID3D11DeviceContext* p_context, std::vector<vertex>& vertices, std::vector<signed int>& indices, std::vector<Texture>& textures)
{
    //set local textures vector to reference texture vector
    this->textures = textures;
    //create a vertex buffer
    vb = std::make_unique<VertexBuffer<vertex>>();
    //init the vertex buffer
    HRESULT hr = vb->Init(p_device, vertices.data(), static_cast<UINT>(vertices.size()));
    //bind the vertex buffer
    vb->Bind(p_context);
    if (FAILED(hr)) { Error::Log(hr, "Failed to create vertex buffer in mesh class"); }
    //create an index buffer
    ib = std::make_unique<IndexBuffer>();
    //init the index buffer
    hr = ib->Init(p_device, indices, static_cast<UINT>(indices.size()));
    //checking for errors
    if (FAILED(hr)) { Error::Log(hr, "Failed to create index buffer in mesh class"); }
}

void Mesh::DrawMesh(ID3D11DeviceContext* p_context) const
{
   //Bind every texture in the vector to correct slots
   for (unsigned int i = 0; i < textures.size(); i++)
   {
       textures[i].Bind(p_context);
   }
   //bind the vertex buffer
   vb->Bind(p_context);
   //bind the index buffer
   ib->Bind(p_context);
   //draw mesh
   p_context->DrawIndexed(ib->BufferSize(), 0u, 0u);
}

void Mesh::Destroy() const noexcept
{
    vb->~VertexBuffer();
    ib->~IndexBuffer();
}
