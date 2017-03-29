#pragma once

#ifndef Logo_h__
#define Logo_h__

#include "Scene.h"

class CMesh;
class CTexture;
class CShader;
class CThreadMgr;
class CLogo
	: public CScene
{
private:
	CLogo();
	virtual ~CLogo();
private:
	HRESULT Initialize( HWND hWnd, ID3D11Device* pDevice );
public:
	int Update( const float& fTimeDelta );
	DWORD Release( void );
	void Render( ID3D11DeviceContext* pContext );
private:
	CTexture*	m_pTexture;
	CMesh*		m_pMesh;
	CShader*	m_pShader;
	CThreadMgr* m_pThreadMgr;
public:
	static CScene* Create( HWND hWnd, ID3D11Device* pDevice );
};

#endif // Logo_h__


