#include "stdafx.h"
#include "Renderer.h"
#include "GameObject.h"
#include "Shader.h"
#include "Function.h"
#include "RenderState.h"
#include "RenderTargetMgr.h"
#include "LightMgr.h"
#include "ParticleMgr.h"
#include "Functor.h"

CRenderer*	CSingleton<CRenderer>::m_pInstance;
bool		CRenderer::m_bWireFrame = false;
bool		CRenderer::m_bRenderTargetDebug = false;
bool		CRenderer::m_bInCave = false;

void CRenderer::SetWireframe( void )
{
	if( m_bWireFrame )
		m_bWireFrame = false;
	else
		m_bWireFrame = true;
}

void CRenderer::SetRenderTargetDebug( void )
{
	if( m_bRenderTargetDebug )
		m_bRenderTargetDebug = false;
	else
		m_bRenderTargetDebug = true;
}

void CRenderer::InCave( void )
{
	m_bInCave = true;
}

void CRenderer::OutCave( void )
{
	m_bInCave = false;
}

void CRenderer::Clear_AlphaObject( void )
{
	for_each( m_listAlphaObject.begin(), m_listAlphaObject.end(), ReleaseElement() );
	m_listAlphaObject.erase( m_listAlphaObject.begin(), m_listAlphaObject.end() );
}

DWORD CRenderer::Release( void )
{
	Clear_AlphaObject();
	CRenderState::Release();

	delete this;

	return 0;
}

