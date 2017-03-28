#pragma once

#ifndef Base_h__
#define Base_h__

class CBase
{
protected:
	explicit CBase()
		:m_dwRefCnt( 0 )
	{
	}
	explicit CBase( const CBase& Instance )
		:m_dwRefCnt( Instance.m_dwRefCnt )
	{
		Add_Ref();
	}
public:
	virtual DWORD Add_Ref( void )
	{
		return ++m_dwRefCnt;
	}
private:
	DWORD	m_dwRefCnt;
public:
	virtual DWORD Release( void )
	{
		return m_dwRefCnt == 0 ? m_dwRefCnt : m_dwRefCnt--;
	}
};

#endif // Base_h__