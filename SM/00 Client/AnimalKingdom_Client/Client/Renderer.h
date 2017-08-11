#pragma once

#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "Singleton.h"
#include "Enum.h"

class CShader;
class CGameObject;
class CRenderer
	: public CSingleton<CRenderer>
{
public:
	HRESULT Add_RenderGroup( RENDERGROUP eGroup, CShader* pShader );
	HRESULT Add_RenderGroup_Alpha( CGameObject* pGameObject );
	HRESULT Copy_RenderGroup( list<CShader*>* pRenderGroup );
	HRESULT Initialize( ID3D11Device* pDevice );
	void Update( const float& fTimeDelta );
	void Render( ID3D11DeviceContext* pContext );
	void Clear_AlphaObject( void );
	void SetWireframe( void );
	void SetRenderTargetDebug( void );
	void InCave( void );
	void OutCave( void );
private:
	using SHADERLIST = list<CShader*>;
	SHADERLIST*			m_pRenderGroup{ nullptr };
public:
	static bool			m_bWireFrame;
	static bool			m_bRenderTargetDebug;
	static bool			m_bInCave;
public:
	void Render_ShadowMap( ID3D11DeviceContext* pContext );
	void Render_Background( ID3D11DeviceContext* pContext );
	void Render_DepthTest( ID3D11DeviceContext* pContext );
	void Render_OutCave( ID3D11DeviceContext* pContext );
	void Render_InCave( ID3D11DeviceContext* pContext );
	void Render_Light( ID3D11DeviceContext* pContext );
	void Render_Blend( ID3D11DeviceContext* pContext );
	void Render_Alpha( ID3D11DeviceContext* pContext );
	void Render_UI( ID3D11DeviceContext* pContext );
	void Render_RenderTargetDebug( ID3D11DeviceContext* pContext );
public:
	DWORD Release( void );
private:
	list<CGameObject*>	m_listAlphaObject{};
};

#endif