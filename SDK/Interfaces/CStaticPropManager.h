#pragma once
#include "../Misc/UtlVector.h"
#include "../Misc/Vector.h"

class IClientUnknown;

class IClientAlphaProperty
{
public:
	// Gets at the containing class...
	virtual IClientUnknown* GetClientUnknown() = 0;

	// Sets a constant alpha modulation value
	virtual void SetAlphaModulation(unsigned char a) = 0;
};

class CStaticProp
{
public:
	char pad_0000[16]; //0x0000
	Vector m_Origin; //0x0010
	char pad_001C[24]; //0x001C
	uint32_t m_Alpha; //0x0034
	char pad_0038[20]; //0x0038
	IClientAlphaProperty* m_pClientAlphaProperty; //0x004C
	char pad_0050[160]; //0x0050
	float m_DiffuseModulation[4]; //0x00F0
};

class CStaticPropMgr {
	void* __vfptr1; //0x0000
	void* __vfptr2; //0x0004
	char pad_0008[20]; //0x0008
public:
	CUtlVector<CStaticProp> m_StaticProps;
};