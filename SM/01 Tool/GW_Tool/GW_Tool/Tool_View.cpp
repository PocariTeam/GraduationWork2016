// Tool_View.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "GW_Tool.h"
#include "Tool_View.h"
#include "GraphicDev.h"
#include "Tool_Header.h"
#include "Importer.h"
#include "Camera.h"
#include "Shader.h"
#include "Light.h"

// CTool_View

IMPLEMENT_DYNCREATE(CTool_View, CView)

CTool_View::CTool_View()
	:m_pActiveImporter( nullptr )
	, m_pGraphicDev( nullptr )
	, m_pCamera( nullptr )
	, m_pRSState( nullptr )
	, m_bWirefream( false )
	, m_bPause( false )
{
}

CTool_View::~CTool_View()
{
	m_pActiveImporter = nullptr;

	if( m_pCamera )
	{
		m_pCamera->Release();
		m_pCamera = nullptr;
	}

	if( m_pLight )
	{
		m_pLight->Release();
		m_pLight = nullptr;
	}

	if( m_pRSState )
	{
		m_pRSState->Release();
		m_pRSState = nullptr;
	}

	if( !m_vecImporter.empty() )
	{
		auto Advance_Iter = m_vecImporter.begin();
		for( ; Advance_Iter != m_vecImporter.end(); ++Advance_Iter )
			Advance_Iter->second->Release();

		m_vecImporter.clear();
		m_vecImporter.erase( m_vecImporter.begin(), m_vecImporter.end() );
	}

	if( m_pGraphicDev )
	{
		m_pGraphicDev->Release();
		m_pGraphicDev = nullptr;
	}
}

BEGIN_MESSAGE_MAP(CTool_View, CView)
	ON_WM_TIMER()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


// CTool_View 그리기입니다.

void CTool_View::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
	if( nullptr == m_pGraphicDev )
		return;

	m_pGraphicDev->BeginScene();

	if( m_bWirefream ) m_pGraphicDev->Get_Context()->RSSetState( m_pRSState );
	else m_pGraphicDev->Get_Context()->RSSetState( nullptr );

	if( !m_bPause )
	{
		m_pCamera->Update();
		m_pLight->SetConstantBuffer( m_pGraphicDev->Get_Context() );
		m_pCamera->SetConstantBuffer( m_pGraphicDev->Get_Context() );
		if( m_pActiveImporter )
		{
			m_pActiveImporter->Update( 0.016f );
			m_pActiveImporter->Render( m_pGraphicDev->Get_Context() );
		}
	}

	m_pGraphicDev->EndScene();
}


// CTool_View 진단입니다.

#ifdef _DEBUG
void CTool_View::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CTool_View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

void CTool_View::CreateRasterizerState()
{
	/* 레스터라이즈 상태 */
	D3D11_RASTERIZER_DESC	tRSDesc;
	ZeroMemory( &tRSDesc, sizeof( D3D11_RASTERIZER_DESC ) );
	tRSDesc.DepthClipEnable = TRUE;
	tRSDesc.FillMode = D3D11_FILL_WIREFRAME;
	tRSDesc.CullMode = D3D11_CULL_NONE;
	tRSDesc.FrontCounterClockwise = false;
	tRSDesc.DepthClipEnable = true;
	m_pGraphicDev->Get_Device()->CreateRasterizerState( &tRSDesc, &m_pRSState );
}

#endif
#endif //_DEBUG


// CTool_View 메시지 처리기입니다.


void CTool_View::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	g_hWnd = m_hWnd;
	m_pGraphicDev = CGraphicDev::Create( CGraphicDev::MODE_WIN, g_hWnd, C_WINSIZE_WIDTH, C_WINSIZE_HEIGHT );
	m_pCamera = CCamera::Create( m_pGraphicDev->Get_Device(), m_pGraphicDev->Get_Context() );
	m_pLight = CLight::Create( m_pGraphicDev->Get_Device() );
	CreateRasterizerState();

	SetTimer( 0, 16, 0 );
}


void CTool_View::OnTimer( UINT_PTR nIDEvent )
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CView::OnTimer( nIDEvent );
	Invalidate( FALSE );
}

BOOL CTool_View::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	float fzDelta = ( float )zDelta * 0.001f;
	XMVECTOR vEye{ XMLoadFloat3( &m_pCamera->m_vEye ) };
	XMVECTOR vMove{ 0.f, fzDelta, fzDelta };

	vEye += vMove;

	XMStoreFloat3( &m_pCamera->m_vEye, vEye );

	Invalidate( FALSE );
	return CView::OnMouseWheel( nFlags, zDelta, pt );
}
