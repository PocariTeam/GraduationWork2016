#pragma once

#ifndef Bar_UI_h__
#define Bar_UI_h__

#include "UserInterface.h"

class CBar_UI
	: public CUserInterface
{
private:
	explicit CBar_UI();
	virtual ~CBar_UI();
private:
	HRESULT		Initialize( CTexture* pTexture, const XMFLOAT4& vPosSize, float* pDest, int iMax, float fSpeed );
public:
	static CBar_UI*	Create( CTexture* pTexture, const XMFLOAT4& vPosSize, float* pDest, int iMax, float fSpeed = 2.f );
public:
	void Render( ID3D11DeviceContext* pContext );
	int Update( const float& fTimeDelta );
	DWORD Release( void );
public:
	void Hide() { m_bHide = true; }
	void Show() { m_bHide = false; }
private:
	bool	m_bHide;
	float	m_fSpeed;
	int		m_iMax;
	float	m_iPrevious;
	float*	m_pData;
};

#endif // Bar_UI_h__