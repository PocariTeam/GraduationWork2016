
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "GW_Tool.h"
#include "Tool_Header.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame
HWND	g_hWnd;
IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
	: m_pToolForm( nullptr )
	, m_pToolView( nullptr )
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if( CFrameWnd::OnCreate( lpCreateStruct ) == -1 )
		return -1;

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG

BOOL CMainFrame::OnCreateClient( LPCREATESTRUCT lpcs, CCreateContext* pContext )
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	m_mfcMainSplitter.CreateStatic( this, 1, 2 );
	m_mfcMainSplitter.CreateView( 0, 0, RUNTIME_CLASS( CTool_Form ), CSize( C_WINSIZE_WIDTH / 2, C_WINSIZE_HEIGHT ), pContext );
	m_mfcMainSplitter.CreateView( 0, 1, RUNTIME_CLASS( CTool_View ), CSize( C_WINSIZE_WIDTH, C_WINSIZE_HEIGHT ), pContext );
	m_mfcMainSplitter.IdFromRowCol( 0, 0 );

	m_pToolForm = ( CTool_Form* )m_mfcMainSplitter.GetPane( 0, 0 );
	m_pToolView = ( CTool_View* )m_mfcMainSplitter.GetPane( 0, 1 );

	return TRUE;
}
