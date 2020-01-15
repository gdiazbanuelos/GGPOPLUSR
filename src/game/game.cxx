#include <windows.h>
#include <detours.h>
#include <d3d9.h>

#include "./game.h"

HMODULE LocatePERoot() {
	return DetourGetContainingModule(DetourGetEntryPoint(NULL));
}

HRESULT LocateGameMethods(HMODULE peRoot, GameMethods* dest) {
	unsigned int peRootOffset = (unsigned int)peRoot;

	dest->GenerateAndShadePrimitives = (void(WINAPI*)())(peRootOffset + 0x149D20);
	dest->SetupD3D9 = (int(WINAPI*)())(peRootOffset + 0x148B60);
	dest->SpawnNewEffect = (GameObjectData*(*)(GameObjectData*, unsigned int, uint*))(peRootOffset + 0x71680);
	dest->SpawnNewProjectile = (GameObjectData*(*)(GameObjectData*, unsigned int, uint*))(peRootOffset + 0x23F800);
	dest->WindowFunc = (LRESULT(WINAPI*)(HWND, UINT, WPARAM, LPARAM))(peRootOffset + 0x146A00);
	dest->SteamAPI_Init = (bool(__cdecl*)())DetourFindFunction("steam_api", "SteamAPI_Init");
	dest->IsDebuggerPresent = IsDebuggerPresent;
	dest->DrawStage = (void(WINAPI*)())(peRootOffset + 0x145130);
	dest->DrawGameUI = (void(__fastcall*)(DWORD))(peRootOffset + 0x114050);

	return S_OK;
}

HRESULT LocateGameState(HMODULE peRoot, GameState* dest) {
	unsigned int peRootOffset = (unsigned int)peRoot;

	dest->bIsBackgroundEnabled = true;
	dest->bIsEffectsEnabled = true;
	dest->gameRenderTarget = (LPDIRECT3DSURFACE9*)(peRootOffset + 0x505AE0);
	dest->uiRenderTarget = (LPDIRECT3DSURFACE9*)(peRootOffset + 0x555B98);
	dest->d3dDevice = (LPDIRECT3DDEVICE9*)(peRootOffset + 0x555B94);
	dest->hWnd = (HWND*)(peRootOffset + 0x506554);
	dest->bHitboxDisplayEnabled = (DWORD*)(peRootOffset + 0x520B3C);
	dest->arrCharacters = (GameObjectData**)(peRootOffset + 0x516778);
	dest->nSceneClearColor = (DWORD*)(peRootOffset + 0x5476E8);

	return S_OK;
}