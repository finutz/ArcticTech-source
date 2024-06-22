#pragma once

template<typename FuncType>
__forceinline static FuncType CallVFunction(void* ppClass, int index)
{
    int* pVTable = *(int**)ppClass;
    int dwAddress = pVTable[index];
    return (FuncType)(dwAddress);
}

template <typename T>
static auto RelativeToAbsolute(int address) {
    return (T)(address + 4 + *(int*)address);
}