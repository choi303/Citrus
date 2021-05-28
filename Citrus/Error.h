#pragma once
#include <string>
#include <comdef.h>
#include <exception>

class Error
{
public:
	static void Log(std::string msg) noexcept
	{
		std::string errorMsg = "Error: " + msg;
		MessageBoxA(NULL, errorMsg.c_str(), "Citrus Log", MB_ICONERROR);
		exit(-1);
	}

	static void Log(HRESULT hr, std::string msg) noexcept
	{
		_com_error error(hr);
		std::string message = "Error: " + msg + "\n" + error.ErrorMessage();
		MessageBoxA(NULL, message.c_str(), "Citrus Log", MB_ICONERROR);
		exit(-1);
	}
};