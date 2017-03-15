#pragma once

#ifndef Skybox_h__
#define Skybox_h__

#include "GameObject.h"

class CMesh;
class CSkybox
	: public CGameObject
{
private:
	explicit CSkybox();
	virtual ~CSkybox();
private:
	HRESULT		Initialize( CMesh* pMesh, CTexture* pTexture );
public:
	static	CSkybox*	Create( CMesh* pMesh, CTexture* pTexture );
public:
	XMFLOAT4X4	GetWorld();
	void	Render( ID3D11DeviceContext* pContext );
	DWORD	Release( void );
};

#endif // Skybox_h__
