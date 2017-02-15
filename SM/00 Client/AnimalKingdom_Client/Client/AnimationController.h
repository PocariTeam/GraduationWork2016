#pragma once

#ifndef AnimationController_h__
#define AnimationController_h__

#include "Enum.h"

template<class CHAR_TYPE>
class CAnimationController
{
private:
	explicit CAnimationController( CHAR_TYPE eOwner );
	~CAnimationController();
public:
	void Update( const float& fTimeDelta );
private:
	CHAR_TYPE	m_eOwner;

};

#endif // AnimationController_h__
