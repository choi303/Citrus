#include "Converter.h"

std::wstring Converter::ToWide(const std::string& narrow)
{
	//converts reference value to wide string
	wchar_t wide[512];
	mbstowcs_s(nullptr, wide, narrow.c_str(), _TRUNCATE);
	return wide;
}
