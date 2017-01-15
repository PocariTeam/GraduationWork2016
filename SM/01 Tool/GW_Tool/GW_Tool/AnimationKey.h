#pragma once

typedef struct tagTimedFloatKeys
{
	float  m_fTime;
	int    m_iValueCnt;  // ��쿡 ���� �Ǽ��� ���� �ٲ�� ������ 
	float* m_pValues;
}TIMEDFLOATKEYS;

class CAnimationKey
{
public:
	CAnimationKey();
	~CAnimationKey();
private:
	int	m_iKeyframeCnt;	// Ű������ ��
	TIMEDFLOATKEYS* m_pKeys;
};

