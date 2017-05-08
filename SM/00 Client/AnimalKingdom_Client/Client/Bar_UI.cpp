#include "stdafx.h"
#include "Bar_UI.h"
#include "Texture.h"

CBar_UI::CBar_UI()
	: CUserInterface()
	, m_bHide( false )
	, m_iMax( 0 )
	, m_pData( nullptr )
	, m_fSpeed( 0.f )
{
}

CBar_UI::~CBar_UI()
{
}

HRESULT CBar_UI::Initialize( CTexture* pTexture, const XMFLOAT4& vPosSize, int* pDest, int iMax, float fSpeed )
{
	m_pTexture = pTexture;
	m_vPosSize_Dest = vPosSize;
	m_fSpeed = fSpeed;
	m_vPosSize_Src = XMFLOAT4( vPosSize.x + vPosSize.z * 0.5f, vPosSize.y - vPosSize.w * 0.5f, 0.f, 0.f );
	m_iMax = iMax;
	m_pData = pDest;

	return S_OK;
}

CBar_UI* CBar_UI::Create( CTexture* pTexture, const XMFLOAT4& vPosSize, int* pDest, int iMax, float fSpeed )
{
	CBar_UI*	pBar_UI = new CBar_UI;

	if( FAILED( pBar_UI->Initialize( pTexture, vPosSize, pDest, iMax, fSpeed ) ) )
	{
		pBar_UI->Release();
		pBar_UI = nullptr;
	}

	return pBar_UI;
}

void CBar_UI::Render( ID3D11DeviceContext* pContext )
{
	if( m_bHide ) return;

	m_pTexture->Render( pContext );
	pContext->Draw( 6, 0 );
}

int CBar_UI::Update( const float& fTimeDelta )
{
	CUserInterface::Update( fTimeDelta );

	if( m_fLerpTime == 1.f )
	{
		m_mtxWorld._31 = m_vPosSize_Dest.z * float( *m_pData ) / float( m_iMax );
		return 0;
	}

	m_fLerpTime += m_fSpeed * fTimeDelta;
	if( m_fLerpTime > 1.f )	m_fLerpTime = 1.f;

	XMVECTOR vLerp = DirectX::XMVectorLerp( XMLoadFloat4( &m_vPosSize_Src ), XMLoadFloat4( &m_vPosSize_Dest ), m_fLerpTime );
	XMFLOAT4 vLerp_Store;
	XMStoreFloat4( &vLerp_Store, vLerp );

	m_mtxWorld._11 = vLerp_Store.x;
	m_mtxWorld._21 = vLerp_Store.y;
	m_mtxWorld._31 = vLerp_Store.z;
	m_mtxWorld._41 = vLerp_Store.w;

	return 0;
}

DWORD CBar_UI::Release( void )
{
	CUserInterface::Release();

	delete this;

	return 0;
}
