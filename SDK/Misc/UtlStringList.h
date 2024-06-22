#pragma once


class CUtlStringList {
	char** m_pMemory;

public:
	char* operator[](size_t id) {
		return m_pMemory[id];
	}

	size_t count() {
		return *(size_t*)((unsigned int)this + 12);
	}
};