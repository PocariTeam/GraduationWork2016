#pragma once

#ifndef Singleton_h__
#define Singleton_h__
#include "Function.h"

template <class T>
class CSingleton
{
protected:
	explicit CSingleton( void ) {}
	virtual ~CSingleton( void ) {}
protected:
	static T*	m_pInstance;
public:
	static T*	GetInstance( void )
	{
		if( nullptr == m_pInstance )
			m_pInstance = new T;

		return m_pInstance;
	}

	static void DestroyInstance()
	{
		Safe_Release( m_pInstance );
	}
};

#endif // Singleton_h__
