#include "Color.h"

Color::Color(const Color& color) noexcept
    :
    dword(color.dword)  //set parent dword to dword
{}

Color::Color(unsigned int dword) noexcept
    :
    dword(dword)
{}

Color::Color(unsigned char x, unsigned char r, unsigned char g, unsigned char b) noexcept
    :
    dword((x << 24u) | (r << 16u) | (g << 8u) | b)
{}

Color::Color(unsigned char r, unsigned char g, unsigned char b) noexcept
    :
    dword((r << 16u) | (g << 8u) | b)
{}

Color::Color(Color col, unsigned char x) noexcept
    :
    Color((x << 24u) | col.dword)
{}

Color& Color::operator=(Color color) noexcept
{
    dword = color.dword;
    return *this;
}

unsigned char Color::GetX() const noexcept
{
    return dword >> 24u;
}

unsigned char Color::GetA() const noexcept
{
    return GetX();
}

unsigned char Color::GetR() const noexcept
{
    return (dword >> 16u) & 0xFFu;
}

unsigned char Color::GetG() const noexcept
{
    return (dword >> 8u) &  0xFFu;
}

unsigned char Color::GetB() const noexcept
{
    return dword & 0xFFu;
}

void Color::SetX(unsigned char x) noexcept
{
    dword = (dword & 0xFFFFFFu) | (x << 24u);
}

void Color::SetA(unsigned char a) noexcept
{
    SetX(a);
}

void Color::SetR(unsigned char r) noexcept
{
    dword = (dword & 0xFF00FFFFu) | (r << 16u);
}

void Color::SetG(unsigned char g) noexcept
{
    dword = (dword & 0xFFFF00FFu) | (g << 8u);
}

void Color::SetB(unsigned char b) noexcept
{
    dword = (dword & 0xFFFFFF00u) | b;
}
