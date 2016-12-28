#include "stdafx.h"
#include "GraphicDev.h"
#include <assert.h>

CGraphicDev::CGraphicDev( void )
	: m_pDevice( NULL )
	, m_pSwapChain( NULL )
	, m_pDeviceContext( NULL )
	, m_pRenderTargetView( NULL )
	, m_pDepthStencilView( NULL )
	, m_pDepthStencilBuffer( NULL )
{
}

CGraphicDev::~CGraphicDev( void )
{
}

CGraphicDev* CGraphicDev::Create( WINMODE Mode, HWND hWnd, const WORD& wSizeX, const WORD& wSizeY )
{
	CGraphicDev*	pGraphicDev = new CGraphicDev;

	if( FAILED( pGraphicDev->Initialize( Mode, hWnd, wSizeX, wSizeY ) ) )
	{
		pGraphicDev->Release();
		pGraphicDev = nullptr;
		MessageBoxA( 0, "CGraphicDev 객체생성 실패", 0, 0 );

		return nullptr;
	}

	return pGraphicDev;
}

HRESULT CGraphicDev::Initialize( WINMODE Mode, HWND hWnd, const WORD& wSizeX, const WORD& wSizeY )
{
	DWORD dwCreateDeviceFlag = 0;
#if defined(DEBUG) || defined(_DEBUG)
	// dwCreateDeviceFlag |= D3D11_CREATE_DEVICE_DEBUG;	/* 플래그 사용시에는 Win8의 SDK 필요_SDKLayer.dll */
#endif

	D3D_DRIVER_TYPE DriverTypes[] = {
		D3D_DRIVER_TYPE_HARDWARE /* HARDWARE 를 통한 그래픽 가속 */
		, D3D_DRIVER_TYPE_WARP
		, D3D_DRIVER_TYPE_REFERENCE /* SOFTWARE 를 통해 느리지만 하드웨어가 지원하지 않을 때 테스트용 */
	};
	DWORD dwDriverTypes = ARRAYSIZE( DriverTypes );

	D3D_DRIVER_TYPE	  eDriverTypes = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL eFeatureLevel = D3D_FEATURE_LEVEL_11_1;

	for( DWORD i = 0; i < dwDriverTypes; i++ )
	{
		eDriverTypes = DriverTypes[i];

		if( SUCCEEDED( D3D11CreateDevice( NULL /* 기본어댑터를 사용하겠다 */
			, eDriverTypes	/* 위에 참조 */
			, NULL			/* SoftWare 구동기 지정 ( 우리는 하드웨어를 사용한다 ) */
			, dwCreateDeviceFlag
			, NULL	/* NULL을 넣을 시 최고수준을 찾아줌 */
			, NULL  /* 앞의 인자인 FeatureLevel 변수의 원소 개수 ( NULL 일 때는 0 ) */
			, D3D11_SDK_VERSION
			, &m_pDevice
			, &eFeatureLevel /* 앞의 FeatureLevel 인자를 NULL로 보내줄 경우 찾아낸 최고수준을 반환 */
			, &m_pDeviceContext ) ) )
			break;
	}

	if( !m_pDevice || !m_pDeviceContext ) {
		MessageBoxA( 0, "ID3D11Device* 또는 ID3D11DeviceContext* 생성 실패", 0, 0 );
		return E_FAIL;
	}
		
	// 생성과 동시에 RefCnt 를 낮추어준다.
	m_pDevice->Release();

	if( FAILED( Create_SwapChain( Mode, hWnd, wSizeX, wSizeY ) ) ) {
		MessageBoxA( 0, "SwapChain 생성 실패", 0, 0 );
		return E_FAIL;
	}

	if( FAILED( Create_RenderTargetView() ) ) {
		MessageBoxA( 0, "RenderTargetView 생성 실패", 0, 0 );
		return E_FAIL;
	}

	if( FAILED( Create_DepthStencilView( wSizeX, wSizeY ) ) ) {
		MessageBoxA( 0, "StencilView 생성 실패", 0, 0 );
		return E_FAIL;
	}

	m_pDeviceContext->OMSetRenderTargets( 1, &m_pRenderTargetView, m_pDepthStencilView );

	D3D11_VIEWPORT	ViewPort;
	ViewPort.TopLeftX = 0;
	ViewPort.TopLeftY = 0;

	ViewPort.Width = ( FLOAT )wSizeX;
	ViewPort.Height = ( FLOAT )wSizeY;
	ViewPort.MinDepth = 0.f;
	ViewPort.MaxDepth = 1.f;

	m_pDeviceContext->RSSetViewports( 1, &ViewPort );

	return S_OK;
}

