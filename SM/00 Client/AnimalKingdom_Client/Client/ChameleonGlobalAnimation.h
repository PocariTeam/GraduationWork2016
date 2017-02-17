#pragma once
#ifndef ChameleonGlobalAnimation_h__
#define ChameleonGlobalAnimation_h__

#include "Animation.h"

class CChameleon;
class CChameleonGlobalAnimation
	: public CAnimation<CChameleon>
{
private:
	explicit CChameleonGlobalAnimation();
	explicit CChameleonGlobalAnimation( const CChameleonGlobalAnimation& );
	CChameleonGlobalAnimation& operator=( const CChameleonGlobalAnimation& );
	virtual ~CChameleonGlobalAnimation();

public:
	virtual void Enter( CChameleon* pChameleon, const float& fTimeDelta );
	virtual void Execute( CChameleon* pChameleon, const float& fTimeDelta );
	virtual void Exit( CChameleon* pChameleon, const float& fTimeDelta );
};

#endif // ChameleonGlobalAnimation_h__