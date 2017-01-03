#include "stdafx.h"
#include "Scene_Stage.h"
#include "ResourceMgr.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Renderer.h"
#include "Function.h"
#include "ShaderMgr.h"
#include "Player.h"
#include "Camera_Dynamic.h"
#include "Terrain.h"
#include "PhysicsMgr.h"
#include "RenderState.h"

HRESULT CScene_Stage::Initialize( HWND hWnd, ID3D11Device * pDevice, ID3D11DeviceContext * pContext )
{
	m_hWnd = hWnd;
	/* PhysX Scene Create */
	m_pPhysicsMgr = CPhysicsMgr::GetInstance();
	if( FAILED( m_pPhysicsMgr->CreateScene( pDevice ) ) )
	{
		MessageBox( m_hWnd, "CPhysicsMgr::CreateScene Failed", 0, MB_OK );
		return E_FAIL;
	}

	Create_BaseObject( pDevice, pContext );
	for( int i = 0; i < 2; ++i )
		m_bOverlapped[i] = true;

	return S_OK;
}

int CScene_Stage::Update( const float & fTimeDelta )
{
	CScene::Update( fTimeDelta );

	if( ( GetAsyncKeyState( '1' ) & 0x8000 )
		&& m_bOverlapped[0] ) {
		CRenderer::m_bWireFrame = !CRenderer::m_bWireFrame;
		m_bOverlapped[0] = false;
	}

	else if( !( GetAsyncKeyState( '1' ) & 0x8000 ) ) m_bOverlapped[0] = true;
	
	m_pPhysicsMgr->Update( fTimeDelta );
	CRenderer::GetInstance()->Copy_RenderGroup( m_RenderGroup );

	return 0;
}

void CScene_Stage::Render( ID3D11DeviceContext * pContext )
{
	if( nullptr != m_pCamera )
		m_pCamera->SetConstantBuffer( pContext );

#ifdef _DEBUG
	CShader* pShader = CShaderMgr::GetInstance()->Clone_Shaders( "Shader_Debug" );

	CRenderState::Set_Rasterize( pContext, CRenderState::RS_WIREFRAME );
	CRenderState::Set_DepthStencilState( pContext, CRenderState::DS_NO_WRITE );
	pShader->Render( pContext );
	
	CPhysicsMgr::GetInstance()->Render( pContext );

	pShader->Release();
#endif

	CRenderer::GetInstance()->Render( pContext );
}

void CScene_Stage::Release( void )
{
	CScene::Release();

	for( int i = 0; i < RENDER_END; ++i ) {
		if( !m_RenderGroup[i].empty() ) {
			auto iter_shader = m_RenderGroup[i].begin();

			for( ; iter_shader != m_RenderGroup[i].end(); ) {
				( *iter_shader )->Release();
				iter_shader = m_RenderGroup[i].erase( iter_shader );
			}
		}
	}

	if( nullptr != m_pPhysicsMgr )
	{
		m_pPhysicsMgr->Release_Scene();
		m_pPhysicsMgr = nullptr;
	}

	delete this;
}

void CScene_Stage::Create_BaseObject( ID3D11Device* pDevice, ID3D11DeviceContext* pContext )
{
	vector<CGameObject*> vecGameObject;
	vecGameObject.reserve( m_pPhysicsMgr->Get_ActorCnt() );

	/* Terrain */
	CShader* pShader = CShaderMgr::GetInstance()->Clone_Shaders( "Shader_Terrain" );
	CMesh* pMesh = CResourceMgr::GetInstance()->CloneMesh( "Mesh_Point" );
	CTexture* pTexture = CResourceMgr::GetInstance()->CloneTexture( "Texture_Terrain" );
	CTerrain*	pTerrain = CTerrain::Create( pDevice, pContext, pMesh, pTexture, XMFLOAT3{ 0.f, 0.f, 0.f } );
	pShader->Set_RenderObject( pTerrain );
	vecGameObject.push_back( pTerrain );
	m_RenderGroup[RENDER_DEPTHTEST].push_back( pShader );

	/*					Base					*/
	// Player
	pShader = CShaderMgr::GetInstance()->Clone_Shaders( "Shader_Cube" );
	pMesh = CResourceMgr::GetInstance()->CloneMesh( "Mesh_Point" );
	pTexture = CResourceMgr::GetInstance()->CloneTexture( "Texture_Metal" );
	CPlayer*	pPlayer = CPlayer::Create( pDevice, pContext, pMesh, pTexture, XMFLOAT3{ 0.f, 0.f, 0.f } );
	pShader->Set_RenderObject( pPlayer );
	vecGameObject.push_back( pPlayer );
	m_RenderGroup[RENDER_DEPTHTEST].push_back( pShader );

	// Camera
	m_pCamera = CCamera_Dynamic::Create( m_hWnd, pDevice, pContext );

	m_pPhysicsMgr->Connect_Actors( &vecGameObject );
}

CScene * CScene_Stage::Create( HWND hWnd, ID3D11Device * pDevice, ID3D11DeviceContext * pContext )
{
	CScene_Stage* pScene = new CScene_Stage;

	if( FAILED( pScene->Initialize( hWnd, pDevice, pContext ) ) ) {
		pScene->Release();
		pScene = nullptr;
	}

	return pScene;
}
