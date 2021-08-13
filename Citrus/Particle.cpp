#include "Particle.h"

static float mParticleDeviationX, mParticleDeviationY, mParticleDeviationZ;
static float mParticleVelocity, mParticleVelocityVariation;
static float mParticleSize, mParticlesPerSecond;
static int mMaxParticles;
static int mCurrentParticleCount;
static float mAccumulatedTime;
static bool mRendered;
static float lifeTime = 10.0f;
static float counter = 0.0f;
static bool isKilled;

Particle::Particle()
{
    mParticleList = 0;
    mVertices = 0;
    mVertexBuffer = 0;
    mIndexBuffer = 0;
}

bool Particle::Initialize(ID3D11Device* pDevice, std::string rPath, ID3D11DeviceContext* pContext)
{
	this->mDevice = pDevice;
	this->mContext = pContext;

	//initalize particle texture
	hr = LoadTexture(rPath);
	if (!hr)
	{
		return false;
	}

	//initialize the particle system.
	hr = InitializeParticleSystem();
	if (!hr)
	{
		return false;
	}

	//initalize vertex, index, shader buffers
	hr = InitializeBuffers();
	if (!hr)
	{
		return false;
	}

	return true;
}

void Particle::Shutdown()
{
	//release the buffers.
	ShutdownBuffers();

	//release the particle system.
	ShutdownParticleSystem();

	return;
}

bool Particle::Frame(float frameTime)
{
	if (mCurrentParticleCount > mMaxParticles)
	{
		Error::Log("Particle crashed.");
	}

	counter += 0.01f;
	if (counter >= lifeTime)
	{
		isKilled = true;
		KillParticles();
		ShutdownBuffers();
		mCurrentParticleCount = 0;
		return true;
	}
	//release old particles.
	KillParticles();

	//emit new particles.
	EmitParticles(frameTime);

	//update the position of the particles.
	UpdateParticles(frameTime);

	//update buffers
	hr = UpdateBuffers();
	if (!hr)
	{
		return false;
	}

	return true;
}

void Particle::Render(Camera3D cam)
{
	//render buffers each frame
	RenderBuffers(cam);

	return;
}

ID3D11ShaderResourceView* Particle::GetTexture()
{
	return mTexture->GetShaderResourceView();
}

int Particle::GetIndexCount()
{
	return mIndexCount;
}

bool Particle::LoadTexture(std::string rPath)
{
	mTexture = std::make_unique<Texture>(mDevice.Get(), mContext.Get(), rPath, 0);

	return true;
}

bool Particle::InitializeParticleSystem()
{
	//set values of particle settings
	mParticleDeviationX = 0.5f;
	mParticleDeviationY = 0.1f;
	mParticleDeviationZ = 2.0f;
	mParticleVelocity = 1.0f;
	mParticleVelocityVariation = 0.2f;
	mParticleSize = 0.2f;
	mParticlesPerSecond = 250.0f;
	mMaxParticles = 5000;
	//create the particle list.
	mParticleList = new ParticleType[mMaxParticles];
	if (!mParticleList)
	{
		return false;
	}

	//initialize the particle list.
	for (int i = 0; i < mMaxParticles; i++)
	{
		mParticleList[i].active = false;
	}
	mCurrentParticleCount = 0;
	mAccumulatedTime = 0.0f;

	return true;
}

void Particle::ShutdownParticleSystem()
{
	//release the particle list.
	if (mParticleList)
	{
		delete[] mParticleList;
		mParticleList = 0;
	}

	return;
}

