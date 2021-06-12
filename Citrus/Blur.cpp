#include "Blur.h"

void Blur::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const int width, const int height)
{
	this->pDevice = pDevice;
	this->pContext = pContext;
	this->width = width;
	this->height = height;
	pVS.Init(L"VSBlur.cso", pDevice);
	pPS.Init(L"PSBlur.cso", pDevice);
	const std::vector<D3D11_INPUT_ELEMENT_DESC> blur_ied =
	{
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Texcoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	pLayout = std::make_unique<InputLayout>(pDevice, blur_ied, &pVS);

	pBlurCBuffer = std::make_unique<CBuffer<ScreenSizeBuffer>>();
	pBlurCBuffer->Init(pDevice, pContext);
}

void Blur::Draw()
{
	pLayout->Bind(pContext.Get());
	pVS.Bind(pContext.Get());
	pPS.Bind(pContext.Get());
	pBlurCBuffer->data.screenWidth = width;
	pBlurCBuffer->data.screenHeight = height;
	pBlurCBuffer->MapData();
	pBlurCBuffer->VSBind(pContext.Get(), 1u, 1u);
}
