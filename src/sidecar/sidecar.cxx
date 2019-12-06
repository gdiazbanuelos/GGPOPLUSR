#ifndef UNICODE
#define UNICODE
#endif

#include <stdio.h>
#include <windows.h>
#include <detours.h>

LPCWSTR DETOUR_SUCCESSFUL_MESSAGE = L"Detouring successful!";
LPCWSTR DETOUR_FAILED_MESSAGE = L"Could not detour targets!!!!!";

static BOOL(WINAPI* RealIsDebuggerPresent)() = IsDebuggerPresent;

static void(WINAPI* RealDrawStage)();
static void(__fastcall* RealDrawGameUI)(DWORD);
static void(WINAPI* RealD3DSceneSetting)(int);

static LPCWSTR szMessage;

BOOL FakeIsDebuggerPresent() {
	// This in theory can be disabled for a production release
	return FALSE;
}

static HINSTANCE g_hinst;
static HWND hwndMain;
static byte* g_lpPEHeaderRoot;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void WINAPI FakeDrawStage() {}
void __fastcall FakeDrawGameUI(DWORD param1) {}
void WINAPI FakeD3DSceneSetting(int param1) {
	// clear memory address: RVA 0x5476E8
	DWORD* g_lpnSceneClearColor = (DWORD*)((byte*)g_lpPEHeaderRoot + 0x5476E8);
	*g_lpnSceneClearColor = 0x00ff00ff;
	return RealD3DSceneSetting(param1);
}

void SetCleanBackgroundState() {
	long error;

	g_lpPEHeaderRoot = (byte*)DetourEnumerateModules(NULL);
	RealDrawStage = (void(WINAPI*)())(
		g_lpPEHeaderRoot + (0x00af5130 - 0x9b0000)
	);
	RealDrawGameUI = (void(__fastcall*)(DWORD))(
		g_lpPEHeaderRoot + (0x00ac4050 - 0x9b0000)
	);
	RealD3DSceneSetting = (void(WINAPI*)(int))(
		g_lpPEHeaderRoot + (0x009f36f0 - 0x9b0000)
	);

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)RealDrawStage, FakeDrawStage);
	DetourAttach(&(PVOID&)RealDrawGameUI, FakeDrawGameUI);
	DetourAttach(&(PVOID&)RealD3DSceneSetting, FakeIsDebuggerPresent);
	error = DetourTransactionCommit();
}

HWND InitializeMainWindow(HINSTANCE hinst) {
	const wchar_t CLASS_NAME[] = L"GGPOPLUSR Sidecar";

	WNDCLASS wc = { };

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hinst;
	wc.lpszClassName = CLASS_NAME;

	return CreateWindowEx(
		0,
		CLASS_NAME,
		L"GGPOPLUSR Sidecar",
		WS_OVERLAPPEDWINDOW,

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, 600, 200,

		NULL,
		NULL,
		g_hinst,
		NULL
	);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		CreateWindowEx(
			0,
			L"BUTTON",
			L"Set Clean Background State",
			WS_CHILD | WS_BORDER | WS_VISIBLE,

			0,
			0,
			300,
			50,

			hwnd,
			NULL,
			g_hinst,
			NULL
		);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_COMMAND:
		if (HIWORD(wParam) == BN_CLICKED) {
			SetCleanBackgroundState();
			return 0;
		}
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

__declspec(dllexport) BOOL WINAPI DllMain(
	HINSTANCE hinst,
	DWORD dwReason,
	LPVOID reserved
) {
	LONG error;

	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		g_hinst = hinst;

		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourAttach(&(PVOID&)RealIsDebuggerPresent, FakeIsDebuggerPresent);
		error = DetourTransactionCommit();

		if (error == NO_ERROR) {
			szMessage = DETOUR_SUCCESSFUL_MESSAGE;
		}
		else {
			szMessage = DETOUR_FAILED_MESSAGE;
		}
		break;

	case DLL_THREAD_ATTACH:
		hwndMain = InitializeMainWindow(hinst);
		ShowWindow(hwndMain, 0);
		break;

	case DLL_THREAD_DETACH:
		break;

	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
