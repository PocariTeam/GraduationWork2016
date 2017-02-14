#pragma once

#ifndef Background_h__
#define Background_h__

#include "Mesh.h"

class CBackground
	: public CMesh
{
private:
	explicit CBackground();
	explicit CBackground( const CBackground& Instance );
	virtual ~CBackground();
private:
	HRESULT CreateBuffer( void );
public:
	CMesh*	Clone( void );
	DWORD   Release( void );
public:
	static  CBackground*	Create( void );
};

#endif // Background_h__