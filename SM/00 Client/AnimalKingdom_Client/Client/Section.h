#pragma once

#ifndef Section_h__
#define Section_h__

#include "GameObject.h"

class CSection
	: public CGameObject
{
private:
	explicit CSection();
	virtual ~CSection();
private:
	HRESULT		Initialize( ID3D11Device* pDevice, CMesh* pMesh, CTexture* pTexture, XMFLOAT3& vPos, XMFLOAT3& vScale );
public:
	static CSection* Create( ID3D11Device* pDevice, CMesh* pMesh, CTexture* pTexture, XMFLOAT3& vPos, XMFLOAT3& vScale );
public:
	void	Render( ID3D11DeviceContext* pContext );
	DWORD	Release( void );
	bool	Check_InPlane( XMFLOAT3 vPos );
private:
	XMFLOAT3	m_vPos;
	XMFLOAT3	m_vScale;
	XMFLOAT4	m_vPlane[ 6 ];
};


#endif // Section_h__