#pragma once

#ifndef ChameleonRunAnimation_h__
#define ChameleonRunAnimation_h__

#include "Animation.h"

class CChameleon;
class CChameleonRunAnimation
	: public CAnimation<CChameleon>
{
private:
	explicit CChameleonRunAnimation();
	explicit CChameleonRunAnimation( const CChameleonRunAnimation& );
	CChameleonRunAnimation& operator=( const CChameleonRunAnimation& );
	~CChameleonRunAnimation();

public:
	virtual void Enter( CChameleon* pChameleon, const float& fTimeDelta );
	virtual void Execute( CChameleon* pChameleon, const float& fTimeDelta );
	virtual void Exit( CChameleon* pChameleon, const float& fTimeDelta );
};

#endif // ChameleonRunAnimation_h__