#ifndef UNICODE
#define UNICODE
#endif

#define DIRECTINPUT_VERSION 0x0800

#include <stdio.h>
#include <windows.h>
#include <strsafe.h>
#include <detours.h>
#include <imgui.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>
#include <imgui_demo.cpp>
//#include <imgui_memory_editor.h>
#include <d3d9.h>
#include <dinput.h>
#include <tchar.h>

#include "../game.h"

#define DEFAULT_ALPHA 0.87f

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static LPCWSTR DETOUR_FAILED_MESSAGE = L"Could not detour targets!!!!!";
static LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
static HWND g_hwnd;

static BOOL(WINAPI* RealIsDebuggerPresent)() = IsDebuggerPresent;

static byte* g_lpPEHeaderRoot;
static void(WINAPI* RealGenerateAndShadePrimitives)();
static DWORD(WINAPI* RealInitializeLibraries)();
static int(WINAPI* RealSetupD3D9)();
static int(WINAPI* RealInitProcess)(int p1, int p2);
static bool(__cdecl* RealSteamAPI_Init)();
static LRESULT(WINAPI* RealWindowFunc)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void	createImguiWindows();

DWORD FakeInitializeLibraries() {
	g_hwnd = *(HWND*)(g_lpPEHeaderRoot + (0xeb6554 - 0x9b0000));

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	//ImGuiIO &g_io = ImGui::GetIO();
	//g_io.MouseDrawCursor = TRUE;
	ImGui::StyleColorsDark();
	ImGuiStyle &style = ImGui::GetStyle();
	style.Alpha = DEFAULT_ALPHA;
	ImGui_ImplWin32_Init(g_hwnd);

	return RealInitializeLibraries();
}

LRESULT WINAPI FakeWindowFunc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;
	return RealWindowFunc(hWnd, msg, wParam, lParam);
}

int FakeSetupD3D9() {
	int out = RealSetupD3D9();
	g_pd3dDevice = *(LPDIRECT3DDEVICE9*)(g_lpPEHeaderRoot + (0xf05b94 - 0x9b0000));
	ImGui_ImplDX9_Init(g_pd3dDevice);
	return out;
}

LPDIRECT3DSURFACE9 GetGameRenderTarget() {
	return *(LPDIRECT3DSURFACE9*)(g_lpPEHeaderRoot + (0xeb5ae0 - 0x9b0000));
}

LPDIRECT3DSURFACE9 GetUIRenderTarget() {
	return *(LPDIRECT3DSURFACE9*)(g_lpPEHeaderRoot + (0xf05b98 - 0x9b0000));
}

LPDIRECT3DDEVICE9 GetD3DDevice() {
	return *(LPDIRECT3DDEVICE9*)(g_lpPEHeaderRoot + (0xf05b94 - 0x9b0000));
}

void FakeGenerateAndShadePrimitives() {
	// Guilty should've started the scene already.
	LPDIRECT3DDEVICE9 d3dDevice = GetD3DDevice();

	// This method temporarily sets the render target to the UI render target
	// to draw the UI elements like the sidebars, then sets the render target
	// back to the game render target so that other methods can rely on the
	// invariant that the device's current render target is always the "game"
	// surface.
	RealGenerateAndShadePrimitives();

	// Since we want to draw Imgui as if it were a UI element (ex. with the
	// full, unscaled window as the viewport), set the render target while we
	// draw Imgui.
	d3dDevice->SetRenderTarget(0, GetUIRenderTarget());

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	createImguiWindows();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	// Preserve the original method's invariant by setting the render target
	// back to the "game" surface.
	d3dDevice->SetRenderTarget(0, GetGameRenderTarget());

	// Guilty will end the scene after this call finishes.
}

void DrawPlayerStateWindow(GameObjectData* lpGameObject) {
	ImGui::Begin(
		lpGameObject->playerIndex == 0 ? "Player 1 State" : "Player 2 State",
		NULL,
		ImGuiWindowFlags_None
	);

	ImGui::Columns(2, NULL, false);

	ImGui::Text("Object address:");
	ImGui::Text("Object ID:");
	ImGui::Text("Action ID:");
	ImGui::Text("Current Health:");
	ImGui::Text("Current Tension:");
	ImGui::Text("Character Facing:");
	ImGui::Text("Character Side:");
	ImGui::Text("Guard Balance:");
	ImGui::Text("Dizzy Value:");
	ImGui::Text("Character X Position:");
	ImGui::Text("Character Y Position:");
	ImGui::Text("Character X Velocity:");
	ImGui::Text("Character Y Velocity:");

	ImGui::NextColumn();

	ImGui::Text("%p", lpGameObject);
	ImGui::Text("%X", lpGameObject->objectID);
	ImGui::Text("%X", lpGameObject->actNo);
	ImGui::Text("%i", lpGameObject->field_0x1e);
	ImGui::Text("%hi", lpGameObject->playerData->currentTension);
	ImGui::Text("%X", lpGameObject->field_0x2);
	ImGui::Text("%X", lpGameObject->field_0x3);
	ImGui::Text("%hi", lpGameObject->playerData->guardBalance);
	ImGui::Text("%hi", lpGameObject->playerData->currentFaint);
	ImGui::Text("%i", lpGameObject->xPos);
	ImGui::Text("%i", lpGameObject->ypos);
	ImGui::Text("%i", lpGameObject->xvel);
	ImGui::Text("%i", lpGameObject->yvel);

	ImGui::End();
}

