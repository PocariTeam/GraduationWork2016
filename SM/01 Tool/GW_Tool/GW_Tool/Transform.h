#pragma once
#include "Tool_Header.h"

class CTransform
{
private:
	CTransform();
	~CTransform();
public:
	static CTransform* Create( ID3D11Device* pDevice );
private:
	HRESULT	Initialize( ID3D11Device* pDevice );
public:
	void	Update( float fTimeDelta );
	void	Render( ID3D11DeviceContext* pContext );
	void	Release();
private:
	ID3D11Buffer*	m_pCBWorld;
	XMFLOAT4X4A		m_mtxWorld;
	float			m_fAngle;
};

