#pragma once

#ifndef Skill_UI_h__
#define Skill_UI_h__

#include "UserInterface.h"

class CSkill_UI
	: public CUserInterface
{
private:
	explicit CSkill_UI();
	virtual ~CSkill_UI();
private:
	HRESULT		Initialize( CTexture* pTexture, const XMFLOAT4& vPosSize, float* pData, CHARACTER eType, float fSpeed );
public:
	static CSkill_UI*	Create( CTexture* pTexture, const XMFLOAT4& vPosSize, float* pData, CHARACTER eType, float fSpeed = 2.f );
public:
	void Render( ID3D11DeviceContext* pContext );
	int Update( const float& fTimeDelta );
	DWORD Release( void );
public:
	void Hide() { m_bHide = true; }
	void Show() { m_bHide = false; }
private:
	bool	m_bHide;
	float*	m_pData;
	float	m_fSpeed;
	CHARACTER	m_eCharacterType;
};

#endif // Skill_UI_h__