void DrawActionLogWindow(GameObjectData* lpGameObject) {
	static int lastAction[2] = { 0, 0 };
	static ExampleAppLog actionLogs[2];

	if (lpGameObject->actNo != lastAction[lpGameObject->playerIndex]) {
		actionLogs[lpGameObject->playerIndex].AddLog("Action ID: %04X\n", lpGameObject->actNo);
		lastAction[lpGameObject->playerIndex] = lpGameObject->actNo;
	}

	actionLogs[lpGameObject->playerIndex].Draw(
		lpGameObject->playerIndex == 0 ? "Player 1 Action Log" : "Player 2 Action Log"
	);
}



void createImguiWindows() {
	static bool show_p1_state = false;
	static bool show_p1_log = false;
	static bool show_p2_state = false;
	static bool show_p2_log = false;
	static bool show_hitboxes = false;
	static bool show_cheattable = false;

	GameObjectData* lpPlayerRoot = *(GameObjectData**)(g_lpPEHeaderRoot + 0x516778);
	DWORD* bHitboxDisplayEnabled = (DWORD*)(g_lpPEHeaderRoot + 0x520B3C);

	if (ImGui::IsMousePosValid() && ImGui::GetIO().MousePos.y < 200) {
		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("Windows")) {
				if (ImGui::BeginMenu("Player State")) {
					ImGui::MenuItem("Player 1 State", NULL, &show_p1_state, lpPlayerRoot != 0);
					ImGui::MenuItem("Player 1 Action Log", NULL, &show_p1_log, lpPlayerRoot != 0);
					ImGui::MenuItem("Player 2 State", NULL, &show_p2_state, lpPlayerRoot != 0);
					ImGui::MenuItem("Player 2 Action Log", NULL, &show_p2_log, lpPlayerRoot != 0);
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}

			ImGui::MenuItem(
				show_hitboxes ? "Hide Hitboxes" : "Show Hitboxes",
				NULL,
				&show_hitboxes
			);
			ImGui::EndMainMenuBar();
		}
	}

	if (show_p1_state) {
		DrawPlayerStateWindow(&lpPlayerRoot[0]);
	}
	if (show_p1_log) {
		DrawActionLogWindow(&lpPlayerRoot[0]);
	}
	if (show_p2_state) {
		DrawPlayerStateWindow(&lpPlayerRoot[1]);
	}
	if (show_p2_log) {
		DrawActionLogWindow(&lpPlayerRoot[1]);
	}
	if (show_hitboxes) {
		if (*bHitboxDisplayEnabled == 0) {
			*bHitboxDisplayEnabled = 1;
		}
	}
	else {
		if (*bHitboxDisplayEnabled != 0) {
			*bHitboxDisplayEnabled = 0;
		}
	}
}

void FakeSteamAPI_Init() {
	DWORD error;

	RealGenerateAndShadePrimitives = (void(WINAPI*)())(g_lpPEHeaderRoot + (0x00af9d20 - 0x9b0000));
	RealInitializeLibraries = (DWORD(WINAPI*)())(g_lpPEHeaderRoot + (0xaf6c90 - 0x9b0000));
	RealSetupD3D9 = (int(WINAPI*)())(g_lpPEHeaderRoot + (0xaf8b60 - 0x9b0000));
	RealWindowFunc = (LRESULT(WINAPI*)(HWND, UINT, WPARAM, LPARAM))(g_lpPEHeaderRoot + (0xaf6a00 - 0x9b0000));

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)RealGenerateAndShadePrimitives, FakeGenerateAndShadePrimitives);
	DetourAttach(&(PVOID&)RealInitializeLibraries, FakeInitializeLibraries);
	DetourAttach(&(PVOID&)RealSetupD3D9, FakeSetupD3D9);
	DetourAttach(&(PVOID&)RealWindowFunc, FakeWindowFunc);
	error = DetourTransactionCommit();
	if (error != NO_ERROR) {
		MessageBox(NULL, TEXT("Detour failure!"), NULL, 0);
	}

	RealSteamAPI_Init();
}

BOOL FakeIsDebuggerPresent() {
	// This in theory can be disabled for a production release
	return FALSE;
}

__declspec(dllexport) BOOL WINAPI DllMain(
	HINSTANCE hinstDLL,
	DWORD dwReason,
	LPVOID reserved
) {
	LONG error;
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DetourRestoreAfterWith();

		g_lpPEHeaderRoot = (byte*)DetourGetContainingModule(
			DetourGetEntryPoint(NULL)
		);
		RealSteamAPI_Init = (bool(__cdecl*)())DetourFindFunction("steam_api", "SteamAPI_Init");

		DetourTransactionBegin();
		DetourAttach(&(PVOID&)RealIsDebuggerPresent, FakeIsDebuggerPresent);
		DetourAttach(&(PVOID&)RealSteamAPI_Init, FakeSteamAPI_Init);
		error = DetourTransactionCommit();

		if (error != NO_ERROR) {
			MessageBox(NULL, DETOUR_FAILED_MESSAGE, NULL, MB_OK);
		}

		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;

	case DLL_PROCESS_DETACH:
		ImGui_ImplDX9_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
		break;
	}
	return TRUE;
}
