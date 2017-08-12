#pragma once

#ifndef EffectMgr_h__
#define EffectMgr_h__

#include "Singleton.h"
#include "Struct.h"

class CPlayer;
class CShader;
class CEffectObject;
class CEffectMgr
	: public CSingleton<CEffectMgr>
{
public:
	enum eEffect_Type	{ EFFECT_TRAIL, EFFECT_DUMMY, /*EFFECT_FIRE, */EFFECT_END };
	using EFFECT_TYPE = eEffect_Type;
public:
	HRESULT	Initialize( ID3D11Device* pDevice );
	DWORD	Release( void );
	CEffectObject* Add( EFFECT_TYPE eType, const XMFLOAT4X4& mtxWorld, const XMFLOAT2& vSize, const float fAge, XMFLOAT3& vVelocity = XMFLOAT3{ 0.f, 0.f, 0.f } );
	CEffectObject* Add( EFFECT_TYPE eType, CPlayer* pOwner );
private:
	ID3D11Device*	m_pDevice;
};

#endif // EffectMgr_h__