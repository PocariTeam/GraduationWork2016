#pragma once

#ifndef MeshMgr_h__
#define MeshMgr_h__

#include "Singleton.h"

class CMesh;
class CMeshMgr
	: public CSingleton<CMeshMgr>
{
public:
	DWORD	Release( void );
public:
	CMesh*	Clone( const char* pKey );
	CMesh*	Find( const char* pKey );
public:
	HRESULT Add( ID3D11Device* pDevice, const char* pKey, CMesh* pMesh );
	HRESULT Add( ID3D11Device* pDevice, const char* pKey, LPVOID pVertexArray, DWORD dwVertexCnt, DWORD* pIndexArray, DWORD dwIdxCnt );
	HRESULT Add( ID3D11Device* pDevice, const char* pKey, const char* pFilePath );
	HRESULT Pop( const char* pKey );
	HRESULT Load( ID3D11Device* pDevice, const char* pFilePath );
private:
	using MESHCONTAINER = map<const string, CMesh*>;
	MESHCONTAINER	m_mapMesh;
};

#endif // MeshMgr_h__
