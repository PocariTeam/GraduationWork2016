#pragma once

#ifndef Skydome_h__
#define Skydome_h__

#include "GameObject.h"

class CMesh;
class CSkydome
	: public CGameObject
{
private:
	explicit CSkydome();
	virtual ~CSkydome();
private:
	HRESULT		Initialize( CMesh* pMesh, CTexture* pTexture );
public:
	static	CSkydome*	Create( CMesh* pMesh, CTexture* pTexture );
public:
	int		Update( const float& fTimeDelta );
	void	Render( ID3D11DeviceContext* pContext );
	DWORD	Release( void );
private:
	float	m_fAngle;
};

#endif // Skydome_h__
