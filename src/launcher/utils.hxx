#pragma once
#include <string>

time_t time_when_compiled();
time_t ParseISO8601(const std::string& input);
std::wstring GetHttpsRequest(const std::wstring& _server,
	const std::wstring& _page,
	const std::wstring& _useragent,
	const std::wstring& _params = L"");
