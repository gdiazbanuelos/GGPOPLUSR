#include <cstdio>
#include <windows.h>
#include <detours.h>
#include <d3d9.h>
#include <ggponet.h>
#include<sstream>

#include <vdf_parser.hpp>

#include "./game.h"

static GameMethods* g_lpGameMethods;
static GameState* g_lpGameState;


void EnableHitboxes(GameState* gameState) {
	std::ofstream configFile(gameState->szConfigPath);
	gameState->config.attribs["ShowHitboxes"] = "true";
	*gameState->bHitboxDisplayEnabled = 1;
	tyti::vdf::write(configFile, gameState->config);
	configFile.close();

}

void DisableHitboxes(GameState* gameState) {
	std::ofstream configFile(gameState->szConfigPath);
	gameState->config.attribs["ShowHitboxes"] = "false";
	*gameState->bHitboxDisplayEnabled = 0;
	tyti::vdf::write(configFile, gameState->config);
	configFile.close();
}

void LoadGGPOPorts(GameState* gameState, unsigned short &nOpponentPort, unsigned short &nOurPort) {
	std::ifstream configFile(gameState->szConfigPath);
	auto root = tyti::vdf::read(configFile);
	nOurPort = std::stoi(root.attribs["GGPOLocalPort"]);
	nOpponentPort = std::stoi(root.attribs["GGPORmtPort"]);
	configFile.close();
}

void SaveGGPOPorts(GameState* gameState, unsigned short &nOpponentPort, unsigned short &nOurPort) {
	std::ofstream configFile(gameState->szConfigPath);
	gameState->config.attribs["GGPOLocalPort"] = std::to_string(nOurPort);
	gameState->config.attribs["GGPORmtPort"] = std::to_string(nOpponentPort);
	tyti::vdf::write(configFile, gameState->config);
	configFile.close();
}

HRESULT ApplyConfiguration(GameState* lpState) {
	std::unordered_map<std::string, std::string>::iterator it;
	for (it = lpState->config.attribs.begin(); it != lpState->config.attribs.end(); ++it) {
		if (it->first.compare("ShowHitboxes") == 0) {
			*lpState->bHitboxDisplayEnabled = it->second.compare("true") == 0 ? 1 : 0;
		}
	}
	return S_OK;
}

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

