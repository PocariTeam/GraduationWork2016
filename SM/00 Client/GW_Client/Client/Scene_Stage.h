#pragma once

#ifndef _Scene_Stage_H_
#define _Scene_Stage_H_


#include "Scene.h"

class CPhysicsMgr;
class CShader;
class CScene_Stage
	: public CScene
{
public:
	enum RENDERGROUP { RENDER_BACKGROUND, RENDER_DEPTHTEST, RENDER_ALPHA, RENDER_UI, RENDER_MIRROR, RENDER_END };
private:
	virtual HRESULT Initialize( HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext );
public:
	virtual int Update( const float& fTimeDelta );
	virtual void Render( ID3D11DeviceContext* pContext );
	virtual void Release( void );
private:
	void	Create_BaseObject( ID3D11Device* pDevice, ID3D11DeviceContext* pContext );
public:
	static CScene* Create( HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext );
private:
	CScene_Stage() = default;
	virtual ~CScene_Stage() = default;
private:
	CPhysicsMgr*		m_pPhysicsMgr;
	using SHADERLIST = list<CShader*>;
	SHADERLIST			m_RenderGroup[RENDER_END];
	bool				m_bOverlapped[2];
};

#endif