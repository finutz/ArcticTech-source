#include "VMTHook.h"

VMT::VMT(void* base) {
	Base = base;
	this->Vmt = *reinterpret_cast<uintptr_t**>(base);
}

void VMT::Hook(int index, void* pHook) {
	DWORD protect;
	auto address = Vmt + index;
	uintptr_t og = *(uintptr_t*)address;
	VirtualProtect(address, sizeof(address), PAGE_EXECUTE_READWRITE, &protect);
	*address = (uintptr_t)pHook;
	VirtualProtect(address, sizeof(address), protect, &protect);
	old_vmt[index] = og;
}

void* VMT::GetOriginal(int index) {
	return (void*)old_vmt[index];
}


void VMT::UnHook(int index) {
	DWORD protect;
	auto address = Vmt + index;
	VirtualProtect(address, sizeof(address), PAGE_EXECUTE_READWRITE, &protect);
	*address = (uintptr_t)old_vmt[index];
	VirtualProtect(address, sizeof(address), protect, &protect);
}