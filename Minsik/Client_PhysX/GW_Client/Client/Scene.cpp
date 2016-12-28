#include "stdafx.h"
#include "Scene.h"
#include "GameObject.h"
#include "Function.h"
#include "Shader.h"
#include "Scene_Logo.h"
#include "Scene_Stage.h"
#include "Camera.h"

CScene * CScene::Create( HWND hWnd, ID3D11Device * pDevice, ID3D11DeviceContext * pContext, BYTE byStageNum )
{
	CScene* pScene = nullptr;

	switch( byStageNum )
	{
	case SCENE_LOAD:
		pScene = CScene_Logo::Create( hWnd, pDevice, pContext );
		break;
	case SCENE_STAGE:
		pScene = CScene_Stage::Create( hWnd, pDevice, pContext );
		break;
	}

	if( nullptr == pScene ) {
		MessageBox( hWnd, "Scene 생성 실패", nullptr, MB_OK );
	}
	return pScene;
}

CScene::CScene()
	:m_hWnd( nullptr )
	, m_pCamera( nullptr )
{
}

HRESULT CScene::Initialize( HWND hWnd, ID3D11Device * pDevice, ID3D11DeviceContext * pContext )
{
	m_hWnd = hWnd;
	if( nullptr == m_hWnd )
		return E_FAIL;

	return S_OK;
}

int CScene::Update( const float& fTimeDelta )
{
	for( size_t i = 0; i < OBJ_END; i++ ) {
		if( m_plistObj[i].empty() )
			continue;

		list<CGameObject*>::iterator iter_begin = m_plistObj[i].begin();

		for( ; iter_begin != m_plistObj[i].end(); )
		{
			if( 0 >( *iter_begin )->Update( fTimeDelta ) )
			{
				( *iter_begin )->Release();
				( *iter_begin ) = nullptr;

				iter_begin = m_plistObj[i].erase( iter_begin );
			}

			else
				iter_begin++;
		}
	}

	return 0;
}

void CScene::Release( void )
{
	::Safe_Release( m_pCamera );

	for( size_t i = 0; i < OBJ_END; i++ ) {
		if( m_plistObj[i].empty() )
			continue;

		list<CGameObject*>::iterator iter_begin = m_plistObj[i].begin();

		for( ; iter_begin != m_plistObj[i].end(); ) {
			( *iter_begin )->Release();
			iter_begin = m_plistObj[i].erase( iter_begin );
		}

		m_plistObj[i].clear();
	}
}