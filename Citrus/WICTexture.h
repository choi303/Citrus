#pragma once
#include <d3d11.h>
#include <iostream>
#include <wrl.h>
#include <DirectXTex.h>
#include "Color.h"
#include "Converter.h"

namespace wrl = Microsoft::WRL;
class WICTexture
{
public:
	WICTexture(unsigned int width, unsigned int height);
	WICTexture(WICTexture&& wictex) noexcept = default;
	WICTexture(WICTexture&) = delete;
	WICTexture& operator=(WICTexture&& child) noexcept = default;
	WICTexture& operator=(const WICTexture& child) = delete;
	~WICTexture() = default;
	unsigned int GetWidth() const noexcept;
	unsigned int GetHeight() const noexcept;
	Color* GetBufferPtr() noexcept;
	const Color* GetBufferPtr() const noexcept;
	const Color* GetBufferPtrConst() const noexcept;
	static WICTexture FromFile(const std::string& name);
private:
	WICTexture(DirectX::ScratchImage scratch) noexcept;
private:
	static constexpr DXGI_FORMAT format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
	DirectX::ScratchImage scratch;
};