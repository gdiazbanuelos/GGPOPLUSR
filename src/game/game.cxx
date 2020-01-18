#include <windows.h>
#include <detours.h>
#include <d3d9.h>

#include "./game.h"

void SaveGameState(GameState* gameState, SavedGameState* dest) {
	CopyMemory(dest->arrCharacters, *gameState->arrCharacters, sizeof(GameObjectData) * 2);
	CopyMemory(dest->arrNpcObjects, *gameState->arrNpcObjects, sizeof(GameObjectData) * 60);
	CopyMemory(dest->arrPlayerData, gameState->arrPlayerData, sizeof(PlayerData) * 2);
	CopyMemory(&dest->projectileOwner, gameState->projectileOwner, sizeof(GameObjectData));
	CopyMemory(&dest->effectOwner, gameState->effectOwner, sizeof(GameObjectData));
	CopyMemory(&dest->unknownOwner, gameState->unknownOwner, sizeof(GameObjectData));
	dest->fCameraXPos = *gameState->fCameraXPos;
	dest->nCameraHoldTimer = *gameState->nCameraHoldTimer;
	dest->nCameraZoom = *gameState->nCameraZoom;
	dest->nRoundTimeRemaining = *gameState->nRoundTimeRemaining;
	CopyMemory(dest->nRandomTable, gameState->nRandomTable, sizeof(DWORD) * 0x272);
	dest->nPlayfieldLeftEdge = *gameState->nPlayfieldLeftEdge;
	dest->nPlayfieldTopEdge = *gameState->nPlayfieldTopEdge;
	CopyMemory(&dest->nCameraPlayerXPositionHistory, gameState->nCameraPlayerXPositionHistory, sizeof(int) * 2);
	CopyMemory(&dest->nCameraPlayerXMovementMagnitudeHistory, gameState->nCameraPlayerXMovementMagnitudeHistory, sizeof(int) * 2);
	CopyMemory(&dest->arrnP1InputRingBuffer, gameState->arrnP1InputRingBuffer, sizeof(WORD) * 32);
	CopyMemory(&dest->arrnP2InputRingBuffer, gameState->arrnP2InputRingBuffer, sizeof(WORD) * 32);
	dest->nP1InputRingBufferPosition = *gameState->nP1InputRingBufferPosition;
	dest->nP2InputRingBufferPosition = *gameState->nP2InputRingBufferPosition;
}

void LoadGameState(GameState* gameState, SavedGameState* src) {
	CopyMemory(*gameState->arrCharacters, src->arrCharacters, sizeof(GameObjectData) * 2);
	CopyMemory(*gameState->arrNpcObjects, src->arrNpcObjects, sizeof(GameObjectData) * 60);
	CopyMemory(gameState->arrPlayerData, src->arrPlayerData, sizeof(PlayerData) * 2);
	CopyMemory(gameState->projectileOwner, &src->projectileOwner, sizeof(GameObjectData));
	CopyMemory(gameState->effectOwner, &src->effectOwner, sizeof(GameObjectData));
	CopyMemory(gameState->unknownOwner, &src->unknownOwner, sizeof(GameObjectData));
	*gameState->fCameraXPos = src->fCameraXPos;
	*gameState->nCameraHoldTimer = src->nCameraHoldTimer;
	*gameState->nCameraZoom = src->nCameraZoom;
	*gameState->nRoundTimeRemaining = src->nRoundTimeRemaining;
	CopyMemory(gameState->nRandomTable, src->nRandomTable, sizeof(DWORD) * 0x272);
	*gameState->nPlayfieldLeftEdge = src->nPlayfieldLeftEdge;
	*gameState->nPlayfieldTopEdge = src->nPlayfieldTopEdge;
	CopyMemory(gameState->nCameraPlayerXPositionHistory, &src->nCameraPlayerXPositionHistory, sizeof(int) * 2);
	CopyMemory(gameState->nCameraPlayerXMovementMagnitudeHistory, &src->nCameraPlayerXMovementMagnitudeHistory, sizeof(int) * 2);
	CopyMemory(gameState->arrnP1InputRingBuffer, &src->arrnP1InputRingBuffer, sizeof(WORD) * 32);
	CopyMemory(gameState->arrnP2InputRingBuffer, &src->arrnP2InputRingBuffer, sizeof(WORD) * 32);
	*gameState->nP1InputRingBufferPosition = src->nP1InputRingBufferPosition;
	*gameState->nP2InputRingBufferPosition = src->nP2InputRingBufferPosition;
}

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
	dest->nCameraPlayerXPositionHistory = (int*)(peRootOffset + 0x51B12C);
	dest->nCameraPlayerXMovementMagnitudeHistory = (int*)(peRootOffset + 0x51B138);
	dest->arrnP1InputRingBuffer = (WORD*)(peRootOffset + 0x516200);
	dest->arrnP2InputRingBuffer = (WORD*)(peRootOffset + 0x516240);
	dest->nP1InputRingBufferPosition = (int*)(peRootOffset + 0x516280);
	dest->nP2InputRingBufferPosition = (int*)(peRootOffset + 0x516284);

	return S_OK;
}