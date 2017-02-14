#pragma once

#ifndef Dot_h__
#define Dot_h__

#include "Mesh.h"

class CDot
	: public CMesh
{
private:
	explicit CDot();
	explicit CDot( const CDot& Instance );
	virtual ~CDot();
public:
	CMesh*	Clone( void );
	HRESULT CreateBuffer( ID3D11Device* pDevice );
	static  CDot*	Create( ID3D11Device* pDevice );
public:
	DWORD   Release( void );
};

#endif // Dot_h__
