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
	dest->WindowFunc = (LRESULT(WINAPI*)(HWND, UINT, WPARAM, LPARAM))(peRootOffset + 0x146A00);
	dest->SteamAPI_Init = (bool(__cdecl*)())DetourFindFunction("steam_api", "SteamAPI_Init");
	dest->IsDebuggerPresent = IsDebuggerPresent;

	return S_OK;
}

HRESULT LocateGameState(HMODULE peRoot, GameState* dest) {
	unsigned int peRootOffset = (unsigned int)peRoot;

	dest->gameRenderTarget = (LPDIRECT3DSURFACE9*)(peRootOffset + 0x505AE0);
	dest->uiRenderTarget = (LPDIRECT3DSURFACE9*)(peRootOffset + 0x555B98);
	dest->d3dDevice = (LPDIRECT3DDEVICE9*)(peRootOffset + 0x555B94);
	dest->hWnd = (HWND*)(peRootOffset + 0x506554);
	dest->bHitboxDisplayEnabled = (DWORD*)(peRootOffset + 0x520B3C);
	dest->arrCharacters = (GameObjectData**)(peRootOffset + 0x516778);
	dest->arrNpcObjects = (GameObjectData**)(peRootOffset + 0x51677c);
	dest->nCameraHoldTimer = (unsigned int*)(peRootOffset + 0x51B114);
	dest->fCameraXPos = (float*)(peRootOffset + 0x51B14C);
	dest->nCameraZoom = (unsigned int*)(peRootOffset + 0x51B110);
	dest->arrPlayerData = (PlayerData*)(peRootOffset + 0x51A038);
	dest->nRoundTimeRemaining = (int*)(peRootOffset + 0x50F800);
	dest->nRandomTable = (DWORD*)(peRootOffset + 0x565F20);
	dest->projectileOwner = (GameObjectData*)(peRootOffset + 0x517A78);
	dest->effectOwner = (GameObjectData*)(peRootOffset + 0x565F20);
	dest->unknownOwner = (GameObjectData*)(peRootOffset + 0x517BA8);
	dest->nPlayfieldLeftEdge = (int*)(peRootOffset + 0x51B0F4);
	dest->nPlayfieldTopEdge = (int*)(peRootOffset + 0x51B0F8);

	return S_OK;
}