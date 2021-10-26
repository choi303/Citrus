#include "TessellationPlane.h"

static float amount = 1.0f;
const bool TessellationPlane::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) noexcept
{
	this->pDevice = pDevice;
	this->pContext = pContext;
    //Initialize Shaders
    pHS.Init(L"HSTessellation.cso", pDevice);
    pDS.Init(L"DSTessellation.cso", pDevice);
    pVS.Init(L"TextureVS.cso", pDevice);
    pPS.Init(L"TexturePS.cso", pDevice);

    //Create Input Layout
	const std::vector<D3D11_INPUT_ELEMENT_DESC> tess_ied =
	{
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Color", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	pLayoutVS = std::make_unique<InputLayout>(
		pDevice, tess_ied, &pVS);

	//Constant Buffer Initialize
	tessCBuffer = std::make_unique<CBuffer<TessellationBuffer>>();
	tessCBuffer->Init(pDevice, pContext);
	matCBuffer = std::make_unique<CBuffer<MatrixBuffer>>();
	matCBuffer->Init(pDevice, pContext);

	//Vertex Buffer Initialize
	mVertices.push_back(vertex{XMFLOAT3(-1.0, -1.0, 0.0), XMFLOAT2(0.0, 0.0)});
	mVertices.push_back(vertex{XMFLOAT3(0.0, 1.0, 0.0), XMFLOAT2(1.0, 0.0)});
	mVertices.push_back(vertex{XMFLOAT3(1.0, -1.0, 0.0), XMFLOAT2(0.0, 1.0)});
	mVertexBuffer = std::make_unique<VertexBuffer<vertex>>();
	mVertexBuffer->Init(pDevice, mVertices.data(), 6);

    return true;
}

void TessellationPlane::Draw(Camera3D cam)
{
	//Set vertex buffer
	mVertexBuffer->Bind(pContext.Get());
	pLayoutVS->Bind(pContext.Get());

	//Set Vertex Shader
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	pVS.Bind(pContext.Get());

	//Set Hull Shader
	pHS.Bind(pContext.Get());

	//Set Domain Shader
	pDS.Bind(pContext.Get());

	//Set Pixel Shader
	pPS.Bind(pContext.Get());

	matCBuffer->data.world = XMMatrixIdentity() * XMMatrixTranslation(0.0f, 5.0f, 0.0f) *
		XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f) * XMMatrixScaling(1.0f, 1.0f, 1.0f);
	matCBuffer->data.view = cam.GetViewMatrix();
	matCBuffer->data.projection = cam.GetProjectionMatrix();
	matCBuffer->MapData();
	matCBuffer->DSBind(pContext.Get(), 0, 1);

	tessCBuffer->data.tessellationAmount = amount;
	tessCBuffer->MapData();
	tessCBuffer->HSBind(pContext.Get(), 1, 1);

	UI::TessellationTri(&amount);

	pContext->Draw(static_cast<UINT>(mVertices.size()), 0);

	pContext->HSSetShader(nullptr, nullptr, 0);
	pContext->DSSetShader(nullptr, nullptr, 0);
}
