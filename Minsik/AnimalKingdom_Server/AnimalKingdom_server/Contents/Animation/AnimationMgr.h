#pragma once

#ifndef AnimationMgr_h__
#define AnimationMgr_h__

class CAnimator;
class CAnimationMgr
	: public Singleton<CAnimationMgr>
{
public:
	~CAnimationMgr();
	CAnimator* Clone( const CHARACTER& eType );
	CAnimator* Find( const CHARACTER& eType );
public:
	HRESULT Load( const char* pFilePath );
	DWORD   Release( void );
private:
	HRESULT Add( const char* pFilePath );
	HRESULT Add( CHARACTER eCharacterType, STATE eState, const char* pFilePath );
private:
	vector<CAnimator*>	m_vecAnimator;
};

#endif // AnimationMgr_h__
