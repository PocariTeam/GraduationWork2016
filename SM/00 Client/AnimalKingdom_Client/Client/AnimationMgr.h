#pragma once

#ifndef AnimationMgr_h__
#define AnimationMgr_h__

#include "Singleton.h"
#include "Enum.h"

class CGameObject;
class CAnimationController;
class CAnimationMgr
	: public CSingleton<CAnimationMgr>
{
public:
	enum eCharacter_Type { CHARACTER_CHM, CHARACTER_MON, CHARACTER_END };
	using CHARACTER_TYPE = eCharacter_Type;
public:
	CAnimationController* Clone( const eCharacter_Type& eType, CGameObject* pOwner );
	CAnimationController* Find( const eCharacter_Type& eType );
public:
	HRESULT Load( ID3D11Device* pDevice, const char* pFilePath );
	DWORD   Release( void );
private:
	HRESULT Add( CHARACTER_TYPE eCharacterType, STATE eState, const char* pFilePath );
	HRESULT Add( ID3D11Device* pDevice, const char* pFilePath );
private:
	vector<CAnimationController*>	m_vecAnimationController;
};

#endif // AnimationMgr_h__
