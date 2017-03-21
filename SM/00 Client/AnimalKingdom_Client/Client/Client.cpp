// Client.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Client.h"
#include "MainFrm.h"
#include "Timer.h"
#include "Function.h"
#include "NetworkMgr.h"


#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[ MAX_LOADSTRING ];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[ MAX_LOADSTRING ];            // 기본 창 클래스 이름입니다.

/* Client MainFrame Object */
CMainFrm*	g_pMainFrm{ nullptr };
WORD g_wWinsizeX = 1024;
WORD g_wWinsizeY = 768;

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
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

	// TODO: 여기에 코드를 입력합니다.
	// _CrtSetBreakAlloc( 66753 );

	// 전역 문자열을 초기화합니다.
	LoadStringW( hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING );
	LoadStringW( hInstance, IDC_CLIENT, szWindowClass, MAX_LOADSTRING );
	MyRegisterClass( hInstance );

	// 응용 프로그램 초기화를 수행합니다.
	if( !InitInstance( hInstance, nCmdShow ) )
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators( hInstance, MAKEINTRESOURCE( IDC_CLIENT ) );

	MSG msg;

	/* Timer for Fixed Frame */
	CTimer* pFixedTimer{ CTimer::Create() };
	CTimer* pRealTimer{ CTimer::Create() };

	float fFrameLimit{ 1.f / 60.f };
	float fFrameTime{ 0.f };

	// 기본 메시지 루프입니다.
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
			//printf( "%f\n", fFrameTime );
			if( fFrameTime >= fFrameLimit )	// d
			{
				pRealTimer->Calculate_TimeDelta();
				if( -1 == g_pMainFrm->Update( pRealTimer->GetTimeDelta() ) )
					break;
				g_pMainFrm->Render();
				fFrameTime = 0.f;
				//fFrameTime -= fFrameLimit;
			}
		}
	}

	::Safe_Release( g_pMainFrm );
	::Safe_Release( pFixedTimer );
	::Safe_Release( pRealTimer );

#ifdef _DEBUG
	FreeConsole();
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	return ( int )msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
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
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance( HINSTANCE hInstance, int nCmdShow )
{
	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

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
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_CREATE:
		break;
	case WM_SOCKET:
		CNetworkMgr::GetInstance()->processSocketMessage(hWnd, lParam);
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
		// 메뉴 선택을 구문 분석합니다.
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
		// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.
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

// 정보 대화 상자의 메시지 처리기입니다.
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
