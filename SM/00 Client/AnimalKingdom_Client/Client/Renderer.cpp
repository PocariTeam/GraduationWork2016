#include "stdafx.h"
#include "Renderer.h"
#include "Shader.h"
#include "Function.h"
#include "RenderState.h"
#include "RenderTargetMgr.h"
#include "LightMgr.h"

CRenderer*	CSingleton<CRenderer>::m_pInstance;
bool		CRenderer::m_bWireFrame = false;
bool		CRenderer::m_bRenderTargetDebug = false;

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

void CRenderer::Clear_RenderGroup( void )
{
	/*for( int i = 0; i < RENDER_END; ++i ) {
		if( m_pRenderGroup[ i ].empty() ) continue;
		auto iter = m_pRenderGroup[ i ].begin();
		for( ; iter != m_pRenderGroup[ i ].end(); )
			iter = m_pRenderGroup[ i ].erase( iter );

		m_pRenderGroup[ i ].clear();
	}*/
}

DWORD CRenderer::Release( void )
{
	Clear_RenderGroup();

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

void CRenderer::Render( ID3D11DeviceContext* pContext )
{
	if( nullptr == m_pRenderGroup ) return;

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

void CRenderer::Render_Background( ID3D11DeviceContext* pContext )
{
	// ���̹��ۿ� ���� ��������ʰ� �׸���.
	if( m_bWireFrame )	CRenderState::Set_Rasterize( pContext, CRenderState::RS_WIREFRAME );
	else CRenderState::Set_Rasterize( pContext, CRenderState::RS_NULL );

	CRenderState::Set_BlendState( pContext, CRenderState::BL_NULL );
	CRenderState::Set_DepthStencilState( pContext, CRenderState::DS_NO_WRITE );

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
	
	ID3D11RenderTargetView* pNullRTV[ 6 ] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	pContext->OMSetRenderTargets( 3, pNullRTV, nullptr );
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
}

void CRenderer::Render_Light( ID3D11DeviceContext* pContext )
{
	CRenderState::Set_DepthStencilState( pContext, CRenderState::DS_NO_TEST );
	
	//D3DX11SaveTextureToFile( pContext, CRenderTargetMgr::GetInstance()->GetTexture( CRenderTargetMgr::RT_DEPTH ), D3DX11_IFF_JPG, "Depth.jpg" );
	//D3DX11SaveTextureToFile( pContext, CRenderTargetMgr::GetInstance()->GetTexture( CRenderTargetMgr::RT_ALBEDO ), D3DX11_IFF_JPG, "Albedo.jpg" );
	//D3DX11SaveTextureToFile( pContext, CRenderTargetMgr::GetInstance()->GetTexture( CRenderTargetMgr::RT_NORMAL ), D3DX11_IFF_JPG, "Normal.jpg" );
	ID3D11RenderTargetView* pNullRTV[ 6 ] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	pContext->OMSetRenderTargets( 3, pNullRTV, nullptr );
	ID3D11ShaderResourceView* pNullSRV[ 6 ] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	pContext->PSSetShaderResources( 0, 3, pNullSRV );
	CRenderTargetMgr::GetInstance()->SetRenderTargetView( pContext, CRenderTargetMgr::RT_LIGHT, 2 );
	CRenderTargetMgr::GetInstance()->SetShaderResourceView( pContext, 0, CRenderTargetMgr::RT_NORMAL, 2 );

	CLightMgr::GetInstance()->Render( pContext );

	m_pRenderGroup[ RENDER_LIGHT ].front()->Render( pContext );
}

void CRenderer::Render_Blend( ID3D11DeviceContext* pContext )
{
	CRenderState::Set_DepthStencilState( pContext, CRenderState::DS_NO_TEST );

	// D3DX11SaveTextureToFile( pContext, CRenderTargetMgr::GetInstance()->GetTexture( CRenderTargetMgr::RT_LIGHT ), D3DX11_IFF_JPG, "Light.jpg" );
	// D3DX11SaveTextureToFile( pContext, CRenderTargetMgr::GetInstance()->GetTexture( CRenderTargetMgr::RT_SPECULAR ), D3DX11_IFF_JPG, "Specular.jpg" );
	ID3D11RenderTargetView* pNullRTV[ 6 ] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	pContext->OMSetRenderTargets( 3, pNullRTV, nullptr );
	ID3D11ShaderResourceView* pNullSRV[ 6 ] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	pContext->PSSetShaderResources( 0, 3, pNullSRV );
 	CRenderTargetMgr::GetInstance()->SetRenderTargetView( pContext, CRenderTargetMgr::RT_BACK, 1 );
	CRenderTargetMgr::GetInstance()->SetShaderResourceView( pContext, 0, CRenderTargetMgr::RT_ALBEDO, 3 );

	m_pRenderGroup[ RENDER_BLEND ].front()->Render( pContext );
}

void CRenderer::Render_Alpha( ID3D11DeviceContext* pContext )
{
	// ���� �����Ѵ�.
	// Z���� ���� ������ �����Ѵ�.
	CRenderState::Set_BlendState( pContext, CRenderState::BL_ALPHA );
	CRenderState::Set_DepthStencilState( pContext, CRenderState::DS_NO_WRITE );

	SHADERLIST::iterator	iter = m_pRenderGroup[ RENDER_ALPHA ].begin();
	SHADERLIST::iterator	iter_end = m_pRenderGroup[ RENDER_ALPHA ].end();

	for( ; iter != iter_end; ++iter )
	{
		( *iter )->Render( pContext );
	}

	// ���� ��������
	CRenderState::Set_BlendState( pContext, CRenderState::BL_NULL );
	CRenderState::Set_DepthStencilState( pContext, CRenderState::DS_NULL );
}

void CRenderer::Render_UI( ID3D11DeviceContext* pContext )
{
	// �����ۿ� ���� ���������, ���̹��ۿ� ���� ������ ���������ʰ� ������ ��� �׸���.
	// ���̹��ۿ� ���� ��������ʰ� �׸���.

	CRenderState::Set_DepthStencilState( pContext, CRenderState::DS_NO_TEST );

	SHADERLIST::iterator	iter = m_pRenderGroup[ RENDER_UI ].begin();
	SHADERLIST::iterator	iter_end = m_pRenderGroup[ RENDER_UI ].end();

	for( ; iter != iter_end; ++iter )
	{
		( *iter )->Render( pContext );
	}

	// ���̹��ۿ� ���� ������ ����.
	// ���̹��ۿ� ���� ����Ѵ�.
}

void CRenderer::Render_RenderTargetDebug( ID3D11DeviceContext* pContext )
{
	CRenderState::Set_DepthStencilState( pContext, CRenderState::DS_NO_TEST );

	m_pRenderGroup[ RENDER_DEBUG ].front()->Render( pContext );
	CRenderTargetMgr::GetInstance()->Render( pContext );
}
