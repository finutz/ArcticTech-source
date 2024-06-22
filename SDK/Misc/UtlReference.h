#pragma once

#include "UtlVector.h"

template<class T> class CUtlReference
{
public:
	__forceinline CUtlReference(void)
	{
		m_pNext = m_pPrev = 0;
		m_pObject = 0;
	}

	__forceinline CUtlReference(T* pObj)
	{
		m_pNext = m_pPrev = 0;
		AddRef(pObj);
	}

	__forceinline CUtlReference(const CUtlReference<T>& other)
	{
		CUtlReference();

		if (other.IsValid())
		{
			AddRef((T*)(other.GetObject()));
		}
	}

	__forceinline ~CUtlReference(void)
	{
		KillRef();
	}

	__forceinline void Set(T* pObj)
	{
		if (m_pObject != pObj)
		{
			KillRef();
			AddRef(pObj);
		}
	}

	__forceinline T* operator()(void) const
	{
		return m_pObject;
	}

	__forceinline bool IsValid(void) const
	{
		return (m_pObject != 0);
	}

	__forceinline operator T* ()
	{
		return m_pObject;
	}

	__forceinline operator const T* () const
	{
		return m_pObject;
	}


	__forceinline T* GetObject(void)
	{
		return m_pObject;
	}

	__forceinline const T* GetObject(void) const
	{
		return m_pObject;
	}


	__forceinline T* operator->()
	{
		return m_pObject;
	}

	__forceinline const T* operator->() const
	{
		return m_pObject;
	}

	__forceinline CUtlReference& operator=(const CUtlReference& otherRef)
	{
		Set(otherRef.m_pObject);
		return *this;
	}

	__forceinline CUtlReference& operator=(T* pObj)
	{
		Set(pObj);
		return *this;
	}


	__forceinline bool operator==(T const* pOther) const
	{
		return (pOther == m_pObject);
	}

	__forceinline bool operator==(T* pOther) const
	{
		return (pOther == m_pObject);
	}

	__forceinline bool operator==(const CUtlReference& o) const
	{
		return (o.m_pObject == m_pObject);
	}

public:
	CUtlReference* m_pNext;
	CUtlReference* m_pPrev;

	T* m_pObject;

	__forceinline void AddRef(T* pObj)
	{
		m_pObject = pObj;
		if (pObj)
		{
			pObj->m_References.AddToHead(this);
		}
	}

	__forceinline void KillRef(void)
	{
		if (m_pObject)
		{
			m_pObject->m_References.RemoveNode(this);
			m_pObject = 0;
		}
	}
};
