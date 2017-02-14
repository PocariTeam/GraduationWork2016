#ifndef _ACTORSHAPE_H_
#define _ACTORSHAPE_H_

#include "Mesh.h"
#include "Struct.h"

class CActorShape
	: public CMesh
{
private:
	explicit CActorShape( void ) = default;
	explicit CActorShape( const CActorShape& Instance );
	virtual ~CActorShape( void );
public:
	HRESULT LoadMeshFromJsm( ID3D11Device* pDevice, const TCHAR* pJsmPath );
	virtual HRESULT CreateBuffer( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* pJsmPath );
	CMesh*	Clone();
public:
	static CActorShape* Create( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* pJsmPath );
public:
	virtual DWORD Release( void );
private:
	XMFLOAT3*	  m_pVertex;
};

#endif