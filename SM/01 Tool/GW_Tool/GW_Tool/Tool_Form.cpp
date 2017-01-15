// Tool_Form.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "GW_Tool.h"
#include "MainFrm.h"
#include "Tool_Form.h"
#include "Importer.h"
#include "Animation_Controller.h"
#include "GraphicDev.h"
#include "Tool_View.h"
#include "FbxContext.h"
#include "AnimationSet.h"

// CTool_Form

IMPLEMENT_DYNCREATE( CTool_Form, CFormView )

CTool_Form::CTool_Form()
	: CFormView( IDD_TOOL_FORM )
{

}

CTool_Form::~CTool_Form()
{
}

void CTool_Form::DoDataExchange( CDataExchange* pDX )
{
	CFormView::DoDataExchange( pDX );
	DDX_Control( pDX, IDC_LIST1, m_mfcListBox[ LISTBOX_MESH ] );
	DDX_Control( pDX, IDC_LIST2, m_mfcListBox[ LISTBOX_ANIMATION ] );
	DDX_Control( pDX, IDC_CHECK1, m_mfcCheckBox[ CHECK_WIREFRAME ] );
	DDX_Control( pDX, IDC_SLIDER1, m_mfcSlider );
	DDX_Control( pDX, IDC_EDIT1, m_mfcEdit[ EDIT_START ] );
	DDX_Control( pDX, IDC_EDIT2, m_mfcEdit[ EDIT_LAST ] );
	DDX_Control( pDX, IDC_EDIT3, m_mfcEdit[ EDIT_SPEED ] );
	DDX_Control( pDX, IDC_SPIN1, m_mfcSphine );
}

BEGIN_MESSAGE_MAP( CTool_Form, CFormView )
	ON_WM_DROPFILES()
	ON_LBN_SELCHANGE( IDC_LIST1, &CTool_Form::OnLbnListBox_Mesh )
	ON_BN_CLICKED( IDC_CHECK1, &CTool_Form::OnBn_CheckBox_WireFrame )
	ON_BN_CLICKED( IDC_BUTTON1, &CTool_Form::OnBnExport )
	ON_BN_CLICKED( IDC_BUTTON2, &CTool_Form::OnBnPlay )
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_LBN_SELCHANGE( IDC_LIST2, &CTool_Form::OnLbnListBox_Animation )
END_MESSAGE_MAP()


// CTool_Form �����Դϴ�.

#ifdef _DEBUG
void CTool_Form::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CTool_Form::Dump( CDumpContext& dc ) const
{
	CFormView::Dump( dc );
}
#endif
#endif //_DEBUG


// CTool_Form �޽��� ó�����Դϴ�.

