#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <shlwapi.h>
#include <strsafe.h>
#include <winuser.h>

#include <detours.h>

static wchar_t g_szWorkingDirBuffer[512] = { 0 };

static HINSTANCE hinst;
static HWND hPathBox;

int CreateGuiltyGearProcess(LPCWSTR szWorkingDir) {
	wchar_t szCwdBuffer[512] = { 0 };
	wchar_t szDllPathW[512] = { 0 };
	wchar_t szExePath[512] = { 0 };
	char szDllPathA[512] = { 0 };
	wchar_t szErrorString[1024] = { 0 };

	GetCurrentDirectory(512, szCwdBuffer);
	PathCombine(szDllPathW, szCwdBuffer, L"Sidecar.dll");
	WideCharToMultiByte(
		CP_ACP,
		0,
		szDllPathW,
		-1,
		szDllPathA,
		512,
		NULL,
		NULL
	);

	PathCombine(szExePath, szWorkingDir, L"GGXXACPR_Win.exe");

	SetLastError(0);

	LPCSTR dllsToLoad[1];
	dllsToLoad[0] = szDllPathA;

	PCHAR pszFilePart = NULL;
	PCHAR pszFilePartExe = NULL;
	STARTUPINFOW si;
	PROCESS_INFORMATION pi;
	DWORD dwFlags = CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED;
	DWORD dwResult = 0;
	DWORD nDlls = 1;

	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	si.cb = sizeof(si);

	if (
		!DetourCreateProcessWithDllsW(
			szExePath,
			NULL,
			NULL,
			NULL,
			TRUE,
			dwFlags,
			NULL,
			szWorkingDir,
			&si,
			&pi,
			nDlls,
			dllsToLoad,
			NULL
		)) {
		DWORD dwError = GetLastError();

		StringCchPrintf(szErrorString, 1024, L"DetourCreateProcessWithDllEx failed: %d", dwError);

		MessageBox(NULL, szErrorString, NULL, MB_OK);
		MessageBox(NULL, szExePath, NULL, MB_OK);
		MessageBox(NULL, szWorkingDir, NULL, MB_OK);
		MessageBox(NULL, szDllPathW, NULL, MB_OK);

		if (dwError == ERROR_INVALID_HANDLE) {
			MessageBox(NULL, L"Can't detour a 64-bit target process from a 32-bit parent process or vice versa.", NULL, MB_OK);
		}
		ExitProcess(9009);
	}

	ResumeThread(pi.hThread);
	WaitForSingleObject(pi.hProcess, INFINITE);

	if (!GetExitCodeProcess(pi.hProcess, &dwResult)) {
		printf("GetExitCodeProcess failed: %d\n", GetLastError());
		return 9010;
	}
	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	hinst = hInstance;

	const wchar_t CLASS_NAME[] = L"GGPOPLUSR Launcher";

	WNDCLASS wc = { };

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	// Create the window.
	HWND hwndMain = CreateWindowEx(
		0,
		CLASS_NAME,
		L"GGPOPLUSR Launcher",
		WS_OVERLAPPEDWINDOW,

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, 600, 200,

		NULL,
		NULL,
		hInstance,
		NULL
	);


	if (!hwndMain)
	{
		return 0;
	}

	ShowWindow(hwndMain, nCmdShow);

	// Run the message loop.
	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:

		CreateWindowEx(
			0,
			L"STATIC",
			L"Path to Guilty Gear working dir",
			WS_CHILD | WS_BORDER | WS_VISIBLE,

			0,
			0,
			300,
			50,

			hwnd,
			NULL,
			hinst,
			NULL
		);

		hPathBox = CreateWindowEx(
			0,
			L"EDIT",
			L"E:\\Steam\\steamapps\\common\\Guilty Gear XX Accent Core Plus R",
			WS_CHILD | WS_BORDER | WS_VISIBLE,

			0,
			50,
			600,
			50,

			hwnd,
			NULL,
			hinst,
			NULL
		);

		// Button to kick off
		CreateWindowEx(
			0,
			L"BUTTON",
			L"Launch",
			WS_CHILD | WS_BORDER | WS_VISIBLE,

			0,
			100,
			300,
			50,

			hwnd,
			NULL,
			hinst,
			NULL
		);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_COMMAND:
		if (HIWORD(wParam) == BN_CLICKED) {
			GetWindowText(hPathBox, g_szWorkingDirBuffer, 512);
			CreateGuiltyGearProcess(g_szWorkingDirBuffer);
			// XXX Translate this to a quit message? Install into
			// Guilty dir?
			return 0;
		}
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}