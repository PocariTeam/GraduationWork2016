#include "stdafx.h"
#include "Base.h"


CBase::CBase()
	:m_dwRefCnt( 0 )
{
}

CBase::CBase( const CBase & Instance )
	:m_dwRefCnt( Instance.m_dwRefCnt )
{
	Add_Ref();
}

CBase::~CBase()
{
}

DWORD CBase::Add_Ref( void )
{
	return ++m_dwRefCnt;
}

DWORD CBase::Release( void )
{
	return m_dwRefCnt == 0 ? m_dwRefCnt : m_dwRefCnt--;
}
