#pragma once
#include <string>

class Converter
{
public:
	Converter() = default;
public:
	static std::wstring ToWide(const std::string& narrow);
};

