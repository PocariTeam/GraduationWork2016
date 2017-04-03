#pragma once

#ifndef AnimationMgr_h__
#define AnimationMgr_h__

#include "Singleton.h"
#include "Protocol.h"

class CGameObject;
class CAnimator;
class CAnimationMgr
	: public CSingleton<CAnimationMgr>
{
public:
	CAnimator* Clone( const CHARACTER& eType );
	CAnimator* Find( const CHARACTER& eType );
public:
	HRESULT Load( ID3D11Device* pDevice, const char* pFilePath );
	DWORD   Release( void );
private:
	HRESULT Add( ID3D11Device* pDevice, const char* pFilePath );
	HRESULT Add( CHARACTER eCharacterType, STATE eState, const char* pFilePath );
private:
	vector<CAnimator*>	m_vecAnimator;
};

#endif // AnimationMgr_h__
