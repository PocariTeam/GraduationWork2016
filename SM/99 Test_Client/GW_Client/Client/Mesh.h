#pragma once

#ifndef _MESH_H_
#define _MESH_H_

#include "Base.h"
class CMesh :
	public CBase
{
protected:
	CMesh();
	explicit CMesh( const CMesh& Instance );
	virtual ~CMesh();
public:
	virtual void Render( ID3D11DeviceContext* pContext, DWORD dwInstanceCnt = 1 );
	virtual CMesh* Clone() { return nullptr; }
protected:
	ID3D11Buffer* m_pVB;
	DWORD		  m_dwVtxSize;
	DWORD		  m_dwVtxCnt;

	ID3D11Buffer* m_pIB;
	DWORD		  m_dwIdxSize;
	DWORD		  m_dwIdxCnt;

	int			  m_iBufferCnt;

	D3D11_PRIMITIVE_TOPOLOGY m_PrimitiveTopology;
public:
	virtual DWORD Release( void );
};

#endif