HRESULT CGraphicDev::Create_SwapChain( WINMODE Mode, HWND hWnd, const WORD& wSizeX, const WORD& wSizeY )
{
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	ZeroMemory( &SwapChainDesc, sizeof( SwapChainDesc ) );

	// 백버퍼 하나만 사용
	SwapChainDesc.BufferCount = 1;

	// 백버퍼의 너비와 높이
	SwapChainDesc.BufferDesc.Width = wSizeX;
	SwapChainDesc.BufferDesc.Height = wSizeY;

	// 백버퍼의 새로고침 비율 설정( 장치로부터 원래 받아와야 함 ) 
	SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;

	// 일반적인 32bit의 서피스를 지정
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// 스캔라인의 정렬과 스캔라이닝을 '지정되지 않음'으로 설정
	SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// 4XMSAA 를 사용 사용수준 0 보다 크도록 강제
	UINT dw4xMsaaQuality = 0;
	if( FAILED( m_pDevice->CheckMultisampleQualityLevels( DXGI_FORMAT_R8G8B8A8_UNORM, 4, &dw4xMsaaQuality ) ) )
	{
		// 멀티샘플링을 끔
		SwapChainDesc.SampleDesc.Count = 1;
		SwapChainDesc.SampleDesc.Quality = 0;
	}

	else
	{
		// 멀티샘플링 적용
		SwapChainDesc.SampleDesc.Count = 4;
		SwapChainDesc.SampleDesc.Quality = dw4xMsaaQuality - 1;
	}

	assert( dw4xMsaaQuality > 0 );	// 0 보다 작으면 종료

									// 백버퍼의 용도 설정
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// 렌더링 될 윈도우의 핸들 설정
	SwapChainDesc.OutputWindow = hWnd;

	// 윈모드 or 풀모드
	SwapChainDesc.Windowed = Mode;

	// 출력된 이후 백퍼버의 내용은 버림
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// 추가 옵션 플래그는 사용하지 않음
	SwapChainDesc.Flags = 0;

	//////////////////////////////////////////////////////////////////////////
	////////////////// 실질적인 SwapChain 생성 ///////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	// IDXGIFactory 를 이용하여 SwapChain 을 생성하는데 그 전에 해야할 과정들
	IDXGIDevice* dxgiDevice = NULL;
	if( FAILED( m_pDevice->QueryInterface( __uuidof( IDXGIDevice ), ( void** )&dxgiDevice ) ) )
		return E_FAIL;

	IDXGIAdapter1* dxgiAdaptor = NULL;
	if( FAILED( dxgiDevice->GetParent( __uuidof( IDXGIAdapter1 ), ( void** )&dxgiAdaptor ) ) )
		return E_FAIL;

	// IDXGIFactory 를 얻어온다
	IDXGIFactory* dxgiFatory = 0;
	if( FAILED( dxgiAdaptor->GetParent( __uuidof( IDXGIFactory ), ( void** )&dxgiFatory ) ) )
		return E_FAIL;

	// SwapChain 생성
	if( FAILED( dxgiFatory->CreateSwapChain( m_pDevice, &SwapChainDesc, &m_pSwapChain ) ) )
		return E_FAIL;

	dxgiDevice->Release();
	dxgiAdaptor->Release();
	dxgiFatory->Release();

	return S_OK;
}

void CGraphicDev::Release( void )
{
	m_pDeviceContext->ClearState();

	DWORD	dwRef{ 0 };
	char	szErrorMessage[64]{ "" };

	if( nullptr != m_pRenderTargetView ) {
		dwRef = m_pRenderTargetView->Release();
		if( 0 != dwRef )
		{
			sprintf_s( szErrorMessage, 64, "RenderTargetView RefCnt = %d", dwRef );
			MessageBoxA( 0, szErrorMessage, 0, 0 );
		}
		else
			m_pRenderTargetView = nullptr;
	}

	if( nullptr != m_pSwapChain ) {
		dwRef = m_pSwapChain->Release();
		if( 0 != dwRef )
		{
			sprintf_s( szErrorMessage, 64, "SwapChain RefCnt = %d", dwRef );
			MessageBoxA( 0, szErrorMessage, 0, 0 );
		}
		else
			m_pSwapChain = nullptr;
	}

	if( nullptr != m_pDeviceContext ) {
		dwRef = m_pDeviceContext->Release();
		if( 0 != dwRef )
		{
			sprintf_s( szErrorMessage, 64, "DeviceContext RefCnt = %d", dwRef );
			MessageBoxA( 0, szErrorMessage, 0, 0 );
		}
		else
			m_pDeviceContext = nullptr;
	}

	if( nullptr != m_pDevice ) {
		do {
			dwRef = m_pDevice->Release();
		} while( 1 != dwRef );

		if( 1 < dwRef )
		{
			sprintf_s( szErrorMessage, 64, "Device RefCnt = %d", dwRef );
			MessageBoxA( 0, szErrorMessage, 0, 0 );
		}
		else
			m_pDevice = nullptr;
	}

	delete this;
}

