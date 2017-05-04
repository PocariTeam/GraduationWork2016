#include "stdafx.h"
#include "OrthoCamera.h"
#include "InputMgr.h"
#include "Value.h"

COrthoCamera::COrthoCamera()
	: CCamera()
	, m_hWnd( nullptr )
{
}

COrthoCamera::~COrthoCamera()
{
}

HRESULT COrthoCamera::Initialize( HWND hWnd, ID3D11Device* pDevice )
{
	m_hWnd = hWnd;

	CCamera::Initialize( pDevice );

	return S_OK;
}

COrthoCamera* COrthoCamera::Create( HWND hWnd, ID3D11Device* pDevice )
{
	COrthoCamera* pDebugCamera = new COrthoCamera;

	if( FAILED( pDebugCamera->Initialize( hWnd, pDevice ) ) )
	{
		pDebugCamera->Release();
		pDebugCamera = nullptr;
	}

	return pDebugCamera;
}

int COrthoCamera::Update( const float& fTimeDelta )
{
	 XMMATRIX mtxProj = XMMatrixOrthographicOffCenterLH( -1.f, 1.f/* float( g_wWinsizeX )*/, -1.f, 1.f/*float( g_wWinsizeY )*/, -100.f, 1000.f );
	 XMStoreFloat4x4( &m_mtxProj, mtxProj );

	return 0;
}

DWORD COrthoCamera::Release( void )
{
	CCamera::Release();
	m_hWnd = nullptr;

	delete this;

	return 0;
}
