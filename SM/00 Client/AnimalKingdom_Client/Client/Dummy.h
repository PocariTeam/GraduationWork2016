#pragma once

#ifndef Dummy_h__
#define Dummy_h__

#include "EffectObject.h"
#include "Struct.h"

class CShader;
class CPlayer;
class CDummy
	: public CEffectObject
{
private:
	explicit CDummy();
	virtual ~CDummy();
private:
	HRESULT		Initialize( ID3D11Device* pDevice, CPlayer* pOwner );
public:
	static CDummy* Create( ID3D11Device* pDevice, CPlayer* pOwner );
public:
	XMFLOAT4X4*		GetWorld();
public:
	int		Update( const float& fTimeDelta );
	void	Render( ID3D11DeviceContext* pContext );
	DWORD	Release( void );
private:
	CShader*		m_pShader;
	CPlayer*		m_pOwner;
};


#endif // Dummy_h__