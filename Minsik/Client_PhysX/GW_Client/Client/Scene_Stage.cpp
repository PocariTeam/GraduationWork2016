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
#include "Camera_Third.h"
#include "Terrain.h"

HRESULT CScene_Stage::Initialize( HWND hWnd, ID3D11Device * pDevice, ID3D11DeviceContext * pContext )
{

	m_pPhysicsManager = new CPhysicsManager;
	m_pPhysicsManager->InitNx();

	Create_BaseObject( pDevice, pContext );
	Create_Environment( pDevice, pContext );
	Create_MovingObject( pDevice, pContext );
	for( int i = 0; i < 2; ++i )
		m_bOverlapped[i] = true;

	return S_OK;
}

int CScene_Stage::Update( const float & fTimeDelta )
{
	if( nullptr != m_pCamera )
		m_pCamera->Update( fTimeDelta );

	if(nullptr != m_pPhysicsManager)
		m_pPhysicsManager->Update( fTimeDelta );

	CScene::Update( fTimeDelta );

	if( ( GetAsyncKeyState( 'W' ) & 0x8000 )
		&& m_bOverlapped[0] ) {
		CRenderer::m_bWireFrame = !CRenderer::m_bWireFrame;
		m_bOverlapped[0] = false;
	}

	else if( !( GetAsyncKeyState( 'W' ) & 0x8000 ) ) m_bOverlapped[0] = true;
		

	CRenderer::GetInstance()->Copy_RenderGroup( m_RenderGroup );

	return 0;
}

void CScene_Stage::Render( ID3D11DeviceContext * pContext )
{
	if( nullptr != m_pCamera )
		m_pCamera->SetConstantBuffer( pContext );

	CRenderer::GetInstance()->Render( pContext );
}

void CScene_Stage::Release( void )
{
	m_pPhysicsManager->ExitNx();
	delete	m_pPhysicsManager;

	for( int i = 0; i < RENDER_END; ++i ) {
		if( !m_RenderGroup[i].empty() ) {
			auto iter_shader = m_RenderGroup[i].begin();

			for( ; iter_shader != m_RenderGroup[i].end(); ) {
				( *iter_shader )->Release();
				iter_shader = m_RenderGroup[i].erase( iter_shader );
			}
		}
	}

	CScene::Release();

	delete this;
}

void CScene_Stage::Create_BaseObject( ID3D11Device* pDevice, ID3D11DeviceContext* pContext )
{
	/*					Base					*/
	// Player
	CShader* pShader = CShaderMgr::GetInstance()->Clone_Shaders( "Shader_Cube" );
	CMesh* pMesh = CResourceMgr::GetInstance()->CloneMesh( "Mesh_Point" );
	CTexture* pTexture = CResourceMgr::GetInstance()->CloneTexture( "Texture_Metal" );
	CPlayer*	pPlayer = CPlayer::Create( pDevice, pContext, pMesh, pTexture, XMFLOAT3{ 0.f, 0.f, 0.f } );
	m_plistObj[OBJ_MOVING].push_back( pPlayer );
	pShader->Set_RenderObject( pPlayer );
	m_RenderGroup[RENDER_DEPTHTEST].push_back( pShader );

	// Camera
	m_pCamera = CCamera_Third::Create( pDevice, pContext, pPlayer->Get_Transform() );
}

void CScene_Stage::Create_Environment( ID3D11Device* pDevice, ID3D11DeviceContext* pContext )
{
	/* Terrain */
	CShader* pShader = CShaderMgr::GetInstance()->Clone_Shaders( "Shader_Terrain" );
	CMesh* pMesh = CResourceMgr::GetInstance()->CloneMesh( "Mesh_Point" );
	CTexture* pTexture = CResourceMgr::GetInstance()->CloneTexture( "Texture_Terrain" );
	CTerrain*	pTerrain = CTerrain::Create( pDevice, pContext, pMesh, pTexture, XMFLOAT3{ 0.f, 0.f, 0.f } );
	m_plistObj[OBJ_ENVIRONMENT].push_back( pTerrain );
	pShader->Set_RenderObject( pTerrain );
	m_RenderGroup[RENDER_DEPTHTEST].push_back( pShader );
}

void CScene_Stage::Create_MovingObject( ID3D11Device* pDevice, ID3D11DeviceContext* pContext )
{
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
