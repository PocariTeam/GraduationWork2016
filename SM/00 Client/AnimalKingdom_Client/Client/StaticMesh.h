#pragma once

#ifndef StaticMesh_h__
#define StaticMesh_h__

#include "Mesh.h"

class CStaticMesh
	: public CMesh
{
private:
	explicit CStaticMesh();
	explicit CStaticMesh( const CStaticMesh& Instance );
	virtual ~CStaticMesh();
public:
	CMesh*	Clone( void );
	HRESULT CreateBuffer( ID3D11Device* pDevice, const char* pFilePath );
	static CStaticMesh*	Create( ID3D11Device* pDevice, const char* pFilePath );
public:
	DWORD	Release( void );
};

#endif // StaticMesh_h__