HRESULT CRenderer::Add_RenderGroup( RENDERGROUP eGroup, CShader* pShader )
{
	if( NULL == pShader )
		return E_FAIL;

	if( eGroup >= RENDER_END )
		return E_FAIL;

	m_pRenderGroup[ eGroup ].push_back( pShader );

	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup_Alpha( CGameObject* pGameObject )
{
	if( nullptr == pGameObject )
		return E_FAIL;

	m_listAlphaObject.push_back( pGameObject );

	return S_OK;
}

HRESULT CRenderer::Copy_RenderGroup( list<CShader*>* pRenderGroup )
{
	if( nullptr == pRenderGroup )
		return E_FAIL;

	m_pRenderGroup = pRenderGroup;

	return S_OK;
}

HRESULT CRenderer::Initialize( ID3D11Device* pDevice )
{
	CRenderState::Initialize( pDevice );

	return S_OK;
}

void CRenderer::Update( const float& fTimeDelta )
{
	if( m_listAlphaObject.empty() ) return;

	auto Advance_iter = m_listAlphaObject.begin();

	for( ; Advance_iter != m_listAlphaObject.end(); ++Advance_iter )
		( *Advance_iter )->Update( fTimeDelta );
}

void CRenderer::Render( ID3D11DeviceContext* pContext )
{
	if( nullptr == m_pRenderGroup ) return;

	Render_ShadowMap( pContext );

	Render_Background( pContext );
	Render_DepthTest( pContext );
	if( !m_bWireFrame )
	{
		Render_Light( pContext );
		Render_Blend( pContext );
	}
	Render_Alpha( pContext );
	Render_UI( pContext );

	if( m_bRenderTargetDebug && !m_bWireFrame )
		Render_RenderTargetDebug( pContext );
}

void CRenderer::Render_ShadowMap( ID3D11DeviceContext* pContext )
{
	if( m_bWireFrame || m_bInCave )	return;

	CRenderState::Set_BlendState( pContext, CRenderState::BL_NULL );
	CRenderState::Set_Rasterize( pContext, CRenderState::RS_CCW );
	CRenderState::Set_DepthStencilState( pContext, CRenderState::DS_NULL );
	
	ID3D11ShaderResourceView* pNullSRV[ 6 ] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	pContext->PSSetShaderResources( 0, 3, pNullSRV );

	CRenderTargetMgr::GetInstance()->Shadow_Begin( pContext );
	CRenderTargetMgr::GetInstance()->ResetRenderTargetView( pContext );

	SHADERLIST::iterator	iter = m_pRenderGroup[ RENDER_SHADOW ].begin();
	SHADERLIST::iterator	iter_end = m_pRenderGroup[ RENDER_SHADOW ].end();

	for( ; iter != iter_end; ++iter )
		( *iter )->Render( pContext );

	CRenderTargetMgr::GetInstance()->Shadow_End( pContext );
}

void CRenderer::Render_Background( ID3D11DeviceContext* pContext )
{
	// 깊이버퍼에 값을 기록하지않고 그린다.
	if( m_bWireFrame )	CRenderState::Set_Rasterize( pContext, CRenderState::RS_WIREFRAME );
	else CRenderState::Set_Rasterize( pContext, CRenderState::RS_NULL );

	CRenderState::Set_BlendState( pContext, CRenderState::BL_NULL );
	CRenderTargetMgr::GetInstance()->SetRenderTargetView( pContext, CRenderTargetMgr::RT_BACK, 1 );
	CRenderState::Set_DepthStencilState( pContext, CRenderState::DS_NO_TEST );

	SHADERLIST::iterator	iter = m_pRenderGroup[ RENDER_BACKGROUND ].begin();
	SHADERLIST::iterator	iter_end = m_pRenderGroup[ RENDER_BACKGROUND ].end();

	for( ; iter != iter_end; ++iter )
	{
		( *iter )->Render( pContext );
	}
}

void CRenderer::Render_DepthTest( ID3D11DeviceContext* pContext )
{
	CRenderState::Set_DepthStencilState( pContext, CRenderState::DS_NULL );

	SHADERLIST::iterator	iter = m_pRenderGroup[ RENDER_DEPTHTEST ].begin();
	SHADERLIST::iterator	iter_end = m_pRenderGroup[ RENDER_DEPTHTEST ].end();
	
	CRenderTargetMgr::GetInstance()->ResetRenderTargetView( pContext );
	ID3D11ShaderResourceView* pNullSRV[ 6 ] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	pContext->PSSetShaderResources( 0, 3, pNullSRV );
	if( m_bWireFrame )
		CRenderTargetMgr::GetInstance()->SetRenderTargetView( pContext, CRenderTargetMgr::RT_BACK, 3 );
	else
		CRenderTargetMgr::GetInstance()->SetRenderTargetView( pContext, CRenderTargetMgr::RT_NORMAL, 3 );

	for( ; iter != iter_end; ++iter )
	{
		( *iter )->Render( pContext );
	}

	if( !m_bInCave ) Render_OutCave( pContext );
}

void CRenderer::Render_OutCave( ID3D11DeviceContext* pContext )
{
	SHADERLIST::iterator	iter = m_pRenderGroup[ RENDER_OUTCAVE ].begin();
	SHADERLIST::iterator	iter_end = m_pRenderGroup[ RENDER_OUTCAVE ].end();

	for( ; iter != iter_end; ++iter )
	{
		( *iter )->Render( pContext );
	}
}

void CRenderer::Render_InCave( ID3D11DeviceContext* pContext )
{
	SHADERLIST::iterator	iter = m_pRenderGroup[ RENDER_INCAVE ].begin();
	SHADERLIST::iterator	iter_end = m_pRenderGroup[ RENDER_INCAVE ].end();

	for( ; iter != iter_end; ++iter )
	{
		( *iter )->Render( pContext );
	}
}

void CRenderer::Render_Light( ID3D11DeviceContext* pContext )
{
	CRenderState::Set_DepthStencilState( pContext, CRenderState::DS_NO_TEST );
	CRenderState::Set_Rasterize( pContext, CRenderState::RS_SHADOW );
	
	//D3DX11SaveTextureToFile( pContext, CRenderTargetMgr::GetInstance()->GetTexture( CRenderTargetMgr::RT_DEPTH ), D3DX11_IFF_JPG, "Depth.jpg" );
	//D3DX11SaveTextureToFile( pContext, CRenderTargetMgr::GetInstance()->GetTexture( CRenderTargetMgr::RT_ALBEDO ), D3DX11_IFF_JPG, "Albedo.jpg" );
	//D3DX11SaveTextureToFile( pContext, CRenderTargetMgr::GetInstance()->GetTexture( CRenderTargetMgr::RT_NORMAL ), D3DX11_IFF_JPG, "Normal.jpg" );
	CRenderTargetMgr::GetInstance()->ResetRenderTargetView( pContext );
	ID3D11ShaderResourceView* pNullSRV[ 6 ] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	pContext->PSSetShaderResources( 0, 3, pNullSRV );
	CRenderTargetMgr::GetInstance()->SetRenderTargetView( pContext, CRenderTargetMgr::RT_LIGHT, 2 );
	CRenderTargetMgr::GetInstance()->SetShaderResourceView( pContext, 0, CRenderTargetMgr::RT_NORMAL, 2 );
	CRenderTargetMgr::GetInstance()->SetShaderResourceView( pContext, 2, CRenderTargetMgr::RT_END, 1 );

	CLightMgr::GetInstance()->Render( pContext );

	m_pRenderGroup[ RENDER_LIGHT ].front()->Render( pContext );

	CRenderState::Set_Rasterize( pContext, CRenderState::RS_NULL );
}

void CRenderer::Render_Blend( ID3D11DeviceContext* pContext )
{
	CRenderState::Set_DepthStencilState( pContext, CRenderState::DS_NO_TEST );

	// D3DX11SaveTextureToFile( pContext, CRenderTargetMgr::GetInstance()->GetTexture( CRenderTargetMgr::RT_LIGHT ), D3DX11_IFF_JPG, "Light.jpg" );
	// D3DX11SaveTextureToFile( pContext, CRenderTargetMgr::GetInstance()->GetTexture( CRenderTargetMgr::RT_SPECULAR ), D3DX11_IFF_JPG, "Specular.jpg" );
	CRenderTargetMgr::GetInstance()->ResetRenderTargetView( pContext );
	ID3D11ShaderResourceView* pNullSRV[ 6 ] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	pContext->PSSetShaderResources( 0, 3, pNullSRV );
 	CRenderTargetMgr::GetInstance()->SetRenderTargetView( pContext, CRenderTargetMgr::RT_BACK, 1 );
	CRenderTargetMgr::GetInstance()->SetShaderResourceView( pContext, 0, CRenderTargetMgr::RT_ALBEDO, 3 );

	m_pRenderGroup[ RENDER_BLEND ].front()->Render( pContext );
}

void CRenderer::Render_Alpha( ID3D11DeviceContext* pContext )
{
	// 블렌딩 설정한다.
	// Z값에 따른 소팅을 수행한다.
	CRenderState::Set_DepthStencilState( pContext, CRenderState::DS_NULL );
	CRenderState::Set_BlendState( pContext, CRenderState::BL_ALPHA );

	CRenderState::Set_Rasterize( pContext, CRenderState::RS_NO_CULL );
	// CParticleMgr::GetInstance()->Render( pContext );
	if( m_bWireFrame )	CRenderState::Set_Rasterize( pContext, CRenderState::RS_WIREFRAME );
	else CRenderState::Set_Rasterize( pContext, CRenderState::RS_NULL );

	if( !m_listAlphaObject.empty() )
	{
		m_listAlphaObject.sort( []( CGameObject*& a, CGameObject*& b ) { return a->GetWorld()->_34 > b->GetWorld()->_34; } );

		auto Advance_iter = m_listAlphaObject.begin();

		for( ; Advance_iter != m_listAlphaObject.end(); ++Advance_iter )
			( *Advance_iter )->Render( pContext );
	}

	if( m_bInCave ) Render_InCave( pContext );

	// 블렌딩 설정해제
	CRenderState::Set_DepthStencilState( pContext, CRenderState::DS_NULL );
	CRenderState::Set_BlendState( pContext, CRenderState::BL_NULL );
}

void CRenderer::Render_UI( ID3D11DeviceContext* pContext )
{
	// 깊에버퍼에 값은 기록하지만, 깊이버퍼에 의한 정렬을 수행하지않고 무조건 덮어서 그린다.
	// 깊이버퍼에 값을 기록하지않고 그린다.

	CRenderState::Set_DepthStencilState( pContext, CRenderState::DS_NO_TEST );
	CRenderState::Set_BlendState( pContext, CRenderState::BL_ALPHA );
	SHADERLIST::iterator	iter = m_pRenderGroup[ RENDER_UI ].begin();
	SHADERLIST::iterator	iter_end = m_pRenderGroup[ RENDER_UI ].end();

	for( ; iter != iter_end; ++iter )
	{
		( *iter )->Render( pContext );
	}

	// 깊이버퍼에 의한 정렬을 수행.
	// 깊이버퍼에 값을 기록한다.
}

void CRenderer::Render_RenderTargetDebug( ID3D11DeviceContext* pContext )
{
	CRenderState::Set_DepthStencilState( pContext, CRenderState::DS_NO_TEST );

	m_pRenderGroup[ RENDER_DEBUG ].front()->Render( pContext );
	CRenderTargetMgr::GetInstance()->Render( pContext );
}
