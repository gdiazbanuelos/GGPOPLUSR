#ifndef UNICODE
#define UNICODE
#endif

#define DIRECTINPUT_VERSION 0x0800

#include <stdio.h>
#include <windows.h>
#include <strsafe.h>
#include <detours.h>
#include <d3d9.h>
#include <dinput.h>
#include <tchar.h>

#include "../game/game.h"
#include "../overlay/overlay.h"

#define DEFAULT_ALPHA 0.87f

static LPCWSTR DETOUR_FAILED_MESSAGE = TEXT("Could not detour targets!");

static HMODULE g_lpPEHeaderRoot;
static GameMethods g_gameMethods;
static GameState g_gameState;

HRESULT AttachInitialFunctionDetours(GameMethods* src);
HRESULT AttachInternalFunctionPointers(GameMethods* src);

LRESULT WINAPI FakeWindowFunc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (OverlayWindowFunc(hWnd, msg, wParam, lParam)) {
		return true;
	}
	return g_gameMethods.WindowFunc(hWnd, msg, wParam, lParam);
}

int FakeSetupD3D9() {
	int out = g_gameMethods.SetupD3D9();
	InitializeOverlay(&g_gameState);
	return out;
}

void FakeGenerateAndShadePrimitives() {
	// Guilty should've started the scene already.
	//
	// This method temporarily sets the render target to the UI render target
	// to draw the UI elements like the sidebars, then sets the render target
	// back to the game render target so that other methods can rely on the
	// invariant that the device's current render target is always the "game"
	// surface.
	g_gameMethods.GenerateAndShadePrimitives();

	// Since we want to draw Imgui as if it were a UI element (ex. with the
	// full, unscaled window as the viewport), set the render target while we
	// draw Imgui.
	(*g_gameState.d3dDevice)->SetRenderTarget(0, *g_gameState.uiRenderTarget);

	DrawOverlay(&g_gameMethods, &g_gameState);

	// Preserve the original method's invariant by setting the render target
	// back to the "game" surface.
	(*g_gameState.d3dDevice)->SetRenderTarget(0, *g_gameState.gameRenderTarget);

	// Guilty will end the scene after this call finishes.
}

void FakeDrawUIPrimitivesAndEndScene() {
	if (g_gameState.nFramesSkipped >= g_gameState.nFramesToSkipRender) {
		g_gameMethods.DrawUIPrimitivesAndEndScene();
	}
	else {
		g_gameState.nFramesSkipped++;
	}
}

void __cdecl FakeBeginSceneAndDrawGamePrimitives(int bShouldBeginScene) {
	if (g_gameState.nFramesSkipped >= g_gameState.nFramesToSkipRender) {
		g_gameMethods.BeginSceneAndDrawGamePrimitives(bShouldBeginScene);
	}
}

void FakePollForInputs() {
	g_gameMethods.PollForInputs();

	if (g_gameState.nFramesSkipped < g_gameState.nFramesToSkipRender) {
		*g_gameState.nP1CurrentFrameInputs = g_gameState.arrInputsDuringFrameSkip[g_gameState.nFramesSkipped][0];
		*g_gameState.nP2CurrentFrameInputs = g_gameState.arrInputsDuringFrameSkip[g_gameState.nFramesSkipped][1];
	}
}

bool FakeSteamAPI_Init() {
	DWORD error;

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	AttachInternalFunctionPointers(&g_gameMethods);
	error = DetourTransactionCommit();
	if (error != NO_ERROR) {
		MessageBox(NULL, DETOUR_FAILED_MESSAGE, NULL, 0);
	}

	return g_gameMethods.SteamAPI_Init();
}

BOOL FakeIsDebuggerPresent() {
	return FALSE;
}

HRESULT AttachInitialFunctionDetours(GameMethods* src) {
	DetourAttach(&(PVOID&)src->IsDebuggerPresent, FakeIsDebuggerPresent);
	DetourAttach(&(PVOID&)src->SteamAPI_Init, FakeSteamAPI_Init);

	return S_OK;
}

HRESULT AttachInternalFunctionPointers(GameMethods* src) {
	DetourAttach(&(PVOID&)src->GenerateAndShadePrimitives, FakeGenerateAndShadePrimitives);
	DetourAttach(&(PVOID&)src->SetupD3D9, FakeSetupD3D9);
	DetourAttach(&(PVOID&)src->WindowFunc, FakeWindowFunc);
	DetourAttach(&(PVOID&)src->BeginSceneAndDrawGamePrimitives, FakeBeginSceneAndDrawGamePrimitives);
	DetourAttach(&(PVOID&)src->DrawUIPrimitivesAndEndScene, FakeDrawUIPrimitivesAndEndScene);
	DetourAttach(&(PVOID&)src->PollForInputs, FakePollForInputs);

	return S_OK;
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

		g_lpPEHeaderRoot = LocatePERoot();
		if (LocateGameMethods(g_lpPEHeaderRoot, &g_gameMethods) != S_OK) {
			MessageBox(NULL, TEXT("Could not locate game methods!"), NULL, MB_OK);
		}

		if (LocateGameState(g_lpPEHeaderRoot, &g_gameState) != S_OK) {
			MessageBox(NULL, TEXT("Could not locate game state!"), NULL, MB_OK);
		}

		DetourTransactionBegin();
		AttachInitialFunctionDetours(&g_gameMethods);
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
		FreeOverlay();
		break;
	}
	return TRUE;
}
