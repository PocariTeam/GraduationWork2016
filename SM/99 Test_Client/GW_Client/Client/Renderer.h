#pragma once
#ifndef _RENDERER_H_
#define _RENDERER_H_

class CShader;
class CRenderer
{
public:
	enum RENDERGROUP { RENDER_BACKGROUND, RENDER_DEPTHTEST, RENDER_ALPHA, RENDER_UI, RENDER_END };
private:
	CRenderer( void ) = default;
	~CRenderer( void ) = default;
private:
	static CRenderer* m_pInstance;
public:
	static CRenderer* GetInstance( void );
	static void		  DestroyInstance( void );
public:
	HRESULT Add_RenderGroup( RENDERGROUP eGroup, CShader* pShader );
	HRESULT Copy_RenderGroup( const list<CShader*>* pRenderGroup );
	HRESULT Initialize( ID3D11Device* pDevice );
	void Render( ID3D11DeviceContext* pContext );
	void Clear_RenderGroup( void );
private:
	using SHADERLIST = list<CShader*>;
	SHADERLIST			m_RenderGroup[RENDER_END];
private:
	ID3D11Device*		m_pDevice{ nullptr };
public:
	static bool	m_bWireFrame;
private:
	void Render_Background( ID3D11DeviceContext* pContext );
	void Render_DepthTest( ID3D11DeviceContext* pContext );
	void Render_Alpha( ID3D11DeviceContext* pContext );
	void Render_UI( ID3D11DeviceContext* pContext );
public:
	void Release( void );
};

#endif