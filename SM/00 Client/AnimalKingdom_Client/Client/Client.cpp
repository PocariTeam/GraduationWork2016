// Client.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "Client.h"
#include "MainFrm.h"
#include "Timer.h"
#include "Function.h"
#include "NetworkMgr.h"

#define MENU_HEIGHT 47
#define MAX_LOADSTRING 100

// ���� ����:
HINSTANCE hInst;                                // ���� �ν��Ͻ��Դϴ�.
WCHAR szTitle[ MAX_LOADSTRING ];                  // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
WCHAR szWindowClass[ MAX_LOADSTRING ];            // �⺻ â Ŭ���� �̸��Դϴ�.

/* Client MainFrame Object */
CMainFrm*	g_pMainFrm{ nullptr };
WORD g_wWinsizeX = 1280;
WORD g_wWinsizeY = 720;

// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
ATOM                MyRegisterClass( HINSTANCE hInstance );
BOOL                InitInstance( HINSTANCE, int );
LRESULT CALLBACK    WndProc( HWND, UINT, WPARAM, LPARAM );
INT_PTR CALLBACK    About( HWND, UINT, WPARAM, LPARAM );

int APIENTRY wWinMain( _In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow )
{
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );

	// TODO: ���⿡ �ڵ带 �Է��մϴ�.
	//_CrtSetBreakAlloc( 370 );

	// ���� ���ڿ��� �ʱ�ȭ�մϴ�.
	LoadStringW( hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING );
	LoadStringW( hInstance, IDC_CLIENT, szWindowClass, MAX_LOADSTRING );
	MyRegisterClass( hInstance );

	// ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
	if( !InitInstance( hInstance, nCmdShow ) )
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators( hInstance, MAKEINTRESOURCE( IDC_CLIENT ) );

	MSG msg;

	/* Timer for Fixed Frame */
	CTimer* pFixedTimer{ CTimer::Create() };

	float fFrameLimit{ 1.f / 60.f };
	float fFrameTime{ 0.f };

	// �⺻ �޽��� �����Դϴ�.
	while( true )
	{
		pFixedTimer->Calculate_TimeDelta();
		fFrameTime += pFixedTimer->GetTimeDelta();

		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			if( msg.message == WM_QUIT )
				break;

			if( !TranslateAccelerator( msg.hwnd, hAccelTable, &msg ) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
		}

		else
		{
			// if( fFrameTime >= fFrameLimit )	// d
			{
				if( -1 == g_pMainFrm->Update( fFrameTime ) ) // ��ô�� ������(��Ÿ)Ÿ���� �־��ֵ��� ��������
					break;
				fFrameTime = 0.f;
				g_pMainFrm->Render();
			}
		}
	}

	::Safe_Release( g_pMainFrm );
	::Safe_Release( pFixedTimer );

#ifdef _DEBUG
	FreeConsole();
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	return ( int )msg.wParam;
}



//
//  �Լ�: MyRegisterClass()
//
//  ����: â Ŭ������ ����մϴ�.
//
ATOM MyRegisterClass( HINSTANCE hInstance )
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof( WNDCLASSEX );

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon( hInstance, MAKEINTRESOURCE( IDI_CLIENT ) );
	wcex.hCursor = LoadCursor( nullptr, IDC_ARROW );
	wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
	wcex.lpszMenuName = nullptr/*MAKEINTRESOURCEW(IDC_CLIENT)*/;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon( wcex.hInstance, MAKEINTRESOURCE( IDI_SMALL ) );

	return RegisterClassExW( &wcex );
}

//
//   �Լ�: InitInstance(HINSTANCE, int)
//
//   ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
//   ����:
//
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
//
BOOL InitInstance( HINSTANCE hInstance, int nCmdShow )
{
	hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

#ifdef _DEBUG
	AllocConsole();
	FILE*	pStreamOut;
	FILE*	pStreamIn;

	system( "mode con:cols=60 lines=15" );
	system( "title Debug Console" );
	freopen_s( &pStreamOut, "CONOUT$", "wt", stdout );
	freopen_s( &pStreamIn, "CONIN$", "rt", stdin );
#endif

	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;

	RECT	 rcWindow{ 0, 0, ( signed short )g_wWinsizeX, ( signed short )g_wWinsizeY };
	AdjustWindowRect( &rcWindow, dwStyle, FALSE );

	HWND hWnd = CreateWindowW( szWindowClass, szTitle, dwStyle,
		CW_USEDEFAULT, CW_USEDEFAULT, rcWindow.right - rcWindow.left, rcWindow.bottom - rcWindow.top, nullptr, nullptr, hInstance, nullptr );

	if( !hWnd )
		return FALSE;

	g_pMainFrm = CMainFrm::Create( hInstance, hWnd );

	ShowWindow( hWnd, nCmdShow );
	UpdateWindow( hWnd );

	return TRUE;
}

//
//  �Լ�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ����:  �� â�� �޽����� ó���մϴ�.
//
//  WM_COMMAND  - ���� ���α׷� �޴��� ó���մϴ�.
//  WM_PAINT    - �� â�� �׸��ϴ�.
//  WM_DESTROY  - ���� �޽����� �Խ��ϰ� ��ȯ�մϴ�.
//
//
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_CREATE:
		CNetworkMgr::GetInstance()->Initialize();
		break;
	case WM_SOCKET:
		CNetworkMgr::GetInstance()->processSocketMessage( hWnd, lParam );
		break;
	case WM_SIZE:
		g_wWinsizeX = LOWORD( lParam );
		g_wWinsizeY = HIWORD( lParam );
		g_pMainFrm->ResizeRenderTarget( g_wWinsizeX, g_wWinsizeY );
		break;
	case WM_KEYDOWN:
		switch( wParam )
		{
		case VK_ESCAPE:
			PostQuitMessage( 0 );
			return 0;
		}
		break;
	case WM_COMMAND:
	{
		int wmId = LOWORD( wParam );
		// �޴� ������ ���� �м��մϴ�.
		switch( wmId )
		{
		case IDM_ABOUT:
			DialogBox( hInst, MAKEINTRESOURCE( IDD_ABOUTBOX ), hWnd, About );
			break;
		case IDM_EXIT:
			DestroyWindow( hWnd );
			break;
		default:
			return DefWindowProc( hWnd, message, wParam, lParam );
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint( hWnd, &ps );
		// TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�.
		EndPaint( hWnd, &ps );
	}
	break;
	case WM_DESTROY:
		PostQuitMessage( 0 );
		break;
	default:
		return DefWindowProc( hWnd, message, wParam, lParam );
	}
	return 0;
}

// ���� ��ȭ ������ �޽��� ó�����Դϴ�.
INT_PTR CALLBACK About( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER( lParam );
	switch( message )
	{
	case WM_INITDIALOG:
		return ( INT_PTR )TRUE;

	case WM_COMMAND:
		if( LOWORD( wParam ) == IDOK || LOWORD( wParam ) == IDCANCEL )
		{
			EndDialog( hDlg, LOWORD( wParam ) );
			return ( INT_PTR )TRUE;
		}
		break;
	}
	return ( INT_PTR )FALSE;
}
