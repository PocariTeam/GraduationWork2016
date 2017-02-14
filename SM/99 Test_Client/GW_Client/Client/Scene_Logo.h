#pragma once

#ifndef _Scene_Logo_H_
#define _Scene_Logo_H_

#include "Scene.h"

class CLoader;
class CScene_Logo
	: public CScene
{
private:
	virtual HRESULT Initialize( HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext );
public:
	virtual int Update( const float& fTimeDelta );
	virtual void Render( ID3D11DeviceContext* pContext );
	virtual void Release( void );

public:
	static CScene* Create( HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext );

private:
	CScene_Logo() = default;
	virtual ~CScene_Logo() = default;
private:
	CLoader*	m_pLoader{ nullptr };
};

#endif