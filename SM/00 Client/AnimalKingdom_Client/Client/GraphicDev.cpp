#include "stdafx.h"
#include "GraphicDev.h"
#include <assert.h>

CGraphicDev::CGraphicDev( void )
	: m_pDevice( nullptr )
	, m_pSwapChain( nullptr )
	, m_pDeviceContext( nullptr )
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
		MessageBoxA( 0, "CGraphicDev ��ü���� ����", 0, 0 );

		return nullptr;
	}

	return pGraphicDev;
}

HRESULT CGraphicDev::Initialize( WINMODE Mode, HWND hWnd, const WORD& wSizeX, const WORD& wSizeY )
{
	DWORD dwCreateDeviceFlag = 0;
#if defined(DEBUG) || defined(_DEBUG)
	dwCreateDeviceFlag |= D3D11_CREATE_DEVICE_DEBUG;	/* �÷��� ���ÿ��� Win8�� SDK �ʿ�_SDKLayer.dll */
#endif

	D3D_DRIVER_TYPE DriverTypes[] = {
		D3D_DRIVER_TYPE_HARDWARE /* HARDWARE �� ���� �׷��� ���� */
		, D3D_DRIVER_TYPE_WARP
		, D3D_DRIVER_TYPE_REFERENCE /* SOFTWARE �� ���� �������� �ϵ��� �������� ���� �� �׽�Ʈ�� */
	};
	DWORD dwDriverTypes = ARRAYSIZE( DriverTypes );

	D3D_DRIVER_TYPE	  eDriverTypes = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL eFeatureLevel = D3D_FEATURE_LEVEL_11_1;

	for( DWORD i = 0; i < dwDriverTypes; i++ )
	{
		eDriverTypes = DriverTypes[i];
		if( SUCCEEDED( D3D11CreateDevice( NULL /* �⺻����͸� ����ϰڴ� */
			, eDriverTypes				       /* ���� ���� */
			, nullptr						   /* SoftWare ������ ���� ( �츮�� �ϵ��� ����Ѵ� ) */
			, dwCreateDeviceFlag
			, nullptr						   /* NULL�� ���� �� �ְ������ ã���� */
			, NULL							   /* ���� ������ FeatureLevel ������ ���� ���� ( NULL �� ���� 0 ) */
			, D3D11_SDK_VERSION
			, &m_pDevice
			, &eFeatureLevel				   /* ���� FeatureLevel ���ڸ� NULL�� ������ ��� ã�Ƴ� �ְ������ ��ȯ */
			, &m_pDeviceContext ) ) )
			break;
	}

	if( !m_pDevice || !m_pDeviceContext ) {
		MessageBoxA( 0, "ID3D11Device* �Ǵ� ID3D11DeviceContext* ���� ����", 0, 0 );
		return E_FAIL;
	}

	// ������ ���ÿ� RefCnt �� ���߾��ش�.
	m_pDevice->Release();

	if( FAILED( Create_SwapChain( Mode, hWnd, wSizeX, wSizeY ) ) ) {
		MessageBoxA( 0, "SwapChain ���� ����", 0, 0 );
		return E_FAIL;
	}

	SetViewport( wSizeX, wSizeY );

	return S_OK;
}

void CGraphicDev::SetViewport( const WORD& wSizeX, const WORD& wSizeY )
{
	D3D11_VIEWPORT	ViewPort;
	ZeroMemory( &ViewPort, sizeof( D3D11_VIEWPORT ) );

	ViewPort.Width = ( FLOAT )wSizeX;
	ViewPort.Height = ( FLOAT )wSizeY;
	ViewPort.MaxDepth = 1.f;

	m_pDeviceContext->RSSetViewports( 1, &ViewPort );
}

HRESULT CGraphicDev::Create_SwapChain( WINMODE Mode, HWND hWnd, const WORD& wSizeX, const WORD& wSizeY )
{
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	ZeroMemory( &SwapChainDesc, sizeof( SwapChainDesc ) );

	// ����� �ϳ��� ���
	SwapChainDesc.BufferCount = 1;

	// ������� �ʺ�� ����
	SwapChainDesc.BufferDesc.Width = wSizeX;
	SwapChainDesc.BufferDesc.Height = wSizeY;

	// ������� ���ΰ�ħ ���� ����( ��ġ�κ��� ���� �޾ƿ;� �� ) 
	SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;

	// �Ϲ����� 32bit�� ���ǽ��� ����
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// ��ĵ������ ���İ� ��ĵ���̴��� '�������� ����'���� ����
	SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// 4XMSAA �� ��� ������ 0 ���� ũ���� ����
	//UINT dw4xMsaaQuality = 0;
	//if( FAILED( m_pDevice->CheckMultisampleQualityLevels( DXGI_FORMAT_R8G8B8A8_UNORM, 4, &dw4xMsaaQuality ) ) )
	//{
		// ��Ƽ���ø��� ��
		SwapChainDesc.SampleDesc.Count = 1;
		SwapChainDesc.SampleDesc.Quality = 0;
	//}

	//else
	//{
		// ��Ƽ���ø� ����
	//	SwapChainDesc.SampleDesc.Count = 4;
	//	SwapChainDesc.SampleDesc.Quality = dw4xMsaaQuality - 1;
	//}

	//assert( dw4xMsaaQuality > 0 );	// 0 ���� ������ ����

									// ������� �뵵 ����
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// ������ �� �������� �ڵ� ����
	SwapChainDesc.OutputWindow = hWnd;

	// ����� or Ǯ���
	SwapChainDesc.Windowed = Mode;

	// ��µ� ���� ���۹��� ������ ����
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// �߰� �ɼ� �÷��״� ������� ����
	SwapChainDesc.Flags = 0;

	//////////////////////////////////////////////////////////////////////////
	////////////////// �������� SwapChain ���� ///////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	// IDXGIFactory �� �̿��Ͽ� SwapChain �� �����ϴµ� �� ���� �ؾ��� ������
	IDXGIDevice* dxgiDevice = NULL;
	if( FAILED( m_pDevice->QueryInterface( __uuidof( IDXGIDevice ), ( void** )&dxgiDevice ) ) )
		return E_FAIL;

	IDXGIAdapter1* dxgiAdaptor = NULL;
	if( FAILED( dxgiDevice->GetParent( __uuidof( IDXGIAdapter1 ), ( void** )&dxgiAdaptor ) ) )
		return E_FAIL;

	// IDXGIFactory �� ���´�
	IDXGIFactory* dxgiFatory = 0;
	if( FAILED( dxgiAdaptor->GetParent( __uuidof( IDXGIFactory ), ( void** )&dxgiFatory ) ) )
		return E_FAIL;

	// SwapChain ����
	if( FAILED( dxgiFatory->CreateSwapChain( m_pDevice, &SwapChainDesc, &m_pSwapChain ) ) )
		return E_FAIL;

	dxgiDevice->Release();
	dxgiAdaptor->Release();
	dxgiFatory->Release();

	return S_OK;
}

DWORD CGraphicDev::Release( void )
{
	m_pDeviceContext->ClearState();

	DWORD	dwRef{ 0 };
	char	szErrorMessage[64]{ "" };

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
		dwRef = m_pDevice->Release();

		if( 1 < dwRef )
		{
			sprintf_s( szErrorMessage, 64, "Device RefCnt = %d", dwRef );
			MessageBoxA( 0, szErrorMessage, 0, 0 );
		}
		else
			m_pDevice = nullptr;
	}

	delete this;

	return dwRef;
}


void CGraphicDev::EndScene( void )
{
	m_pSwapChain->Present( 0, 0 );
}