#pragma once
#include "Tool_Header.h"

class CAnimationSet;
class CAnimation_Controller
{
private:
	CAnimation_Controller();
	~CAnimation_Controller();
public:
	static CAnimation_Controller* Create(ID3D11Device* pDeivce, const char* pAnimationName, vector<JOINT> vecJoint, DWORD dwFrameCnt, DWORD dwTickPerSecond, DWORD dwAnimationLength );
private:
	HRESULT Initialize( ID3D11Device* pDevice, const char* pAnimationName, vector<JOINT> vecJoint, DWORD dwFrameCnt, DWORD dwTickPerSecond, DWORD dwAnimationLength );
public:
	void	CreateAnimationSet( vector<JOINT> vecJoint, const char* pName, DWORD dwStart, DWORD dwLast );
private:
	void    CreateConstantBuffer(ID3D11Device* pDevice, DWORD dwFrameCnt );
	void	SetConstantBuffer(ID3D11DeviceContext* pContext);
public:
	void Animate(float fTimeDelta);
	void Render(ID3D11DeviceContext* pContext);
	void Release();
public:
	DWORD			m_dwAnimationTickPerSecond;
	int				m_iAnimationSetCnt;
	CAnimationSet*	m_pActiveAnimationSet;
	vector<CAnimationSet*> m_vecAnimationSet;
	ID3D11Buffer*	m_pCBAnimate;
};

