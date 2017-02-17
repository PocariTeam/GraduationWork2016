#pragma once

#ifndef Animation_h__
#define Animation_h__

#include "Enum.h"
#include "Singleton.h"

template<class pCharacter_Type>
class CAnimation
	: public CSingleton<CAnimation<pCharacter_Type>>
{
protected:
	virtual ~CAnimation() {};
public:
	virtual void Enter( pCharacter_Type* pOwner, const float& fTimeDelta )PURE;
	virtual void Execute( pCharacter_Type* pOwner, const float& fTimeDelta )PURE;
	virtual void Exit( pCharacter_Type* pOwner, const float& fTimeDelta )PURE;
public:
	virtual DWORD Release( void );
};

template<class eCharacter_Type>
DWORD CAnimation<eCharacter_Type>::Release( void )
{
	return 0;
}

#endif // Animation_h__