#pragma once

#ifndef Mesh_h__
#define Mesh_h__

#include "Resources.h"

class CMesh
	: public CResources
{
protected:
	explicit CMesh();
	explicit CMesh( const CMesh& Instance );
	virtual ~CMesh();
public:
	virtual void Render( ID3D11DeviceContext* pContext, UINT dwInstanceCnt = 1 );
	virtual CMesh* Clone( void )PURE;
public:
	virtual DWORD Release( void );
protected:
	ID3D11Buffer*		m_pVB;
	UINT				m_dwVtxSize;
	DWORD				m_dwVtxCnt;

	ID3D11Buffer*		m_pIB;
	UINT				m_dwIdxSize;
	DWORD				m_dwIdxCnt;

	D3D11_PRIMITIVE_TOPOLOGY	m_ePrimitiveTopology;
};

#endif // Mesh_h__