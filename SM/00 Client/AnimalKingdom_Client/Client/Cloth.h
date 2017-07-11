#pragma once

#ifndef Cloth_h__
#define Cloth_h__

#include "GameObject.h"
#include "Struct.h"
#include "NxMeshData.h"

class NxScene;
class NxClothMeshDesc;
class NxClothMesh;
class NxCloth;
class CCloth
	: public CGameObject
{
private:
	explicit CCloth();
	virtual ~CCloth();
private:
	HRESULT		Initialize( ID3D11Device* pDevice, NxScene* pScene, NxActor* pActor, CTexture* pTexture, XMFLOAT3& vScale );
	HRESULT		CreateCloth( ID3D11Device* pDevice );
	void		ReleaseMeshDescBuffers( const NxClothMeshDesc& rDesc );
	void		AllocateReceiveBuffers( ID3D11Device* pDevice, DWORD dwVTXCnt, DWORD dwIDXCnt );
public:
	static CCloth* Create( ID3D11Device* pDevice, NxScene* pScene, NxActor* pActor, CTexture* pTexture, XMFLOAT3& vScale );
public:
	XMFLOAT4X4* GetWorld();
	NxCloth*	GetNxCloth() { return m_pCloth; }
public:
	int		Update( const float& fTimeDelta );
	void	Render( ID3D11DeviceContext* pContext );
	DWORD	Release( void );
private:
	NxScene*		m_pPhysXScene;
	bool			m_bInitialize;
	XMFLOAT3		m_vScale;
	NxClothMesh*	m_pClothMesh;
	NxCloth*		m_pCloth;
	NxMeshData		m_tReceiveBuffers;
	VERTEX_PNT*		m_pVertexArray;
	DWORD			m_dwVTXCnt;
	DWORD*			m_pIndexArray;
	DWORD			m_dwIDXCnt;
};
#endif // Cloth_h__