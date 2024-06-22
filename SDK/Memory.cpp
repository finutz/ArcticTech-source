#include "Memory.h"
#include <TlHelp32.h>

CMemory* Memory = new CMemory;

bool CMemory::ModuleRelativeAddress(const uintptr_t address, std::string* pModuleName, uintptr_t* pRelativeAddress) {
	*pRelativeAddress = 0;

	HANDLE modulesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
	MODULEENTRY32 currentModule;
	currentModule.dwSize = sizeof(MODULEENTRY32);

	for (Module32First(modulesSnapshot, &currentModule); GetLastError() != ERROR_NO_MORE_FILES; Module32Next(modulesSnapshot, &currentModule)) {
		if (address < (uintptr_t)currentModule.modBaseAddr || address > (uintptr_t)currentModule.modBaseAddr + (uintptr_t)currentModule.modBaseSize)
			continue;

		*pRelativeAddress = address - (uintptr_t)currentModule.modBaseAddr;
		*pModuleName = currentModule.szModule;

		return true;
	}

	return false;
}

void CMemory::DumpCallAddress(const void* returnAddress, const char* functionName) {
	std::string moduleName;
	uintptr_t offset;
	Memory->ModuleRelativeAddress((uintptr_t)returnAddress, &moduleName, &offset);
	printf("%s\t%s\t%X\n", functionName, moduleName.c_str(), offset);
}

std::string CMemory::ModuleName(const void* address_) {
	const uintptr_t address = reinterpret_cast<uintptr_t>(address_);

	HANDLE modulesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
	MODULEENTRY32 currentModule;
	currentModule.dwSize = sizeof(MODULEENTRY32);

	for (Module32First(modulesSnapshot, &currentModule); GetLastError() != ERROR_NO_MORE_FILES; Module32Next(modulesSnapshot, &currentModule)) {
		if (address < (uintptr_t)currentModule.modBaseAddr || address > (uintptr_t)currentModule.modBaseAddr + (uintptr_t)currentModule.modBaseSize)
			continue;

		return currentModule.szModule;
	}

	return "";
}

uintptr_t CMemory::ToAbsolute(const uintptr_t at) {
	const int relative = *reinterpret_cast<int*>(at);
	return at + relative + sizeof(int);
}

void CMemory::BytePatch(void* address, std::vector<unsigned char> bytes) {
	unsigned long org_prot;
	VirtualProtect(address, bytes.size(), PAGE_EXECUTE_READWRITE, &org_prot);
	memcpy(address, bytes.data(), bytes.size());
	VirtualProtect(address, bytes.size(), org_prot, &org_prot);
}