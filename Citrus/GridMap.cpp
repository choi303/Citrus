#include "GridMap.h"

static bool mRender = true;
static XMFLOAT3 mapColor;

bool GridMap::init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	//set local device and context to reference device and context
	this->pDevice = pDevice;
	this->pContext = pContext;
	//init shaders
	pVS.Init(L"VSGridMap.cso", pDevice);
	pPS.Init(L"PSGridMap.cso", pDevice);
	//init and create input layout
	const std::vector<D3D11_INPUT_ELEMENT_DESC> grid_ied =
	{
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	//create grid map with vertices
	float b = 1.0f;
	for (int i = 0; i < 250; i++)
	{
		vertices.push_back(vertex{XMFLOAT3(0.0f, 0.0f, b)});
		vertices.push_back(vertex{XMFLOAT3(12451.0f, 0.0f, b)});

		vertices.push_back(vertex{XMFLOAT3(-12451.0f, 0.0f, b)});
		vertices.push_back(vertex{XMFLOAT3(0, 0.0f, b)});

		vertices.push_back(vertex{XMFLOAT3(b, 0.0f, 0.0f)});
		vertices.push_back(vertex{XMFLOAT3(b, 0.0f, 12451.0f)});

		vertices.push_back(vertex{XMFLOAT3(b, 0.0f, -12451.0f)});
		vertices.push_back(vertex{XMFLOAT3(b, 0.0f, 0)});

		b += 50;
	}
	b = 1.0f;
	for (int i = 0; i < 250; i++)
	{
		vertices.push_back(vertex{XMFLOAT3(-12451.0f, 0.0f, b)});
		vertices.push_back(vertex{XMFLOAT3(0, 0.0f, b)});

		vertices.push_back(vertex{XMFLOAT3(0.0f, 0.0f, b)});
		vertices.push_back(vertex{XMFLOAT3(12451.0f, 0.0f, b)});

		vertices.push_back(vertex{XMFLOAT3(b, 0.0f, -12451.0f)});
		vertices.push_back(vertex{XMFLOAT3(b, 0.0f, 0)});

		vertices.push_back(vertex{XMFLOAT3(b, 0.0f, 0.0f)});
		vertices.push_back(vertex{XMFLOAT3(b, 0.0f, 12451.0f)});

		b -= 50;
	}
	pLayout = std::make_unique<InputLayout>(pDevice, grid_ied, &pVS);
	//create vertex buffer
	pVertexBuffer = std::make_unique<VertexBuffer<vertex>>();
	pVertexBuffer->Init(pDevice, vertices.data(), static_cast<UINT>(vertices.size()));
	
	//create color and transform cbuffers
	colorBuffer = std::make_unique<CBuffer<color>>();
	colorBuffer->Init(pDevice, pContext);
	transformBuffer = std::make_unique<CBuffer<transform>>();
	transformBuffer->Init(pDevice, pContext);
	return true;
}

void GridMap::draw(Camera3D cam)
{
	if (mRender)
	{
		//set primitive topology to line list for grid map
		pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		//bind vertex buffer (IA stage)
		pVertexBuffer->Bind(pContext.Get());
		//bind input layout (IA stage)
		pLayout->Bind(pContext.Get());
		//bind shaders (vertex, pixel)
		pVS.Bind(pContext.Get());
		pPS.Bind(pContext.Get());
		//set values of matrices (world, view, proj)
		transformBuffer->data.world = XMMatrixIdentity() * XMMatrixTranslation(0.0f, 0.0f, 0.0f) * XMMatrixScaling(1, 1, 1);
		transformBuffer->data.view = cam.GetViewMatrix();
		transformBuffer->data.projection = cam.GetProjectionMatrix();
		//applying values
		transformBuffer->MapData();
		//bind transform cbuffer
		transformBuffer->VSBind(pContext.Get(), 0u, 1u);
		//set values of color buffer for changing grid map color dynamically
		colorBuffer->data.color = mapColor;
		//apply values
		colorBuffer->MapData();
		colorBuffer->PSBind(pContext.Get(), 0u, 1u);
		//draw grid map (vertices)
		pContext->Draw(static_cast<UINT>(vertices.size()), 0);
	}
}

bool* GridMap::getRender()
{
	//return grid map enabled
	return &mRender;
}

XMFLOAT3* GridMap::getColor()
{
	//return grid map color
	return &mapColor;
}

bool GridMap::setRender(bool value)
{
	//set grid map value to reference value
	return mRender = value;
}
