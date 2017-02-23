#pragma once

#ifndef Jungle_h__
#define Jungle_h__

#include "Scene.h"

/* Test 170210 */
class CShader;
class CTerrain;
class CWallpaper;
class CPlayer;
/*              */
class CGameObject;
class CJungle
	: public CScene
{
private:
	CJungle();
	virtual ~CJungle();
private:
	HRESULT Initialize( HWND hWnd, ID3D11Device* pDevice );
public:
	int Update( const float& fTimeDelta );
	DWORD Release( void );
	void Render( ID3D11DeviceContext* pContext );
private:
	/* Test 170210 */
	CShader*	m_pShader;
	CTerrain*	m_pTerrain;
	/* Test 170213 */
	CShader*	m_pLightShader;
	CWallpaper*	m_pScreen;
	CShader*	m_pBlendShader;
	CShader*	m_pDebugShader;
	/* Test 170222 */
	CPlayer*	m_pPlayer;
	CShader*	m_pAnimateShader;
public:
	static CScene* Create( HWND hWnd, ID3D11Device* pDevice );
};

#endif // Jungle_h__