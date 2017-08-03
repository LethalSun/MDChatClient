#pragma once
#include <stdio.h>
#include <string>
#include "../MDClientNetLib/MDClientNetLib/pch.h"
#include <Windows.h>
namespace MDClientNetworkLib
{

	class Util
	{
	public:
		Util() = default;
		~Util() = default;

		static std::wstring CharToWstring(const char* str)
		{
			
			std::string tempStr = str;
			int nLen = MultiByteToWideChar(CP_ACP, 0, &tempStr[0], tempStr.size(), NULL, NULL);
			std::wstring wstr(nLen, 0);
			MultiByteToWideChar(CP_ACP, 0, &tempStr[0], tempStr.size(), &wstr[0], nLen);
			return wstr;
		}

		static std::string WstringToString(std::wstring & wstr)
		{
			int len = WideCharToMultiByte(CP_ACP, 0, &wstr[0], -1, NULL, 0, NULL, NULL);
			std::string str(len, 0);
			WideCharToMultiByte(CP_ACP, 0, &wstr[0], -1, &str[0], len, NULL, NULL);

			return str;
		}

	};

}

