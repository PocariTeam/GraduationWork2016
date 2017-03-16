#include "stdafx.h"
#include "Lobby.h"
#include "InputMgr.h"
#include "Shader.h"

CLobby::CLobby()
	: CScene()
	, m_pShader( nullptr )
{
}

CLobby::~CLobby()
{
}

HRESULT CLobby::Initialize( HWND hWnd, ID3D11Device* pDevice )
{


	return CScene::Initialize( hWnd, pDevice );
}

int CLobby::Update( const float& fTimeDelta )
{
	CScene::Update( fTimeDelta );

	if( CInputMgr::GetInstance()->Get_KeyboardState( DIK_RETURN ) & 0x80 )
		return CScene::SCENE_JUNGLE;

	return 0;
}

DWORD CLobby::Release( void )
{
	CScene::Release();

	::Safe_Release( m_pShader );

	delete this;

	return 0;
}

void CLobby::Render( ID3D11DeviceContext* pContext )
{
	CScene::Render( pContext );

	m_pShader->Render( pContext );
}

CScene* CLobby::Create( HWND hWnd, ID3D11Device* pDevice )
{
	CLobby*	pLobby = new CLobby;

	if( FAILED( pLobby->Initialize( hWnd, pDevice ) ) )
	{
		pLobby->Release();
		pLobby = nullptr;
	}

	return pLobby;
}
