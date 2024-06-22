#pragma once
#include <Windows.h>

class VMT {
private:
	void* Base;
	uintptr_t* Vmt;
	uintptr_t old_vmt[256];
public:
	VMT(void* base);
	void Hook(int index, void* pHooked);
	void UnHook(int index);
	void* GetOriginal(int index);
};