HRESULT CGraphicDev::Create_RenderTargetView()
{
	// 렌더 타겟 뷰 생성
	ID3D11Texture2D*	pBackBuffer = NULL;

	// 교환사슬이 현재 가르키는 포인터를 얻는다
	if( FAILED( m_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer ) ) )
		return E_FAIL;

	if( FAILED( m_pDevice->CreateRenderTargetView( pBackBuffer, NULL, &m_pRenderTargetView ) ) )
		return E_FAIL;

	// GetBuffer 를 사용하면 RefCnt 가 1개 증가하기 때문에 제거
	pBackBuffer->Release();

	return S_OK;
}

HRESULT CGraphicDev::Create_DepthStencilView( const WORD& wSizeX, const WORD& wSizeY )
{
	D3D11_TEXTURE2D_DESC DepthBufferDesc;
	ZeroMemory( &DepthBufferDesc, sizeof( DepthBufferDesc ) );

	DepthBufferDesc.Width = wSizeX;
	DepthBufferDesc.Height = wSizeY;
	// 깊이.스텐실 버퍼를 위한 텍스쳐는 밉맵 수준이 하나면 됨
	DepthBufferDesc.MipLevels = 1;
	// 텍스쳐 배열의 텍스쳐 개수 - 깊이.스텐실 버퍼에서는 텍스쳐 하나만 필요
	DepthBufferDesc.ArraySize = 1;

	DepthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 텍스쳐의 용도를 나타냄
	DepthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	DepthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	// 스텐실 버퍼에서는 아래 두 개 다 0
	DepthBufferDesc.CPUAccessFlags = 0;
	DepthBufferDesc.MiscFlags = 0;

	// 백버퍼와 일치시켜줘야함
	// 4XMSAA 를 사용 사용수준 0 보다 크도록 강제
	UINT dw4xMsaaQuality = 0;
	if( UINT( m_pDevice->CheckMultisampleQualityLevels( DXGI_FORMAT_R8G8B8A8_UNORM, 4, &dw4xMsaaQuality ) ) )
	{
		// 멀티샘플링을 끔
		DepthBufferDesc.SampleDesc.Count = 1;
		DepthBufferDesc.SampleDesc.Quality = 0;
	}

	else
	{
		// 멀티샘플링 적용
		DepthBufferDesc.SampleDesc.Count = 4;
		DepthBufferDesc.SampleDesc.Quality = dw4xMsaaQuality - 1;
	}

	if( FAILED( m_pDevice->CreateTexture2D( &DepthBufferDesc, NULL, &m_pDepthStencilBuffer ) ) )
		return E_FAIL;

	if( FAILED( m_pDevice->CreateDepthStencilView( m_pDepthStencilBuffer, NULL, &m_pDepthStencilView ) ) )
		return E_FAIL;

	return S_OK;
}

void CGraphicDev::BeginScene( void )
{
	if( NULL == m_pDeviceContext )
		return;
	if( NULL == m_pSwapChain )
		return;

	float fColor[4] = { 0.f
		, 0.f
		, 0.3f
		, 0.f
	};

	// 백버퍼를 파란색으로 초기화
	m_pDeviceContext->ClearRenderTargetView( m_pRenderTargetView, fColor );

	// 깊이버퍼를 1.f로, 스텐실 버퍼를 0으로 지운다
	m_pDeviceContext->ClearDepthStencilView( m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL
		, 1.f, 0 );
}

void CGraphicDev::EndScene( void )
{
	m_pSwapChain->Present( 0, 0 );
}

void CGraphicDev::Clear_DepthBuffer( void )
{
	m_pDeviceContext->ClearDepthStencilView( m_pDepthStencilView, D3D11_CLEAR_DEPTH
		, 1.f, 0 );
}

void CGraphicDev::Clear_StencilBuffer( void )
{
	m_pDeviceContext->ClearDepthStencilView( m_pDepthStencilView, D3D11_CLEAR_STENCIL
		, 1.f, 0 );
}

void CGraphicDev::Set_RenderTargetView( ID3D11DeviceContext* pOut )
{
	if( nullptr == pOut )
		return;

	D3D11_VIEWPORT	ViewPort;
	ViewPort.TopLeftX = 0;
	ViewPort.TopLeftY = 0;

	ViewPort.Width = ( FLOAT )WIN_WIDTH;
	ViewPort.Height = ( FLOAT )WIN_HEIGHT;
	ViewPort.MinDepth = 0.f;
	ViewPort.MaxDepth = 1.f;

	pOut->OMSetRenderTargets( 1, &m_pRenderTargetView, m_pDepthStencilView );
	pOut->RSSetViewports( 1, &ViewPort );
}

void CGraphicDev::Disconnect_RenderTargetView( ID3D11DeviceContext* pOut )
{
	if( nullptr == pOut )
		return;

	pOut->OMSetRenderTargets( 0, nullptr, nullptr );
}
