#pragma once

#ifndef Number_UI_h__
#define Number_UI_h__

#include "UserInterface.h"

class CNumber_UI
	: public CUserInterface
{
private:
	explicit CNumber_UI();
	virtual ~CNumber_UI();
private:
	HRESULT		Initialize( CTexture* pTexture, const XMFLOAT4& vPosSize, UINT iNum, float fSpeed );
public:
	static CNumber_UI*	Create( CTexture* pTexture, const XMFLOAT4& vPosSize, UINT iNum, float fSpeed = 2.f );
public:
	void Render( ID3D11DeviceContext* pContext );
	int Update( const float& fTimeDelta );
	DWORD Release( void );
	void SetNumber( UINT iNum );
public:
	void Hide() { m_bHide = true; }
	void Show() { m_bHide = false; }
private:
	bool	m_bHide;
	float	m_fSpeed;
	UINT	m_iValue;
};

#endif // Number_UI_h__