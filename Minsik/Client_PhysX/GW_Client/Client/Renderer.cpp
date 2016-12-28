#include "stdafx.h"
#include "Renderer.h"
#include "Shader.h"
#include "Function.h"
#include "RenderState.h"

CRenderer*	CRenderer::m_pInstance = nullptr;
bool		CRenderer::m_bWireFrame = false;

CRenderer*	CRenderer::GetInstance()
{
	if( nullptr == m_pInstance )
		m_pInstance = new CRenderer;

	return m_pInstance;
}
void CRenderer::DestroyInstance()
{
	if( nullptr != m_pInstance )
	{
		m_pInstance->Release();
		m_pInstance = nullptr;
	}
}

void CRenderer::Clear_RenderGroup( void )
{
	for( int i = 0; i < RENDER_END; ++i ) {
		if( m_RenderGroup[i].empty() ) continue;
		auto iter = m_RenderGroup[i].begin();
		for( ; iter != m_RenderGroup[i].end(); )
			iter = m_RenderGroup[i].erase( iter );

		m_RenderGroup[i].clear();
	}
}

void CRenderer::Release( void )
{
	Clear_RenderGroup();

	delete this;
}

HRESULT CRenderer::Add_RenderGroup( RENDERGROUP eGroup, CShader* pShader )
{
	if( NULL == pShader )
		return E_FAIL;

	if( eGroup >= RENDER_END )
		return E_FAIL;

	m_RenderGroup[eGroup].push_back( pShader );

	return S_OK;
}

HRESULT CRenderer::Copy_RenderGroup( const list<CShader*>* pRenderGroup )
{
	if( NULL == pRenderGroup )
		return E_FAIL;

	for( int i = 0; i < RENDER_END; ++i )
		m_RenderGroup[i] = pRenderGroup[i];
		
	return S_OK;
}

HRESULT CRenderer::Initialize( ID3D11Device* pDevice )
{
	m_pDevice = pDevice;

	return S_OK;
}

void CRenderer::Render( ID3D11DeviceContext* pContext )
{
	Render_Background( pContext );
	Render_DepthTest( pContext );
	Render_Alpha( pContext );
	Render_UI( pContext );
}

void CRenderer::Render_Background( ID3D11DeviceContext* pContext )
{
	// 스카이박스 전용 : 그 어떤 객체들 보다도 먼저 그려져야하는 객체들

	// 깊에버퍼에 값은 기록하지만, 깊이버퍼에 의한 정렬을 수행하지않고 무조건 덮어서 그린다.
	// 깊이버퍼에 값을 기록하지않고 그린다.
	if( m_bWireFrame )	CRenderState::Set_Rasterize( pContext, CRenderState::RS_WIREFRAME );
	else CRenderState::Set_Rasterize( pContext, CRenderState::RS_NULL );
	CRenderState::Set_BlendState( pContext, CRenderState::BL_NULL );
	CRenderState::Set_DepthStencilState( pContext, CRenderState::DS_NO_WRITE );

	// 깊이값을 저장하기위한 변수
	SHADERLIST::iterator	iter = m_RenderGroup[RENDER_BACKGROUND].begin();
	SHADERLIST::iterator	iter_end = m_RenderGroup[RENDER_BACKGROUND].end();

	for( ; iter != iter_end; ++iter )
	{
		( *iter )->Render( pContext );
	}
}

void CRenderer::Render_DepthTest( ID3D11DeviceContext* pContext )
{
	// 깊이버퍼에 의한 정렬을 수행.
	// 깊이버퍼에 값을 기록한다.
	CRenderState::Set_DepthStencilState( pContext, CRenderState::DS_NULL );

	SHADERLIST::iterator	iter = m_RenderGroup[RENDER_DEPTHTEST].begin();
	SHADERLIST::iterator	iter_end = m_RenderGroup[RENDER_DEPTHTEST].end();

	for( ; iter != iter_end; ++iter )
	{
		( *iter )->Render( pContext );
	}
}

void CRenderer::Render_Alpha( ID3D11DeviceContext* pContext )
{
	// 블렌딩 설정한다.
	// Z값에 따른 소팅을 수행한다.
	CRenderState::Set_BlendState( pContext, CRenderState::BL_ALPHA );
	CRenderState::Set_DepthStencilState( pContext, CRenderState::DS_NO_WRITE );

	SHADERLIST::iterator	iter = m_RenderGroup[RENDER_ALPHA].begin();
	SHADERLIST::iterator	iter_end = m_RenderGroup[RENDER_ALPHA].end();

	for( ; iter != iter_end; ++iter )
	{
		( *iter )->Render( pContext );
	}

	// 블렌딩 설정해제
	CRenderState::Set_BlendState( pContext, CRenderState::BL_NULL );
	CRenderState::Set_DepthStencilState( pContext, CRenderState::DS_NULL );
}

void CRenderer::Render_UI( ID3D11DeviceContext* pContext )
{
	// 깊에버퍼에 값은 기록하지만, 깊이버퍼에 의한 정렬을 수행하지않고 무조건 덮어서 그린다.
	// 깊이버퍼에 값을 기록하지않고 그린다.

	SHADERLIST::iterator	iter = m_RenderGroup[RENDER_UI].begin();
	SHADERLIST::iterator	iter_end = m_RenderGroup[RENDER_UI].end();

	for( ; iter != iter_end; ++iter )
	{
		( *iter )->Render( pContext );
	}

	// 깊이버퍼에 의한 정렬을 수행.
	// 깊이버퍼에 값을 기록한다.
}