#pragma once

#ifndef Player_h__
#define Player_h__

#include "Enum.h"
#include "GameObject.h"

class CPlayer
	: public CGameObject
{
private:
	explicit CPlayer();
	virtual ~CPlayer() = default;
public:
	/*virtual int Update( const float& fTimeDelta );
	virtual void Render( ID3D11DeviceContext* pContext )PURE;
	virtual DWORD Release( void );
	virtual void CreateConstantBuffer( ID3D11Device* pDevice );
	virtual void SetConstantBuffer( ID3D11DeviceContext* pContext );*/
protected:
};

#endif // Player_h__