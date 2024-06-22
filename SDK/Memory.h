#pragma once
#include <Windows.h>
#include <string>
#include <vector>

#define DUMP_RETURN(func) Memory->DumpCallAddress(_ReturnAddress(), func);

class CMemory
{
public:
	bool		ModuleRelativeAddress(const uintptr_t address, std::string* pModuleName, uintptr_t* pRelativeAddress);
	void		DumpCallAddress(const void* returnAddress, const char* functionName);
	std::string	ModuleName(const void* address);
	uintptr_t	ToAbsolute(const uintptr_t at);
	void		BytePatch(void* address, std::vector<unsigned char> bytes);
};

extern CMemory* Memory;