#include "Fire.h"

float posX=0.0f, posY=0.0f, posZ=0.0f, scaleX=1.0f, scaleY=1.0f, scaleZ=1.0f;
float sSpeedX=1.3f, sSpeedY=2.1f, sSpeedZ=2.3f;
float scalesX=1.0f, scalesY=2.0f, scalesZ=3.0f;
float distortion1X=0.1f, distortion1Y=0.2f;
float distortion2X=0.1f, distortion2Y=0.3f;
float distortion3X = 0.1f, distortion3Y = 0.1f;
float distortionScale=0.8f, distortionBias=0.5f;

Fire::Fire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:
	mDevice(pDevice),
	mContext(pContext)
{
	//init shaders
	mVS.Init(L"VSFire.cso", mDevice.Get());
	mPS.Init(L"PSFire.cso", mDevice.Get());
	//init and create input layout
	const std::vector<D3D11_INPUT_ELEMENT_DESC> fire_ied
		=
	{
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Texcoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	mLayout = std::make_unique<InputLayout>(
		mDevice.Get(), fire_ied, &mVS);

	//init constant buffer
	mMatricesBuffer = std::make_unique<CBuffer<matricesBuffer>>();
	mMatricesBuffer->Init(mDevice.Get(), mContext.Get());
	mNoiseBuffer = std::make_unique<CBuffer<noiseBuffer>>();
	mNoiseBuffer->Init(mDevice.Get(), mContext.Get());
	mDistortionBuffer = std::make_unique<CBuffer<distortionBuffer>>();
	mDistortionBuffer->Init(mDevice.Get(), mContext.Get());

	//texture initalize
	mTexture = std::make_unique<Texture>(mDevice.Get(), mContext.Get(), "Images\\fire01.dds", 0);
	mNoise = std::make_unique<Texture>(mDevice.Get(), mContext.Get(), "Images\\noise01.dds", 1);
	mAlpha = std::make_unique<Texture>(mDevice.Get(), mContext.Get(), "Images\\alpha01.dds", 2);

	//vertex buffer initalize
	mVertices.push_back(vertex(XMFLOAT3(-1.0, 1.0, 0.0), XMFLOAT2(0.0, 0.0)));
	mVertices.push_back(vertex(XMFLOAT3(1.0, 1.0, 0.0), XMFLOAT2(1.0, 0.0)));
	mVertices.push_back(vertex(XMFLOAT3(-1.0, -1.0, 0.0), XMFLOAT2(0.0, 1.0)));
	mVertices.push_back(vertex(XMFLOAT3(-1.0, -1.0, 0.0), XMFLOAT2(0.0, 1.0)));
	mVertices.push_back(vertex(XMFLOAT3(1.0, 1.0, 0.0), XMFLOAT2(1.0, 0.0)));
	mVertices.push_back(vertex(XMFLOAT3(1.0, -1.0, 0.0), XMFLOAT2(1.0, 1.0)));
	mVertexBuffer = std::make_unique<VertexBuffer<vertex>>();
	mVertexBuffer->Init(mDevice.Get(), mVertices.data(), 6);

	//create an alpha enabled blend state description.
	D3D11_BLEND_DESC blendStateDescription = {};
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;
	HRESULT hr = mDevice->CreateBlendState(&blendStateDescription, mBlendOn.GetAddressOf());
	if (FAILED(hr))
		Error::Log(hr, "Failed to create alpha blend state.");
	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;
	hr = mDevice->CreateBlendState(&blendStateDescription, mBlendOff.GetAddressOf());
	if (FAILED(hr))
		Error::Log(hr, "Failed to create alpha blend state.");
}

void Fire::Draw(Camera3D cam, float frameTime)
{
	//set vertex buffer
	mVertexBuffer->Bind(mContext.Get());
	//texture binds
	mTexture->Bind(mContext.Get());
	mNoise->Bind(mContext.Get());
	mAlpha->Bind(mContext.Get());
	//bind input layout
	mLayout->Bind(mContext.Get());
	//bind shaders
	mVS.Bind(mContext.Get());
	mPS.Bind(mContext.Get());
	//bind matrix constant buffer
	mMatricesBuffer->data.world = XMMatrixIdentity() * XMMatrixTranslation(posX, posY, posZ) *
		XMMatrixScaling(scaleX, scaleY, scaleZ);
	mMatricesBuffer->data.view = cam.GetViewMatrix();
	mMatricesBuffer->data.proj = cam.GetProjectionMatrix();
	mMatricesBuffer->MapData();
	mMatricesBuffer->VSBind(mContext.Get(), 0, 1);
	//bind noise constant buffer
	mNoiseBuffer->data.frameTime = frameTime;
	mNoiseBuffer->data.scales = XMFLOAT3(scalesX, scalesY, scalesZ);
	mNoiseBuffer->data.scrollSpeeds = XMFLOAT3(sSpeedX, sSpeedY, sSpeedZ);
	mNoiseBuffer->MapData();
	mNoiseBuffer->VSBind(mContext.Get(), 1, 1);
	//bind distortian constant buffer
	mDistortionBuffer->data.distortion1 = XMFLOAT2(distortion1X, distortion1Y);
	mDistortionBuffer->data.distortion2 = XMFLOAT2(distortion2X, distortion2Y);
	mDistortionBuffer->data.distortion3 = XMFLOAT2(distortion3X, distortion3Y);
	mDistortionBuffer->data.distortionScale = distortionScale;
	mDistortionBuffer->data.distortionBias = distortionBias;
	mDistortionBuffer->MapData();
	mDistortionBuffer->PSBind(mContext.Get(), 0, 1);
	
	//bind blend state
	float blendFactor[4];
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	//turn on the alpha blending.
	mContext->OMSetBlendState(mBlendOn.Get(), blendFactor, 0xffffffff);
	
	//ui render
	UI::FireUI(&posX, &posY, &posZ, &sSpeedX, &sSpeedY, &sSpeedZ, &scalesX, &scalesY, &scalesZ, &distortion1X,
		&distortion1Y, &distortion2X, &distortion2Y, &distortion3X, &distortion3Y, &distortionScale,
		&distortionBias, &scaleX, &scaleY, &scaleZ);

	//draw fire
	mContext->Draw(6, 0);

	//turn off the alpha blending
	mContext->OMSetBlendState(mBlendOff.Get(), blendFactor, 0xffffffff);
}

float* Fire::GetPosX()
{
	return &posX;
}

float* Fire::GetPosY()
{
	return &posY;
}

float* Fire::GetPosZ()
{
	return &posZ;
}

float* Fire::GetScaleX()
{
	return &scaleX;
}

float* Fire::GetScaleY()
{
	return &scaleY;
}

float* Fire::GetScaleZ()
{
	return &scaleZ;
}

float* Fire::GetSSpeedX()
{
	return &sSpeedX;
}

float* Fire::GetSSpeedY()
{
	return &sSpeedY;
}

float* Fire::GetSSpeedZ()
{
	return &sSpeedZ;
}

float* Fire::GetScalesX()
{
	return &scalesX;
}

float* Fire::GetScalesY()
{
	return &scalesY;
}

float* Fire::GetScalesZ()
{
	return &scalesZ;
}

float* Fire::GetDistortion1X()
{
	return &distortion1X;
}

float* Fire::GetDistortion1Y()
{
	return &distortion1Y;
}

float* Fire::GetDistortion2X()
{
	return &distortion2X;
}

float* Fire::GetDistortion2Y()
{
	return &distortion2Y;
}

float* Fire::GetDistortion3X()
{
	return &distortion3X;
}

float* Fire::GetDistortion3Y()
{
	return &distortion3Y;
}

float* Fire::GetDistortionScale()
{
	return &distortionScale;
}

float* Fire::GetDistortionBias()
{
	return &distortionBias;
}

float Fire::SetPosX(float value)
{
	return posX = value;
}

float Fire::SetPosY(float value)
{
	return posY = value;
}

float Fire::SetPosZ(float value)
{
	return posZ = value;
}

float Fire::SetScaleX(float value)
{
	return scaleX = value;
}

float Fire::SetScaleY(float value)
{
	return scaleY = value;
}

float Fire::SetScaleZ(float value)
{
	return scaleZ = value;
}

float Fire::SetSSpeedX(float value)
{
	return sSpeedX = value;
}

float Fire::SetSSpeedY(float value)
{
	return sSpeedY = value;
}

float Fire::SetSSpeedZ(float value)
{
	return sSpeedZ = value;
}

float Fire::SetScalesX(float value)
{
	return scalesX = value;
}

float Fire::SetScalesY(float value)
{
	return scalesY = value;
}

float Fire::SetScalesZ(float value)
{
	return scalesZ = value;
}

float Fire::SetDistortion1X(float value)
{
	return distortion1X =value;
}

float Fire::SetDistortion1Y(float value)
{
	return distortion1Y = value;
}

float Fire::SetDistortion2X(float value)
{
	return distortion2X = value;
}

float Fire::SetDistortion2Y(float value)
{
	return distortion2Y = value;
}

float Fire::SetDistortion3X(float value)
{
	return distortion3X = value;
}

float Fire::SetDistortion3Y(float value)
{
	return distortion3Y = value;
}

float Fire::SetDistortionScale(float value)
{
	return distortionScale = value;
}

float Fire::SetDistortionBias(float value)
{
	return distortionBias = value;
}
