#pragma once

#ifndef Cloth_h__
#define Cloth_h__

#include "Mesh.h"
#include "Struct.h"

//************************************
// ClassName:    CCloth
// TODO:  		 
//
// Author:    	 jsm81
// Date:   	 	 2017/06/19
// Version: 	 1.0
// Contact: 	 jsm8165@naver.com
//************************************

class CCloth
	: public CMesh
{
private:
	explicit CCloth();
	explicit CCloth( const CCloth& Instance );
	virtual ~CCloth();
private:
	HRESULT CreateBuffer( ID3D11Device* pDevice, VERTEX_PNT* pVtx_Array, DWORD dwVtxCnt, DWORD* pIndex_Array, DWORD dwIdxCnt );
public:
	void	UpdateGeometryInformation( ID3D11DeviceContext* pContext, VERTEX_PNT* pVtx_Array, DWORD* pIndex_Array );
	CMesh*	Clone( void );
	DWORD   Release( void );
public:
	static  CCloth*	Create( ID3D11Device* pDevice, VERTEX_PNT* pVtx_Array, DWORD dwVtxCnt, DWORD* pIndex_Array, DWORD dwIdxCnt );
};

#endif // Cloth_h__
