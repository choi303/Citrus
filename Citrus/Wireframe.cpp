#include "Wireframe.h"

static XMFLOAT3 wireColor = XMFLOAT3(1,1,1);

void Wireframe::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	this->pDevice = pDevice;
	this->pContext = pContext;
	pVS.Init(L"VSWireframe.cso", pDevice);
	pPS.Init(L"PSWireframe.cso", pDevice);
	const std::vector<D3D11_INPUT_ELEMENT_DESC> wireframe_ied =
	{
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	pLayout = std::make_unique<InputLayout>(pDevice, wireframe_ied, &pVS);
	pColorCBuffer = std::make_unique<CBuffer<ColorBuffer>>();
	pColorCBuffer->Init(pDevice, pContext);
}

void Wireframe::Draw()
{
	pLayout->Bind(pContext.Get());
	pVS.Bind(pContext.Get());
	pPS.Bind(pContext.Get());
	pColorCBuffer->data.wireColor = wireColor;
	pColorCBuffer->MapData();
	pColorCBuffer->PSBind(pContext.Get(), 0u, 1u);
}

XMFLOAT3* Wireframe::GetWireColor()
{
	return &wireColor;
}
