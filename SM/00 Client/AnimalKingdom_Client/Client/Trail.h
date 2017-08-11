#pragma once

#ifndef Trail_h__
#define Trail_h__

#include "ParticleObject.h"
#include "Struct.h"

#define TRAIL_LENGTH 31

class CShader;
class CTrail
	: public CParticleObject
{
private:
	explicit CTrail();
	virtual ~CTrail();
private:
	HRESULT		Initialize( ID3D11Device* pDevice, const XMFLOAT4X4& mtxWorld, const XMFLOAT2& vSize, const float fAge, const XMFLOAT3& vPos );
public:
	static CTrail* Create( ID3D11Device* pDevice, const XMFLOAT4X4& mtxWorld, const XMFLOAT2& vSize, const float fAge, const XMFLOAT3& vPos );
public:
	void			UpdateParticleValue( const XMFLOAT4X4& mtxWorld );
	void			SetParticleValue( const XMFLOAT4X4& mtxWorld );
	XMFLOAT4X4*		GetWorld();
	LPVOID			GetParticleData();
	UINT			GetParticleDataCnt() { return m_iTrailCnt; }
private:
	HRESULT			CreateConstantBuffer( ID3D11Device* pDevice );
	void			SetConstantBuffer( ID3D11DeviceContext* pContext );
public:
	int		Update( const float& fTimeDelta );
	void	Render( ID3D11DeviceContext* pContext );
	DWORD	Release( void );
private:
	ID3D11Buffer*	m_pConstantBuffer;
	CShader*		m_pShader;
	CB_PARTICLE		m_tParticleInfo[ TRAIL_LENGTH ];
	UINT			m_iTrailCnt;
};


#endif // Trail_h__