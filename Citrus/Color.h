#pragma once

class Color
{
public:
	unsigned int dword;
public:
	Color() noexcept : dword()
	{}
	Color(const Color& color) noexcept;
	Color(unsigned int dword) noexcept;
	Color(unsigned char x, unsigned char r, unsigned char g, unsigned char b) noexcept;
	Color(unsigned char r, unsigned char g, unsigned char b) noexcept;
	Color(Color col, unsigned char x) noexcept;
	Color& operator =(Color color) noexcept;
	unsigned char GetX() const noexcept;
	unsigned char GetA() const noexcept;
	unsigned char GetR() const noexcept;
	unsigned char GetG() const noexcept;
	unsigned char GetB() const noexcept;
	void SetX(unsigned char x) noexcept;
	void SetA(unsigned char a) noexcept;
	void SetR(unsigned char r) noexcept;
	void SetG(unsigned char g) noexcept;
	void SetB(unsigned char b) noexcept;
};

