#include "WICTexture.h"
#include <algorithm>
#include <cassert>
#include <filesystem>
#include "ChoiWin.h"
#include <iomanip>

WICTexture::WICTexture(unsigned int width, unsigned int height)
{
	//initialize scratch object
	HRESULT hr = scratch.Initialize2D(format, width, height, 1u, 1u);
	if (FAILED(hr))
	{
		Error::Log(hr, "Failed to initialize 2d scracth image (bitmap)");
	}
}

unsigned int WICTexture::GetWidth() const noexcept
{	
	//get width from image meta data
	return (unsigned int)scratch.GetMetadata().width;
}

unsigned int WICTexture::GetHeight() const noexcept
{
	//get height from image meta data
	return (unsigned int)scratch.GetMetadata().height;
}

Color* WICTexture::GetBufferPtr() noexcept
{
	//get buffer as a pointer
	return reinterpret_cast<Color*>(scratch.GetPixels());
}

const Color* WICTexture::GetBufferPtr() const noexcept
{
	//get buffer as const pointer
	return const_cast<WICTexture*>(this)->GetBufferPtr();
}

const Color* WICTexture::GetBufferPtrConst() const noexcept
{
	//get buffer as pointer to pointer
	return const_cast<WICTexture*>(this)->GetBufferPtr();
}

WICTexture WICTexture::FromFile(const std::string& name)
{
	DirectX::ScratchImage scratch;
	//load wic (.png, .jpg) files
	HRESULT hr = DirectX::LoadFromWICFile(Converter::ToWide(name).c_str(), DirectX::WIC_FLAGS_IGNORE_SRGB, nullptr, scratch);
	if (FAILED(hr))
	{
		Error::Log(hr, "Failed to load wic image.");
	}

	
	if (scratch.GetImage(0, 0, 0)->format != format)
	{
		DirectX::ScratchImage converted;
		hr = DirectX::Convert(
			*scratch.GetImage(0, 0, 0),
			format,
			DirectX::TEX_FILTER_DEFAULT,
			DirectX::TEX_THRESHOLD_DEFAULT,
			converted
		);

		if (FAILED(hr)) { Error::Log(hr, "Failed to convert scracth image"); }

		return WICTexture(std::move(converted));
	}

	return WICTexture(std::move(scratch));
}

WICTexture::WICTexture(DirectX::ScratchImage scratch) noexcept
	:
	scratch(std::move(scratch))
{}