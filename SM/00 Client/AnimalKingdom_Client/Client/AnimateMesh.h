#pragma once
#ifndef AnimateMesh_h__
#define AnimateMesh_h__

#include "Mesh.h"

class CAnimateMesh
	: public CMesh
{
private:
	explicit CAnimateMesh();
	explicit CAnimateMesh( const CAnimateMesh& Instance );
	virtual ~CAnimateMesh();

public:
	CMesh*	Clone( void );
	HRESULT CreateBuffer( ID3D11Device* pDevice, const char* pFilePath );
	static CAnimateMesh* Create( ID3D11Device* pDevice, const char* pFilePath );
public:
	DWORD	Release( void );
};

#endif // AnimateMesh_h__