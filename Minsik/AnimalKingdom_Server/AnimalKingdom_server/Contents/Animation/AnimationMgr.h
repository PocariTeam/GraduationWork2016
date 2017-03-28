#pragma once

#ifndef AnimationMgr_h__
#define AnimationMgr_h__

class CAnimator;
class CAnimationMgr
	: public Singleton<CAnimationMgr>
{
public:
	CAnimator* Clone( const S_CHARACTER& eType );
	CAnimator* Find( const S_CHARACTER& eType );
public:
	HRESULT Load( const char* pFilePath );
	DWORD   Release( void );
private:
	HRESULT Add( const char* pFilePath );
	HRESULT Add( S_CHARACTER eCharacterType, STATE eState, const char* pFilePath );
private:
	vector<CAnimator*>	m_vecAnimator;
};

#endif // AnimationMgr_h__
