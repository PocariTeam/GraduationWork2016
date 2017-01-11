#pragma once

class CAnimationKey;
class CAnimation
{
public:
	CAnimation();
	~CAnimation();
private:
	char			m_strFrameName[MAX_PATH];
	int				m_iAnimationKeyCnt;
	CAnimationKey*  m_pAnimationKeys;
};

