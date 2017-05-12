#pragma once

#ifndef Crown_h__
#define Crown_h__

#include "GameObject.h"

class CPlayer;
class CCrown
	: public CGameObject
{
private:
	explicit CCrown();
	virtual ~CCrown();
private:
	HRESULT		Initialize( ID3D11Device* pDevice, NxActor* pActor, CMesh* pMesh, CTexture* pTexture, XMFLOAT3& vScale );
public:
	static CCrown* Create( ID3D11Device* pDevice, NxActor* pActor, CMesh* pMesh, CTexture* pTexture, XMFLOAT3& vScale );
public:
	int			Update( const float& fTimeDelta );
	XMFLOAT4X4* GetWorld();
	void		SetOwner( CPlayer* pPlayer );
public:
	void		Render( ID3D11DeviceContext* pContext );
	DWORD		Release( void );
private:
	CPlayer*	m_pOwner;
	XMFLOAT3	m_vScale;
};


#endif // Crown_h__