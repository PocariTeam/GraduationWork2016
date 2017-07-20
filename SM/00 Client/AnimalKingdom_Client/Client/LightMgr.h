#pragma once

#ifndef LightMgr_h__
#define LightMgr_h__

#include "SingleTon.h"
#include "Struct.h"

class CLightMgr
	: public CSingleton<CLightMgr>
{
public:
	enum eLight_Type { LIGHT_DIRECTIONAL, LIGHT_POINT, LIGHT_SPOT };
	using LIGHT_TYPE = eLight_Type;
public:
	HRESULT Initialize( ID3D11Device* pDevice, DWORD dwLightingCnt = 1 );
	DWORD	Release( void );
	LIGHT* Add( ID3D11Device* pDevice, LIGHT_TYPE eType, const XMFLOAT4& vDiffuse, const XMFLOAT4& vOption );
private:
	HRESULT	CreateConstantBuffer( ID3D11Device* pDevice );
	HRESULT CreateConstantBuffer_Shadow( ID3D11Device* pDevice, XMFLOAT3& vLightDir );
	void	SetConstantBuffer( ID3D11DeviceContext* pContext );
	void	SetConstantBuffer_Shadow( ID3D11DeviceContext* pContext );
	void	AssembleLightArr( LIGHT* pNewLighting );
public:
	void	Render( ID3D11DeviceContext* pContext );
private:
	LIGHT**					m_pArrLight;
	XMFLOAT4X4				m_mtxShadowTransform;
	DWORD					m_dwLightingCnt;
	ID3D11Buffer*			m_pConstantBufferLight;
	ID3D11Buffer*			m_pConstantBufferShadow;
};


#endif // LightMgr_h__