#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <shlwapi.h>
#include <strsafe.h>
#include <winuser.h>

#include <detours.h>

#include "discovery.hxx"

void FindSidecar(LPSTR szDllPathA) {
	char szCwdBuffer[1024] = { 0 };
	GetCurrentDirectoryA(1024, szCwdBuffer);
	PathCombineA(szDllPathA, szCwdBuffer, "Sidecar.dll");
}

void CreateAppIDFile(LPWSTR szGuiltyDirectory) {
	wchar_t szAppIDPath[1024] = { 0 };
	DWORD nBytesWritten = 0;

	PathCombine(szAppIDPath, szGuiltyDirectory, L"steam_appid.txt");

	HANDLE hAppIDHandle = CreateFile(
		szAppIDPath,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);


	if (hAppIDHandle != INVALID_HANDLE_VALUE) {
		// Didn't exist, auto-created the file.
		WriteFile(hAppIDHandle, "348550", 6, &nBytesWritten, NULL);
		if (nBytesWritten != 6) {
			// Error!
		}
		CloseHandle(hAppIDHandle);
	}
}

void CreateGuiltyGearProcess(
	LPWSTR szGuiltyDirectory,
	LPWSTR szGuiltyExePath,
	LPSTR szSidecarDllPathA
) {
	wchar_t szErrorString[1024] = { 0 };
	DWORD dwError;
	LPCSTR dllsToLoad[1] = { szSidecarDllPathA };
	STARTUPINFOW si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	si.cb = sizeof(si);

	SetLastError(0);

	if (
		!DetourCreateProcessWithDllsW(
			szGuiltyExePath,
			NULL,
			NULL,
			NULL,
			TRUE,
			CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED,
			NULL,
			szGuiltyDirectory,
			&si,
			&pi,
			1,
			dllsToLoad,
			NULL
		)) {
		dwError = GetLastError();
		StringCchPrintf(szErrorString, 1024, L"DetourCreateProcessWithDllEx failed: %d", dwError);
		MessageBox(NULL, szErrorString, NULL, MB_OK);
		MessageBox(NULL, szGuiltyDirectory, NULL, MB_OK);
		MessageBox(NULL, szGuiltyExePath, NULL, MB_OK);
		MessageBoxA(NULL, szSidecarDllPathA, NULL, MB_OK);
		if (dwError == ERROR_INVALID_HANDLE) {
			MessageBox(NULL, L"Can't detour a 64-bit target process from a 32-bit parent process or vice versa.", NULL, MB_OK);
		}
		ExitProcess(9009);
	}

	ResumeThread(pi.hThread);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	wchar_t szGuiltyDirectory[1024] = { 0 };
	wchar_t szGuiltyExePath[1024] = { 0 };
	char szSidecarDllPathA[1024] = { 0 };

	FindGuilty(szGuiltyDirectory, szGuiltyExePath);
	FindSidecar(szSidecarDllPathA);
	CreateAppIDFile(szGuiltyDirectory);
	CreateGuiltyGearProcess(szGuiltyDirectory, szGuiltyExePath, szSidecarDllPathA);
	return 0;
}