void CTool_Form::OnDropFiles( HDROP hDropInfo )
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	// �� ��° ���ڿ� 0xFFFFFFFF �� �Է��ϸ�, ����� ������ ���� ��ȯ
	// �� ��° ���ڿ� 0 ~ ����� ������ ���� ���� �־��ָ�, �����̸��� ���� ��ȯ
	// ���� ���� ��ȯ���� ���� ���� �� ��° ���ڸ� nullptr �� �־��ش�.

	( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_bPause = true;

	if( nullptr != ( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_pActiveImporter )
		( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_pActiveImporter->m_bAnimate = false;

	UINT iDropCnt = DragQueryFile( hDropInfo, 0xFFFFFFFF, nullptr, 0 );

	TCHAR** wstrFilePath = new TCHAR*[ iDropCnt ];
	for( UINT i = 0; i < iDropCnt; ++i )
		wstrFilePath[ i ] = new TCHAR[ MAX_PATH ];

	CGraphicDev* pGraphicDev = ( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_pGraphicDev;

	if( iDropCnt > 0 )
	{
		for( UINT i = 0; i < iDropCnt; ++i )
		{
			UINT iLen = DragQueryFile( hDropInfo, i, nullptr, 0 ) + 1;	// \0 ����

			DragQueryFile( hDropInfo, i, ( LPWSTR )wstrFilePath[ i ], iLen );
			wcscpy_s( wstrFilePath[ i ], MAX_PATH, ConvertRelativePath( wstrFilePath[ i ] ).c_str() );
			m_mfcListBox[ LISTBOX_MESH ].AddString( wstrFilePath[ i ] );
			char szFileName[ MAX_PATH ]{ "" };
			WideCharToMultiByte( CP_ACP, 0, wstrFilePath[ i ], MAX_PATH, szFileName, MAX_PATH, NULL, NULL );

			CImporter* pImporter = CImporter::Create( pGraphicDev, szFileName );
			bool bExist{ false };
			size_t j = 0;
			for( ; j < ( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_vecImporter.size(); j++ )
				if( 0 == strcmp( ( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_vecImporter[ j ].first.c_str(), pImporter->m_pFbxContext->m_pMeshName ) )
				{
					bExist = true;
					break;
				}
					

			if( false == bExist || false == pImporter->m_pFbxContext->m_bIncludeAnimation )
				( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_vecImporter.push_back( make_pair( pImporter->m_pFbxContext->m_pMeshName, pImporter ) );
			else
			{
				( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_vecImporter[j].second->m_pAnimation_Controller->CreateAnimationSet( pImporter->m_pFbxContext->m_vecJoint, pImporter->m_pFbxContext->m_strAnimationName.c_str(), 0, ( DWORD )pImporter->m_pFbxContext->m_llAnimationLength );
				
				pImporter = nullptr;
				int iIndex = m_mfcListBox[ LISTBOX_MESH ].GetCount();
				m_mfcListBox[ LISTBOX_MESH ].DeleteString( iIndex - 1 );
			}
		}
	}

	auto test = ( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_vecImporter;
	DragFinish( hDropInfo );

	for( UINT i = 0; i < iDropCnt; ++i )
		delete[] wstrFilePath[ i ];

	( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_bPause = false;

	CFormView::OnDropFiles( hDropInfo );
}

wstring CTool_Form::ConvertRelativePath( wstring szFullPath )
{
	TCHAR szRelativePath[ MAX_PATH ] = L"";
	TCHAR szDirectoryPath[ MAX_PATH ] = L"";

	GetCurrentDirectory( sizeof( szDirectoryPath ), szDirectoryPath );

	PathRelativePathTo( ( LPWSTR )szRelativePath
		, szDirectoryPath
		, FILE_ATTRIBUTE_DIRECTORY
		, szFullPath.c_str()
		, FILE_ATTRIBUTE_NORMAL );

	return szRelativePath;
}


void CTool_Form::Print_ListBox_Animation( void )
{
	m_mfcListBox[ LISTBOX_ANIMATION ].ResetContent();
	int iStrCnt = m_mfcListBox[ LISTBOX_ANIMATION ].GetCount();
	for( int i = 0; i < iStrCnt; ++i )
		m_mfcListBox[ LISTBOX_ANIMATION ].DeleteString( i );

	if( !( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_pActiveImporter->m_pFbxContext->m_bIncludeAnimation ) return;

	int iAnimationCnt = ( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_pActiveImporter->m_pAnimation_Controller->m_iAnimationSetCnt;

	for( int i = 0; i < iAnimationCnt; ++i ) {
		string strName = ( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_pActiveImporter->m_pAnimation_Controller->m_vecAnimationSet[ i ]->m_strName;
		CString cs( strName.c_str() );
		m_mfcListBox[ LISTBOX_ANIMATION ].AddString( cs );
	}
	
	Active_Slider();
}

void CTool_Form::Active_Slider()
{
	m_mfcSlider.EnableWindow( TRUE );

	CImporter* pImporter = ( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_pActiveImporter;
	int iMax = pImporter->m_pAnimation_Controller->m_pActiveAnimationSet->m_iLength - 1;
	
	m_mfcSlider.SetRange( 0, iMax );
	m_mfcSlider.SetPos( 0 );
	m_mfcSlider.SetTicFreq( 1 );
	m_mfcSlider.SetLineSize( 1 );
	m_mfcSlider.SetPageSize( 1 );
}

void CTool_Form::OnLbnListBox_Mesh()
{
	( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_bPause = true;
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int iIndex = m_mfcListBox[ LISTBOX_MESH ].GetCurSel();
	if( nullptr != ( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_pActiveImporter )
		( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_pActiveImporter->m_bAnimate = false;

	if( iIndex < 0 )
	{
		( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_bPause = false;
		return;
	}

	m_mfcSlider.EnableWindow( FALSE );

	auto Advance_Iter = ( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_vecImporter.begin();
	for( int i = 0; i < iIndex; i++ )
		Advance_Iter++;
	( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_pActiveImporter = Advance_Iter->second;
	Print_ListBox_Animation();

	( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_bPause = false;

	( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->Invalidate();
}

void CTool_Form::OnBn_CheckBox_WireFrame()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_bWirefream = ( bool )m_mfcCheckBox[ CHECK_WIREFRAME ].GetCheck();
	( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->Invalidate();
}


void CTool_Form::OnBnExport()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_bPause = true;
	( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_pActiveImporter->m_bAnimate = false;
	( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_pActiveImporter->Export_JSM();
	( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_bPause = false;
}


void CTool_Form::OnBnPlay()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( m_mfcListBox[ LISTBOX_ANIMATION ].GetCurSel() < 0 ) return;

	( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_bPause = true;

	( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_pActiveImporter->m_bAnimate = true;

	( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_bPause = false;
}


void CTool_Form::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	SetDlgItemTextW( IDC_EDIT1, ( LPCTSTR )"0" );
	SetDlgItemTextW( IDC_EDIT2, ( LPCTSTR )"0" );
	SetDlgItemTextW( IDC_EDIT3, ( LPCTSTR )"1" );

	m_mfcSphine.SetBuddy( &m_mfcEdit[ EDIT_SPEED ] );
	m_mfcSphine.SetRange( 1, 100 );
}


void CTool_Form::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_bPause = true;

	CImporter* pImporter = ( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_pActiveImporter;
	if( nullptr == pImporter )
	{
		( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_bPause = false;
		return;
	}
	if( !pImporter->m_pFbxContext->m_bIncludeAnimation )
	{
		( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_bPause = false;
		return;
	}

	if( nullptr != pImporter->m_pAnimation_Controller
		|| nullptr != pImporter->m_pAnimation_Controller->m_pActiveAnimationSet )
		pImporter->m_pAnimation_Controller->m_pActiveAnimationSet->m_fTimePos = ( float )nPos;

	( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_bPause = false;

	CFormView::OnHScroll( nSBCode, nPos, pScrollBar );
}


void CTool_Form::OnVScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_bPause = true;
	CImporter* pImporter = ( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_pActiveImporter;
	if( nullptr == pImporter )
	{
		( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_bPause = false;
		return;
	}
	if( !pImporter->m_pFbxContext->m_bIncludeAnimation )
	{
		( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_bPause = false;
		return;
	}

	if( nullptr != pImporter->m_pAnimation_Controller
		|| nullptr != pImporter->m_pAnimation_Controller->m_pActiveAnimationSet )
		pImporter->m_pAnimation_Controller->m_pActiveAnimationSet->m_fSpeed = ( float )nPos;

	( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_bPause = false;

	CFormView::OnVScroll( nSBCode, nPos, pScrollBar );
}


void CTool_Form::OnLbnListBox_Animation()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_bPause = true;

	CImporter* pImporter = ( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_pActiveImporter;
	if( nullptr == pImporter )
	{
		( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_bPause = false;
		return;
	}

	if( !pImporter->m_pFbxContext->m_bIncludeAnimation )
	{
		( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_bPause = false;
		return;
	}

	( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_pActiveImporter->m_bAnimate = false;
	size_t iAnimationCnt = pImporter->m_pAnimation_Controller->m_vecAnimationSet.size();
	int iIndex = m_mfcListBox[ LISTBOX_ANIMATION ].GetCurSel();

	if( iIndex > ( int )iAnimationCnt
		|| iIndex < 0 )
	{
		( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_bPause = false;
		return;
	}

	pImporter->m_pAnimation_Controller->m_pActiveAnimationSet = pImporter->m_pAnimation_Controller->m_vecAnimationSet[ iIndex ];

	SetDlgItemInt( IDC_EDIT2, ( UINT ) pImporter->m_pAnimation_Controller->m_pActiveAnimationSet->m_iLength - 1 );
	SetDlgItemInt( IDC_EDIT3, ( UINT ) pImporter->m_pAnimation_Controller->m_pActiveAnimationSet->m_fSpeed );

	Active_Slider();

	( ( CMainFrame* )AfxGetMainWnd() )->m_pToolView->m_bPause = false;
}
