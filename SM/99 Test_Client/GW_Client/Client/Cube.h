#ifndef _CUBE_H_
#define _CUBE_H_

#include "Mesh.h"
#include "Struct.h"

class CCube
	: public CMesh
{
private:
	explicit CCube( void ) = default;
	explicit CCube( const CCube& Instance );
	virtual ~CCube( void );
public:
	virtual HRESULT CreateBuffer( ID3D11Device* pDevice, ID3D11DeviceContext* pContext );
	CMesh*	Clone();
private:
	void	Calculate_Normal( VERTEX_TANGENT& tVertex_1, VERTEX_TANGENT& tVertex_2, VERTEX_TANGENT& tVertex_3, VERTEX_TANGENT& tVertex_4 );
	void	Calculate_Tangent( VERTEX_TANGENT& tVertex_1, VERTEX_TANGENT& tVertex_2, VERTEX_TANGENT& tVertex_3, VERTEX_TANGENT& tVertex_4 );
public:
	static CCube* Create( ID3D11Device* pDevice, ID3D11DeviceContext* pContext );
public:
	virtual DWORD Release( void );
};

#endif