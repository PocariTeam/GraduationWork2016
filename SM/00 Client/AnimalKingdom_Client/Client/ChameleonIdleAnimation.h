#pragma once

#ifndef ChameleonIdleAnimation_h__
#define ChameleonIdleAnimation_h__

#include "Animation.h"

class CChameleon;
class CChameleonIdleAnimation
	: public CAnimation<CChameleon>
{
private:
	explicit CChameleonIdleAnimation();
	explicit CChameleonIdleAnimation( const CChameleonIdleAnimation& );
	CChameleonIdleAnimation& operator=( const CChameleonIdleAnimation& );
	~CChameleonIdleAnimation();

public:
	virtual void Enter( CChameleon* pChameleon, const float& fTimeDelta );
	virtual void Execute( CChameleon* pChameleon, const float& fTimeDelta );
	virtual void Exit( CChameleon* pChameleon, const float& fTimeDelta );
};

#endif // ChameleonIdleAnimation_h__
