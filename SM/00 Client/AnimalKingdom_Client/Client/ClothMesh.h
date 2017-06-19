#pragma once

#ifndef ClothMesh_h__
#define ClothMesh_h__

#include "Mesh.h"
#include "Struct.h"

//************************************
// ClassName:    CClothMesh
// TODO:  		 
//
// Author:    	 jsm81
// Date:   	 	 2017/06/19
// Version: 	 1.0
// Contact: 	 jsm8165@naver.com
//************************************

class CClothMesh
	: public CMesh
{
private:
	explicit CClothMesh();
	explicit CClothMesh( const CClothMesh& Instance );
	virtual ~CClothMesh();
private:
	HRESULT CreateBuffer( ID3D11Device* pDevice, VERTEX_PNT* pVtx_Array, DWORD dwVtxCnt, DWORD* pIndex_Array, DWORD dwIdxCnt );
public:
	void	UpdateGeometryInformation( ID3D11DeviceContext* pContext, VERTEX_PNT* pVtx_Array, DWORD* pIndex_Array );
	CMesh*	Clone( void );
	DWORD   Release( void );
public:
	static  CClothMesh*	Create( ID3D11Device* pDevice, VERTEX_PNT* pVtx_Array, DWORD dwVtxCnt, DWORD* pIndex_Array, DWORD dwIdxCnt );
};

#endif // ClothMesh_h__
