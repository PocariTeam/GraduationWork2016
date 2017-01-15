#pragma once

#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "Tool_Header.h"

class CLight
{
private:
	CLight( void );
	~CLight( void );
private:
	HRESULT Initialize( ID3D11Device* pDevice );
	void	CreateConstantBuffer( ID3D11Device* pDevice, int iLightType );
public:
	static CLight* Create( ID3D11Device* pDevice );
	void	SetConstantBuffer( ID3D11DeviceContext* pContext );
private:
	LIGHT*			m_pLightInfo;
	ID3D11Buffer*	m_pCbVSLight;
	ID3D11Buffer*	m_pCbPSLight;
public:
	void Release( void );
};

#endif