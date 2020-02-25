#include <windows.h>
#include <winhttp.h>
#include <iostream>
#include <sstream>
#include <time.h>

#include "utils.hxx"

const int RESPONSE_DATA_BUFFER_SIZE = 10000;

//https://stackoverflow.com/a/49941336
time_t ParseISO8601(const std::string& input)
{
    constexpr const size_t expectedLength = sizeof("1234-12-12T12:12:12Z") - 1;
    static_assert(expectedLength == 20, "Unexpected ISO 8601 date/time length");

    if (input.length() < expectedLength)
    {
        return 0;
    }

    std::tm time = { 0 };
    time.tm_year = std::strtol(&input[0], nullptr, 10) - 1900;
    time.tm_mon = std::strtol(&input[5], nullptr, 10) - 1;
    time.tm_mday = std::strtol(&input[8], nullptr, 10);
    time.tm_hour = std::strtol(&input[11], nullptr, 10);
    time.tm_min = std::strtol(&input[14], nullptr, 10);
    time.tm_sec = std::strtol(&input[17], nullptr, 10);
    time.tm_isdst = 0;
    return mktime(&time);
}

// GET HTTPS request. Not asynchronous!
// Largely inspired by https://stackoverflow.com/q/18910463
std::wstring GetHttpsRequest(const std::wstring& _server,
    const std::wstring& _page,
    const std::wstring& _useragent,
    const std::wstring& _params)
{
    // fat buffer
    char responseDataBuffer[RESPONSE_DATA_BUFFER_SIZE] = { 0 };

    // initialize WinInet
    HINTERNET hInternet = WinHttpOpen(_useragent.c_str(), WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (hInternet == NULL)
    {
        return L"";
    }

    // open HTTP session
    HINTERNET hConnect = WinHttpConnect(hInternet, _server.c_str(), INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (hConnect == NULL)
    {
        return L"";
    }

    std::wstring request = _page +
        (_params.empty() ? L"" : (L"?" + _params));

    // open request
    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", (LPCWSTR)request.c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    if (hRequest == NULL)
    {
        return L"";
    }

    // send request
    BOOL isSend = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);

    WinHttpReceiveResponse(hRequest, NULL);

    if (!isSend)
    {
        return L"";
    }

    BOOL bKeepReading = TRUE;
    DWORD dwBytesRead = -1;

    while (bKeepReading && dwBytesRead != 0)
    {
        // reading data
        bKeepReading = WinHttpReadData(hRequest, responseDataBuffer, sizeof(responseDataBuffer) - 1, &dwBytesRead);

        //zero-terminate the buffer
        if (dwBytesRead > -1) {
            responseDataBuffer[dwBytesRead] = 0;
        }
    }


    // close request
    WinHttpCloseHandle(hRequest);

    // close session
    WinHttpCloseHandle(hConnect);

    // close WinInet
    WinHttpCloseHandle(hInternet);

    std::wstring answer = std::wstring(responseDataBuffer, responseDataBuffer + strlen(responseDataBuffer));

    return answer;
}