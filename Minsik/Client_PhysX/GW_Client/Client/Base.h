#pragma once

#ifndef _BASE_H_
#define _BASE_H_

class CBase
{
protected:
	explicit CBase();
	explicit CBase( const CBase& Instance );
	virtual ~CBase();
public:
	virtual DWORD Add_Ref( void );
private:
	DWORD	m_dwRefCnt;
public:
	virtual DWORD Release( void );
};

#endif