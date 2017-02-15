#pragma once

#ifndef AnimateMeshMgr_h__
#define AnimateMeshMgr_h__

#include "Singleton.h"

class CMesh;
class CAnimateMeshMgr
	: public CSingleton<CAnimateMeshMgr>
{
public:
	DWORD	Release( void );
public:
	CMesh*	Clone( const char* pKey );
	CMesh*	Find( const char* pKey );
	HRESULT Add( ID3D11Device* pDevice, const char* pKey, const char* pFilePath );
	HRESULT Load( ID3D11Device* pDevice, const char* pFilePath );
private:
	using MESHCONTAINER = map<const string, CMesh*>;
	MESHCONTAINER	m_mapMesh;
};

#endif // AnimateMeshMgr_h__