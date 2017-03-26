#pragma once

#ifndef ThreeD_UI_h__
#define ThreeD_UI_h__

#include "UserInterface.h"

class CThreeD_UI
	: public CUserInterface
{
private:
	explicit CThreeD_UI();
	virtual ~CThreeD_UI();
private:
	HRESULT		Initialize( const XMFLOAT4& vOffset );
public:
	static CThreeD_UI*	Create( const XMFLOAT4& vOffset );
public:
	void Render( ID3D11DeviceContext* pContext );
	int Update( const float& fTimeDelta );
	DWORD Release( void );
public:
	void SetMesh( CMesh* pMesh ) { m_pMesh = pMesh; }
};

#endif // ThreeD_UI_h__