bool Particle::InitializeBuffers()
{
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

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
	hr = mDevice->CreateBlendState(&blendStateDescription, mBlendOn.GetAddressOf());
	if (FAILED(hr)) 
		Error::Log(hr, "Failed to create alpha blend state.");
	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;
	hr = mDevice->CreateBlendState(&blendStateDescription, mBlendOff.GetAddressOf());
	if (FAILED(hr)) 
		Error::Log(hr, "Failed to create alpha blend state.");

	//shaders init
	mVS.Init(L"VSParticle.cso", mDevice.Get());
	mPS.Init(L"PSParticle.cso", mDevice.Get());

	//init and create input layout
	const std::vector<D3D11_INPUT_ELEMENT_DESC> fx_ied =
	{
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Texcoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Color", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	mLayout = std::make_unique<InputLayout>(mDevice.Get(), fx_ied, &mVS);

	fxCBuffer = std::make_unique<CBuffer<MatrixBuffer>>();
	fxCBuffer->Init(mDevice.Get(), mContext.Get());

	//set the maximum number of vertices in the vertex array.
	mVertexCount = mMaxParticles * 6;

	//set the maximum number of indices in the index array.
	mIndexCount = mVertexCount;

	//create the vertex array
	mVertices = new VertexType[mVertexCount];
	if (!mVertices)
	{
		return false;
	}

	//create the index array.
	indices = new unsigned long[mIndexCount];
	if (!indices)
	{
		return false;
	}

	//initialize vertex array
	memset(mVertices, 0, (sizeof(VertexType) * mVertexCount));

	//initialize the index array.
	for (int i = 0; i < mIndexCount; i++)
	{
		indices[i] = i;
	}

	//set up the description of the dynamic vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * mVertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = mVertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//now finally create the vertex buffer.
	hr = mDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &mVertexBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	//set up the description of the index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * mIndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	//give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//create the index buffer.
	hr = mDevice->CreateBuffer(&indexBufferDesc, &indexData, &mIndexBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	//release the index array
	delete[] indices;
	indices = 0;

	return true;
}

void Particle::ShutdownBuffers()
{
	//release the index buffer.
	if (mIndexBuffer)
	{
		mIndexBuffer->Release();
		mIndexBuffer = 0;
	}

	//release the vertex buffer.
	if (mVertexBuffer)
	{
		mVertexBuffer->Release();
		mVertexBuffer = 0;
	}

	return;
}

void Particle::EmitParticles(float frameTime)
{
	bool emitParticle, found;
	float positionX, positionY, positionZ, velocity, red, green, blue;
	int index;

	//increment the frame time.
	mAccumulatedTime += frameTime;

	//set emit particle to false for now.
	emitParticle = false;

	//check if it is time to emit a new particle or not.
	if (mAccumulatedTime > (1000.0f / mParticlesPerSecond))
	{
		mAccumulatedTime = 0.0f;
		emitParticle = true;
	}

	//if there are particles to emit then emit one per frame.
	if ((emitParticle == true) && (mCurrentParticleCount < (mMaxParticles - 1)))
	{
		mCurrentParticleCount++;

		positionX = (((float)rand() - (float)rand()) / RAND_MAX) * mParticleDeviationX;
		positionY = (((float)rand() - (float)rand()) / RAND_MAX) * mParticleDeviationY;
		positionZ = (((float)rand() - (float)rand()) / RAND_MAX) * mParticleDeviationZ;

		velocity = mParticleVelocity + (((float)rand() - (float)rand()) / RAND_MAX) * mParticleVelocityVariation;

		red = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		green = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		blue = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;

		index = 0;
		found = false;
		while (!found)
		{
			if ((mParticleList[index].active == false) || (mParticleList[index].positionZ < positionZ))
			{
				found = true;
			}
			else
			{
				index++;
			}
		}

		int i = mCurrentParticleCount;
		int j = i - 1;

		while (i != index)
		{
			mParticleList[i].positionX = mParticleList[j].positionX;
			mParticleList[i].positionY = mParticleList[j].positionY;
			mParticleList[i].positionZ = mParticleList[j].positionZ;
			mParticleList[i].red = mParticleList[j].red;
			mParticleList[i].green = mParticleList[j].green;
			mParticleList[i].blue = mParticleList[j].blue;
			mParticleList[i].velocity = mParticleList[j].velocity;
			mParticleList[i].active = mParticleList[j].active;
			i--;
			j--;
		}

		mParticleList[index].positionX = positionX;
		mParticleList[index].positionY = positionY;
		mParticleList[index].positionZ = positionZ;
		mParticleList[index].red = red;
		mParticleList[index].green = green;
		mParticleList[index].blue = blue;
		mParticleList[index].velocity = velocity;
		mParticleList[index].active = true;
	}

	return;
}

void Particle::UpdateParticles(float frameTime)
{
	//update particles
	for (int i = 0; i < mCurrentParticleCount; i++)
	{
		mParticleList[i].positionY = mParticleList[i].positionY - (mParticleList[i].velocity * frameTime * 0.001f);
	}

	return;
}

void Particle::KillParticles()
{
	//kill all the particles
	for (int i = 0; i < mMaxParticles; i++)
	{
		if ((mParticleList[i].active == true) && (mParticleList[i].positionY < -3.0f))
		{
			mParticleList[i].active = false;
			mCurrentParticleCount--;

			for (int j = i; j < mMaxParticles - 1; j++)
			{
				mParticleList[j].positionX = mParticleList[j + 1].positionX;
				mParticleList[j].positionY = mParticleList[j + 1].positionY;
				mParticleList[j].positionZ = mParticleList[j + 1].positionZ;
				mParticleList[j].red = mParticleList[j + 1].red;
				mParticleList[j].green = mParticleList[j + 1].green;
				mParticleList[j].blue = mParticleList[j + 1].blue;
				mParticleList[j].velocity = mParticleList[j + 1].velocity;
				mParticleList[j].active = mParticleList[j + 1].active;
			}
		}
	}

	return;
}

bool Particle::UpdateBuffers()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;

	//initialize vertex array
	memset(mVertices, 0, (sizeof(VertexType) * mVertexCount));
	int index = 0;

	for (int i = 0; i < mCurrentParticleCount; i++)
	{
		//bottom left.
		mVertices[index].position = XMFLOAT3(mParticleList[i].positionX - mParticleSize, mParticleList[i].positionY - mParticleSize, mParticleList[i].positionZ);
		mVertices[index].texture = XMFLOAT2(0.0f, 1.0f);
		mVertices[index].color = XMFLOAT4(mParticleList[i].red, mParticleList[i].green, mParticleList[i].blue, 1.0f);
		index++;

		//top left.
		mVertices[index].position = XMFLOAT3(mParticleList[i].positionX - mParticleSize, mParticleList[i].positionY + mParticleSize, mParticleList[i].positionZ);
		mVertices[index].texture = XMFLOAT2(0.0f, 0.0f);
		mVertices[index].color = XMFLOAT4(mParticleList[i].red, mParticleList[i].green, mParticleList[i].blue, 1.0f);
		index++;

		//bottom right.
		mVertices[index].position = XMFLOAT3(mParticleList[i].positionX + mParticleSize, mParticleList[i].positionY - mParticleSize, mParticleList[i].positionZ);
		mVertices[index].texture = XMFLOAT2(1.0f, 1.0f);
		mVertices[index].color = XMFLOAT4(mParticleList[i].red, mParticleList[i].green, mParticleList[i].blue, 1.0f);
		index++;

		//bottom right.
		mVertices[index].position = XMFLOAT3(mParticleList[i].positionX + mParticleSize, mParticleList[i].positionY - mParticleSize, mParticleList[i].positionZ);
		mVertices[index].texture = XMFLOAT2(1.0f, 1.0f);
		mVertices[index].color = XMFLOAT4(mParticleList[i].red, mParticleList[i].green, mParticleList[i].blue, 1.0f);
		index++;

		//top left.
		mVertices[index].position = XMFLOAT3(mParticleList[i].positionX - mParticleSize, mParticleList[i].positionY + mParticleSize, mParticleList[i].positionZ);
		mVertices[index].texture = XMFLOAT2(0.0f, 0.0f);
		mVertices[index].color = XMFLOAT4(mParticleList[i].red, mParticleList[i].green, mParticleList[i].blue, 1.0f);
		index++;

		//top right.
		mVertices[index].position = XMFLOAT3(mParticleList[i].positionX + mParticleSize, mParticleList[i].positionY + mParticleSize, mParticleList[i].positionZ);
		mVertices[index].texture = XMFLOAT2(1.0f, 0.0f);
		mVertices[index].color = XMFLOAT4(mParticleList[i].red, mParticleList[i].green, mParticleList[i].blue, 1.0f);
		index++;
	}

	//lock the vertex buffer.
	hr = mContext->Map(mVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
	{
		return false;
	}

	//get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	//copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)mVertices, (sizeof(VertexType) * mVertexCount));

	//unlock the vertex buffer.
	mContext->Unmap(mVertexBuffer, 0);

	return true;
}

void Particle::RenderBuffers(Camera3D cam)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	mContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	mContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer.
	mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	mTexture->Bind(mContext.Get());
	mLayout->Bind(mContext.Get());
	mVS.Bind(mContext.Get());
	mPS.Bind(mContext.Get());
	fxCBuffer->data.world = XMMatrixIdentity() * XMMatrixTranslation(0.0f, 3.0f, 0.0f) * XMMatrixScaling(10.0f, 10.0f, 10.0f);
	fxCBuffer->data.view = cam.GetViewMatrix();
	fxCBuffer->data.proj = cam.GetProjectionMatrix();
	fxCBuffer->MapData();
	fxCBuffer->VSBind(mContext.Get(), 0, 1);
	
	float blendFactor[4];
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	//turn on the alpha blending.
	mContext->OMSetBlendState(mBlendOn.Get(), blendFactor, 0xffffffff);

	mContext->DrawIndexed(GetIndexCount(), 0, 0);
	mRendered = true;

	//turn pff the alpha blending
	mContext->OMSetBlendState(mBlendOff.Get(), blendFactor, 0xffffffff);
}

float* Particle::GetDeviationX()
{
	return &mParticleDeviationX;
}

float* Particle::GetDeviationY()
{
	return &mParticleDeviationY;
}

float* Particle::GetDeviationZ()
{
	return &mParticleDeviationZ;
}

float* Particle::GetParticleVelocity()
{
	return &mParticleVelocity;
}

float* Particle::GetParticleVelocityVariation()
{
	return &mParticleVelocityVariation;
}

float* Particle::GetParticleSize()
{
	return &mParticleSize;
}

float* Particle::GetParticlesPerSecond()
{
	return &mParticlesPerSecond;
}

int* Particle::GetParticleMax()
{
	return &mMaxParticles;
}

int* Particle::GetCurrentParticleCount()
{
	return &mCurrentParticleCount;
}

float* Particle::GetAccumulatedTime()
{
	return &mAccumulatedTime;
}

bool* Particle::GetFxRendered()
{
	return &mRendered;
}

float* Particle::GetLifeTime()
{
	return &lifeTime;
}

bool* Particle::GetIsKilled()
{
	return &isKilled;
}
