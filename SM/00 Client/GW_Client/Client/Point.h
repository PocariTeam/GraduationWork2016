#ifndef _POINT_H_
#define _POINT_H_

#include "Mesh.h"
#include "Struct.h"

class CPoint
	: public CMesh
{
private:
	explicit CPoint( void ) = default;
	explicit CPoint( const CPoint& Instance );
	virtual ~CPoint( void );
public:
	virtual HRESULT CreateBuffer( ID3D11Device* pDevice, ID3D11DeviceContext* pContext );
	CMesh*	Clone();

	static CPoint* Create( ID3D11Device* pDevice, ID3D11DeviceContext* pContext );
public:
	virtual DWORD Release( void );
};

#endif