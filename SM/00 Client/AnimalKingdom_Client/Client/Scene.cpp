#include "stdafx.h"
#include "Scene.h"
#include "GameObject.h"
#include "Function.h"
#include "Camera.h"
#include "Logo.h"
#include "Jungle.h"

CScene * CScene::Create( HWND hWnd, ID3D11Device * pDevice, BYTE byStageNum )
{
	CScene* pScene = nullptr;

	switch( byStageNum )
	{
	case SCENE_LOGO:
		pScene = CLogo::Create( hWnd, pDevice );
		break;
	case SCENE_JUNGLE:
		pScene = CJungle::Create( hWnd, pDevice );
		break;
	}

	if( nullptr == pScene ) {
		MessageBox( hWnd, "Scene Create Fail", nullptr, MB_OK );
	}
	return pScene;
}

CScene::CScene()
	:m_hWnd( nullptr )
	, m_pCamera( nullptr )
{
}

HRESULT CScene::Initialize( HWND hWnd, ID3D11Device * pDevice )
{
	m_hWnd = hWnd;
	if( nullptr == m_hWnd )
		return E_FAIL;

	return S_OK;
}

int CScene::Update( const float& fTimeDelta )
{
	if( nullptr != m_pCamera )
		m_pCamera->Update( fTimeDelta );

	return 0;
}

DWORD CScene::Release( void )
{
	::Safe_Release( m_pCamera );

	return 0;
}

void CScene::Render( ID3D11DeviceContext* pContext )
{
	if( nullptr != m_pCamera )
		m_pCamera->Render( pContext );
}
