#pragma once

#ifndef Scan_h__
#define Scan_h__

#include "EffectObject.h"
#include "Struct.h"

class CShader;
class CPlayer;
class CScan
	: public CEffectObject
{
private:
	explicit CScan();
	virtual ~CScan();
private:
	HRESULT		Initialize( ID3D11Device* pDevice, CPlayer* pOwner );
public:
	static CScan* Create( ID3D11Device* pDevice, CPlayer* pOwner );
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


#endif // Scan_h__