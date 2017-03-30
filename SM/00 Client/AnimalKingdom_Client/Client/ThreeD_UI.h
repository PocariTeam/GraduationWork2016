#pragma once

#ifndef ThreeD_UI_h__
#define ThreeD_UI_h__

#include "UserInterface.h"

class CAnimator;
class CThreeD_UI
	: public CUserInterface
{
private:
	explicit CThreeD_UI();
	virtual ~CThreeD_UI();
private:
	HRESULT		Initialize( CMesh* pMesh, CTexture* pTexture, CAnimator* pAnimator, const XMFLOAT4& vPosSize );
public:
	static CThreeD_UI*	Create( CMesh* pMesh, CTexture* pTexture, CAnimator* pAnimator, const XMFLOAT4& vPosSize );
public:
	void Render( ID3D11DeviceContext* pContext );
	int Update( const float& fTimeDelta );
	DWORD Release( void );
public:
	void SetMesh( CMesh* pMesh );
	void SetAnimator( CAnimator* pAnimator );
	XMFLOAT4X4 GetWorld();
private:
	bool			m_bHide;
	CAnimator*		m_pAnimator;
public:
	void Hide() { m_bHide = true; }
	void Show() { m_bHide = false; }
};

#endif // ThreeD_UI_h__