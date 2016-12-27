#pragma once

class CMesh;
class CTexture;
class CResourceMgr
{
public:
	enum MESH_TYPE { MESH_RECT, MESH_CUBE, MESH_SPHERE
		, MESH_HEIGHTMAP, MESH_BACKGROUND, MESH_POINT, MESH_OBJ, MESH_PARTICLE, MESH_END };
	enum TEXTURE_TYPE { TEXTURE_SINGLE, TEXTURE_MULTI, TEXTURE_END	};
private:
	CResourceMgr();
	~CResourceMgr();
private:
	static CResourceMgr*	m_pInstance;
public:
	static CResourceMgr*	GetInstance( void );
	static void			DestroyInstance( void );
public:
	CMesh* FindMesh( const TCHAR* pMeshKey );
	CMesh* CloneMesh( const TCHAR* pMeshKey );

	CTexture* FindTexture( const TCHAR* pTextureKey );
	CTexture* CloneTexture( const TCHAR* pTextrueKey );

public:
	HRESULT AddMesh( ID3D11Device* pDevice, ID3D11DeviceContext* pContext
		, MESH_TYPE eMeshType, const TCHAR* pMeshKey, XMFLOAT3& vSizeData = XMFLOAT3( 0.f, 0.f, 1.f ), const TCHAR* pObjPath = nullptr );
	HRESULT AddTexture( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TEXTURE_TYPE eTextureType
	, const TCHAR* pPath, const TCHAR* pTextureKey, int iTextureCnt = 1 );
private:
	using MESHCONTAINER = map<const TCHAR*, CMesh*>;
	MESHCONTAINER		m_mapMesh;

	using TEXTURECONTAINER = map<const TCHAR*, CTexture*>;
	TEXTURECONTAINER	m_mapTexture;

public:
	void Release( void );
};

