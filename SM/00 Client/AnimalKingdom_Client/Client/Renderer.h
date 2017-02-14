#pragma once

#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "Singleton.h"

class CShader;
class CRenderer
	: public CSingleton<CRenderer>
{
public:
	enum RENDERGROUP { RENDER_BACKGROUND, RENDER_DEPTHTEST, RENDER_LIGHT, RENDER_BLEND, RENDER_ALPHA, RENDER_UI, RENDER_END };
public:
	HRESULT Add_RenderGroup( RENDERGROUP eGroup, CShader* pShader );
	HRESULT Copy_RenderGroup( const list<CShader*>* pRenderGroup );
	HRESULT Initialize( ID3D11Device* pDevice );
	void Render( ID3D11DeviceContext* pContext );
	void Clear_RenderGroup( void );
	void SetWireframe( void );
private:
	using SHADERLIST = list<CShader*>;
	SHADERLIST			m_RenderGroup[ RENDER_END ];
public:
	static bool			m_bWireFrame;
private:
	void Render_Background( ID3D11DeviceContext* pContext );
	void Render_DepthTest( ID3D11DeviceContext* pContext );
	void Render_Light( ID3D11DeviceContext* pContext );
	void Render_Blend( ID3D11DeviceContext* pContext );
	void Render_Alpha( ID3D11DeviceContext* pContext );
	void Render_UI( ID3D11DeviceContext* pContext );
public:
	DWORD Release( void );
};

#endif