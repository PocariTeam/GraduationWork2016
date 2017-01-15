#ifndef _Mesh_H_
#define _Mesh_H_

#include "Tool_Header.h"

class CMesh
{
protected:
	explicit CMesh();
	~CMesh() = default;
protected:
	HRESULT CreateBuffer( ID3D11Device* pDevice, const LPVOID pVertexData, const LPVOID pIndexData );
public:
	virtual void Release( void );
	virtual void  Render( ID3D11DeviceContext* pContext );
protected:
	ID3D11Buffer* m_pVB;
	DWORD		  m_dwVtxSize;
	DWORD		  m_dwVtxCnt;

	ID3D11Buffer* m_pIB;
	DWORD		  m_dwIdxSize;
	DWORD		  m_dwIdxCnt;
	D3D11_PRIMITIVE_TOPOLOGY m_PrimitiveTopology;
};

#endif