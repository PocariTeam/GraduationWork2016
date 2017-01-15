#pragma once

typedef struct tagTimedFloatKeys
{
	float  m_fTime;
	int    m_iValueCnt;  // 경우에 따라 실수의 수가 바뀌기 때문에 
	float* m_pValues;
}TIMEDFLOATKEYS;

class CAnimationKey
{
public:
	CAnimationKey();
	~CAnimationKey();
private:
	int	m_iKeyframeCnt;	// 키프레임 수
	TIMEDFLOATKEYS* m_pKeys;
};