bool __cdecl ggpo_save_game_state_callback(
	unsigned char** buffer,
	int* len,
	int* checksum,
	int frame
) {
	*len = sizeof(SavedGameState);
	*buffer = (unsigned char*)malloc(*len);
	SavedGameState* dest = *(SavedGameState**)buffer;
	SaveGameState(g_lpGameState, dest);
	return true;
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

bool __cdecl ggpo_load_game_state_callback(unsigned char* buffer, int len) {
	SavedGameState* src = (SavedGameState*)buffer;
	LoadGameState(g_lpGameState, src);
	return true;
}

void __cdecl ggpo_free_buffer(void* buffer) {
	free(buffer);
}

bool __cdecl ggpo_advance_frame(int flags) {
	unsigned int inputs[2];
	int disconnect_flags;
	// Make sure we fetch new inputs from GGPO and use those to update
	// the game state instead of reading from the keyboard.
	ggpo_synchronize_input(g_lpGameState->ggpoState.ggpo, (void*)inputs, sizeof(int) * 2, &disconnect_flags);
	*g_lpGameState->nP1CurrentFrameInputs = inputs[0];
	*g_lpGameState->nP2CurrentFrameInputs = inputs[1];
	g_lpGameMethods->SimulateCurrentState();
	return true;
}

bool __cdecl ggpo_log_game_state(char* filename, unsigned char* buffer, int len) {
	return true;
}

bool __cdecl ggpo_begin_game(const char* game) {
	return true;
}

bool __cdecl ggpo_on_event(GGPOEvent* info) {
	int progress;
	switch (info->code) {
	case GGPO_EVENTCODE_CONNECTED_TO_PEER:
		// MessageBoxA(NULL, "connected to peer", NULL, MB_OK);
		break;
	case GGPO_EVENTCODE_SYNCHRONIZING_WITH_PEER:
		// MessageBoxA(NULL, "synchronizing to peer", NULL, MB_OK);
		break;
	case GGPO_EVENTCODE_SYNCHRONIZED_WITH_PEER:
		// MessageBoxA(NULL, "sync complete to peer", NULL, MB_OK);
		break;
	case GGPO_EVENTCODE_RUNNING:
		// MessageBoxA(NULL, "running", NULL, MB_OK);
		break;
	case GGPO_EVENTCODE_CONNECTION_INTERRUPTED:
		// MessageBoxA(NULL, "interrupted", NULL, MB_OK);
		break;
	case GGPO_EVENTCODE_CONNECTION_RESUMED:
		// MessageBoxA(NULL, "resumed", NULL, MB_OK);
		break;
	case GGPO_EVENTCODE_DISCONNECTED_FROM_PEER:
		// MessageBoxA(NULL, "disconnected", NULL, MB_OK);
		break;
	case GGPO_EVENTCODE_TIMESYNC:
		// MessageBoxA(NULL, "timesync", NULL, MB_OK);
		break;
	}
	return true;
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
	dest->BeginSceneAndDrawGamePrimitives = (void(__cdecl*)(int))(peRootOffset + 0x436F0);
	dest->DrawUIPrimitivesAndEndScene = (void(WINAPI*)())(peRootOffset + 0x14AD80);
	dest->PollForInputs = (void(WINAPI*)())(peRootOffset + 0x52630);
	dest->SimulateCurrentState = (void(WINAPI*)())(peRootOffset + 0xE7AE0);

	g_lpGameMethods = dest;

	return S_OK;
}

HRESULT LocateGameState(HMODULE peRoot, GameState* dest) {
	unsigned int peRootOffset = (unsigned int)peRoot;
	dest->nFramesToSkipRender = 0;
	dest->nFramesSkipped = 0;

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
	dest->nP1CurrentFrameInputs = (unsigned int*)(peRootOffset + 0x51EDC8);
	dest->nP2CurrentFrameInputs = (unsigned int*)(peRootOffset + 0x51EE60);
	dest->recTarget = (TrainingModeRec*)(peRootOffset + 0x4FDD28);
	dest->recStatus = (int*)(peRootOffset + 0x4FDD24);
	dest->recEnabled = (DWORD*)(peRootOffset + 0x4FDD2C);

	return S_OK;
}

void SaveRecording(char* cLogpath, GameState* gameState) {
	FILE* pFile;
	pFile = fopen(cLogpath, "wb");

	fwrite(&gameState->recTarget->nPlayer, sizeof(byte), 1, pFile);
	fwrite(gameState->recTarget->nUnknown, sizeof(byte) * 3, 1, pFile);
	fwrite(&gameState->recTarget->RecInputs, sizeof(Inputs) * 3599, 1, pFile);
	fclose(pFile);

}

void LoadRecording(char* cLogpath, GameState* gameState) {
	FILE* pFile;
	pFile = fopen(cLogpath, "rb");

	fread(&gameState->recTarget->nPlayer, sizeof(byte), 1, pFile);
	fread(gameState->recTarget->nUnknown, sizeof(byte) * 3, 1, pFile);
	fread(&gameState->recTarget->RecInputs, sizeof(Inputs) * 3599, 1, pFile);
	fclose(pFile);

}

void PrepareGGPOSession(GameState* lpGameState, unsigned short nOurPort, char* szOpponentIP, unsigned short nOpponentPort, int nOpponentPlayerPosition) {
	char msgBuffer[16];
	GGPOErrorCode result;
	GGPOSessionCallbacks* cb = &lpGameState->ggpoState.cb;
	lpGameState->ggpoState.localPlayerIndex = nOpponentPlayerPosition == 0 ? 1 : 0;
	cb->load_game_state = ggpo_load_game_state_callback;
	cb->save_game_state = ggpo_save_game_state_callback;
	cb->free_buffer = ggpo_free_buffer;
	cb->advance_frame = ggpo_advance_frame;
	cb->log_game_state = ggpo_log_game_state;
	cb->begin_game = ggpo_begin_game;
	cb->on_event = ggpo_on_event;

	g_lpGameState = lpGameState;
	lpGameState->ggpoState.ggpo = NULL;
	result = ggpo_start_session(
		&lpGameState->ggpoState.ggpo,
		cb,
		"GGPOPLUSR",
		2,
		sizeof(int), // inputsize,
		nOurPort
	);
	ggpo_set_disconnect_timeout(lpGameState->ggpoState.ggpo, 3000);
	ggpo_set_disconnect_notify_start(lpGameState->ggpoState.ggpo, 3000);
	if (!GGPO_SUCCEEDED(result)) {
		MessageBoxA(NULL, "nope", NULL, MB_OK);
	}

	if (nOpponentPlayerPosition == 0) {
		lpGameState->ggpoState.remotePlayer = &(lpGameState->ggpoState.p1);
		lpGameState->ggpoState.localPlayer = &(lpGameState->ggpoState.p2);
	}
	else {
		lpGameState->ggpoState.remotePlayer = &(lpGameState->ggpoState.p2);
		lpGameState->ggpoState.localPlayer = &(lpGameState->ggpoState.p1);
	}
	lpGameState->ggpoState.p1.size = lpGameState->ggpoState.p2.size = sizeof(GGPOPlayer);
	lpGameState->ggpoState.p1.player_num = 1;
	lpGameState->ggpoState.p2.player_num = 2;
	lpGameState->ggpoState.remotePlayer->type = GGPO_PLAYERTYPE_REMOTE;
	lpGameState->ggpoState.localPlayer->type = GGPO_PLAYERTYPE_LOCAL;
	strcpy(lpGameState->ggpoState.remotePlayer->u.remote.ip_address, szOpponentIP);
	lpGameState->ggpoState.remotePlayer->u.remote.port = nOpponentPort;

	result = ggpo_add_player(
		lpGameState->ggpoState.ggpo, &lpGameState->ggpoState.p1,
		&lpGameState->ggpoState.player_handles[0]);
	if (!GGPO_SUCCEEDED(result)) {
		sprintf(msgBuffer, "nope p1 %d", result);
		MessageBoxA(NULL, msgBuffer, NULL, MB_OK);
	}
	result = ggpo_add_player(
		lpGameState->ggpoState.ggpo, &lpGameState->ggpoState.p2,
		&lpGameState->ggpoState.player_handles[1]);
	if (!GGPO_SUCCEEDED(result)) {
		sprintf(msgBuffer, "nope p2 %d", result);
		MessageBoxA(NULL, msgBuffer, NULL, MB_OK);
	}
}
