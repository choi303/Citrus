#include "Blur.h"

void Blur::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const int width, const int height)
{
	//set local device and context to reference device and context
	this->pDevice = pDevice;
	this->pContext = pContext;
	//set local width and height to reference width and height
	this->width = width;
	this->height = height;
	//init shaders
	pVS.Init(L"VSBlur.cso", pDevice);
	pPS.Init(L"PSBlur.cso", pDevice);
	//init and create input layout
	const std::vector<D3D11_INPUT_ELEMENT_DESC> blur_ied =
	{
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Texcoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	pLayout = std::make_unique<InputLayout>(pDevice, blur_ied, &pVS);

	//create constant buffer for screen size
	pBlurCBuffer = std::make_unique<CBuffer<ScreenSizeBuffer>>();
	pBlurCBuffer->Init(pDevice, pContext);
}

void Blur::Draw()
{
	//bind input layout
	pLayout->Bind(pContext.Get());
	//bind shaders
	pVS.Bind(pContext.Get());
	pPS.Bind(pContext.Get());
	//bind c buffer
	pBlurCBuffer->data.screenWidth = width;
	pBlurCBuffer->data.screenHeight = height;
	pBlurCBuffer->MapData();
	pBlurCBuffer->VSBind(pContext.Get(), 1u, 1u);
}
