#pragma once

#ifndef Environment_h__
#define Environment_h__

#include "GameObject.h"

class CEnvironment
	: public CGameObject
{
private:
	explicit CEnvironment();
	virtual ~CEnvironment();
private:
	HRESULT		Initialize( ID3D11Device* pDevice, NxActor* pActor, CMesh* pMesh, CTexture* pTexture, XMFLOAT3& vScale );
public:
	static CEnvironment* Create( ID3D11Device* pDevice, NxActor* pActor, CMesh* pMesh, CTexture* pTexture, XMFLOAT3& vScale );
public:
	XMFLOAT4X4 GetWorld();
public:
	void	Render( ID3D11DeviceContext* pContext );
	DWORD	Release( void );
private:
	XMFLOAT3	m_vScale;
};


#endif // Environment_h__