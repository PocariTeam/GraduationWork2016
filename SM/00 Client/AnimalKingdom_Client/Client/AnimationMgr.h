#pragma once

#ifndef AnimationMgr_h__
#define AnimationMgr_h__

#include "Singleton.h"

template <class CHAR_TYPE>
class CAnimationController;
class CAnimationMgr
	: public CSingleton<CAnimationMgr>
{
public:
	enum eCharacter_Type { CHARACTER_CHM, CHARACTER_MON, CHARACTER_END };
	using CHAR_TYPE = eCharacter_Type;
public:
	template <class CHAR_TYPE>
	CAnimationController<CHAR_TYPE>*	Clone( const eCharacter_Type& eType );
	template <class CHAR_TYPE>
	CAnimationController<CHAR_TYPE>*	Find( const eCharacter_Type& eType );
public:
	HRESULT Load( ID3D11Device* pDevice, const char* pFilePath );
	DWORD   Release( void );
private:
	vector<CAnimationController<CHAR_TYPE>*>	m_vecAnimationController;
};

#endif // AnimationMgr_h__
