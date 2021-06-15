#include "Wireframe.h"

//create wireColor static variable for imgui window
static XMFLOAT3 wireColor = XMFLOAT3(1,1,1);

void Wireframe::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	//Set local device and context to reference device and context
	this->pDevice = pDevice;
	this->pContext = pContext;
	//Init shaders
	pVS.Init(L"VSWireframe.cso", pDevice);
	pPS.Init(L"PSWireframe.cso", pDevice);
	//Init and create input layout
	const std::vector<D3D11_INPUT_ELEMENT_DESC> wireframe_ied =
	{
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	pLayout = std::make_unique<InputLayout>(pDevice, wireframe_ied, &pVS);
	//Create color constant buffer
	pColorCBuffer = std::make_unique<CBuffer<ColorBuffer>>();
	pColorCBuffer->Init(pDevice, pContext);
}

void Wireframe::Draw()
{
	//bind layout
	pLayout->Bind(pContext.Get());
	//bind shaders
	pVS.Bind(pContext.Get());
	pPS.Bind(pContext.Get());
	//bind color buffer value and apply that value
	pColorCBuffer->data.wireColor = wireColor;
	pColorCBuffer->MapData();
	//constant buffer bind
	pColorCBuffer->PSBind(pContext.Get(), 0u, 1u);
}

XMFLOAT3* Wireframe::GetWireColor()
{
	//return wire color
	return &wireColor;
}
