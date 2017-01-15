#pragma once
#include "Tool_Header.h"

class CAnimationSet
{
private:
	CAnimationSet();
	~CAnimationSet() = default;
public:
	static CAnimationSet* Create( vector<JOINT> vecJoint, const char* strName, int iStart, int iLast );
private:
	HRESULT	Initialize( vector<JOINT> vecJoint, const char* strName, int iStart, int iLast );
public:
	void Animate( float fTimedelta );
public:
	void Release( void );
public:
	char			m_strName[ MAX_PATH ]{ "" };
	float			m_fTimePos{ 0.f };
	float			m_fSpeed{ 1.f };
	int				m_iLength{ 0 };
	vector<JOINT>	m_vecJoint;
};

