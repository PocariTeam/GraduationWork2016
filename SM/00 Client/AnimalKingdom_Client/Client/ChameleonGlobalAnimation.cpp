#include "stdafx.h"
#include "ChameleonGlobalAnimation.h"


CChameleonGlobalAnimation::CChameleonGlobalAnimation()
	: CAnimation<CChameleon>()
{
}

CChameleonGlobalAnimation& CChameleonGlobalAnimation::operator=( const CChameleonGlobalAnimation& )
{
	return *this;
}

CChameleonGlobalAnimation::~CChameleonGlobalAnimation()
{
}

void CChameleonGlobalAnimation::Enter( CChameleon* pChameleon, const float& fTimeDelta )
{
}

void CChameleonGlobalAnimation::Execute( CChameleon* pChameleon, const float& fTimeDelta )
{
}

void CChameleonGlobalAnimation::Exit( CChameleon* pChameleon, const float& fTimeDelta )
{
}
