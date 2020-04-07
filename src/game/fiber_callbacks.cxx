#include <windows.h>
#include <detours.h>
#include "game.h"

void MakeBFBTCallback(FiberHandler lpHandler) {
	PVOID lpCallback = DetourAllocateRegionWithinJumpBounds(lpHandler, 0x4f9 + 0x10);
	PVOID lpJumpTarget = BFBTJumpTarget;
	CopyMemory(lpCallback, lpHandler, 0x4f9);

	// SwitchToFiber call 1:
	// start + b2: mov
	// overwritten with: mov eax, 1
	char[6] mov1Overwrite = { 0xb8, 0x01, 0x00, 0x00, 0x00 };
	CopyMemory(lpCallback + 0xb2, mov1Overwrite, 0x6);
	// start + b8: push
	// start + c0: call
	char[6] call1Overwrite = { 0xb8, 0x01, 0x00, 0x00, 0x00 };
	CopyMemory(lpCallback + 0xc0, call1Overwrite, 0x6);

	// SwitchToFiber call 2:
	// start + 0x466: mov
	// overwritten with: mov eax, 2
	char[6] mov2Overwrite = { 0xb8, 0x02, 0x00, 0x00, 0x00 };
	CopyMemory(lpCallback + 0x466, mov2Overwrite, 0x6);

	// SwitchToFiber call 3:
	// start + 0x2ae: mov
	// overwritten with: mov eax, 3
	char[6] mov3Overwrite = { 0xb8, 0x03, 0x00, 0x00, 0x00 };
	CopyMemory(lpCallback + 0x2ae, mov3Overwrite, 0x6);

	// SwitchToFiber call 4:
	// start + 0x4DD: mov
	// overwritten with: mov eax, 4
	char[6] mov4Overwrite = { 0xb8, 0x04, 0x00, 0x00, 0x00 };
	CopyMemory(lpCallback + 0x4DD, mov4Overwrite, 0x6);

	// Overwrite returns:
}

typedef struct CallbackData {
	int nextCallStart;
	LPVOID data;
} CallbackData;

void BFBTCallback(CallbackData* lpcd) {
	// setup the stack
	if (lpcd->nextCallStart == 0) {
		lpcd->nextCallStart = lpHandler();
		lpcd->data = BFBTAllocate();
	}
	else {
		BFBTLoad(lpcd);
		if (lpcd->nextCallStart == 1) {
			__asm jmp start + 0xc6;
		}
		else if (lpcd->nextCallStart == 2) {
			__asm jmp start + 0x47A;
		}
		else if (lpcd->nextCallStart == 3) {
			__asm jmp start + 0x2c2;
		}
		else if (lpcd->nextCallStart == 4) {
			__asm jmp start + 0x4ed;
		}
	}

	BFBTSave(lpcd->data);
}

__declspec(naked) void BFBTJumpTarget() {

}

__declspec(naked) void BFBTLoad(CallbackData* lpcd) {
	int stackPointer;
	__asm {
		mov stackPointer, ESP;
		sub ESP, 0x18;
	}
	memcpy_s(stackPointer, 0x18, bfbtData, 0x18);
}

__declspec(naked) void BFBTSave(CallbackData* lpcd) {
	int stackPointer;
	__asm {
		mov stackPointer, esp;
	}
	memcpy_s(bfbtData, 0x18, stackPointer, 0x18);
}

LPVOID BFBTAllocate() {
	// 0x18
	return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 0x18);
}

void BFBTFree(LPVOID bfbtData) {
	HeapFree(bfbtData);
}