#include "stdafx.h"
#include "Number_UI.h"
#include "Texture.h"

CNumber_UI::CNumber_UI()
	: CUserInterface()
	, m_bHide( false )
	, m_fSpeed( 0.f )
	, m_iValue( 1000 )
{
}

CNumber_UI::~CNumber_UI()
{
}

HRESULT CNumber_UI::Initialize( CTexture* pTexture, const XMFLOAT4& vPosSize, UINT iNum, float fSpeed )
{
	m_pTexture = pTexture;
	m_vPosSize_Dest = vPosSize;
	m_fSpeed = fSpeed;
	m_vPosSize_Src = XMFLOAT4( vPosSize.x + vPosSize.z * 0.5f, vPosSize.y - vPosSize.w * 0.5f, 0.f, 0.f );
	SetNumber( iNum );

	return S_OK;
}

CNumber_UI* CNumber_UI::Create( CTexture* pTexture, const XMFLOAT4& vPosSize, UINT iNum, float fSpeed )
{
	CNumber_UI*	pNumber_UI = new CNumber_UI;

	if( FAILED( pNumber_UI->Initialize( pTexture, vPosSize, iNum, fSpeed ) ) )
	{
		pNumber_UI->Release();
		pNumber_UI = nullptr;
	}

	return pNumber_UI;
}

void CNumber_UI::Render( ID3D11DeviceContext* pContext )
{
	if( m_bHide ) return;

	m_pTexture->Render( pContext );
	//pContext->Draw( 6, 0 );
	pContext->DrawInstanced( 6, 5, 0, 0 );
}

int CNumber_UI::Update( const float& fTimeDelta )
{
	CUserInterface::Update( fTimeDelta );

	if( m_fLerpTime == 1.f )
		return 0;

	m_fLerpTime += m_fSpeed * fTimeDelta;
	if( m_fLerpTime > 1.f )	m_fLerpTime = 1.f;

	XMVECTOR vLerp = DirectX::XMVectorLerp( XMLoadFloat4( &m_vPosSize_Src ), XMLoadFloat4( &m_vPosSize_Dest ), m_fLerpTime );
	XMFLOAT4 vLerp_Store;
	XMStoreFloat4( &vLerp_Store, vLerp );

	m_mtxPosSize._11 = vLerp_Store.x;
	m_mtxPosSize._21 = vLerp_Store.y;
	m_mtxPosSize._31 = vLerp_Store.z;
	m_mtxPosSize._41 = vLerp_Store.w;

	return 0;
}

DWORD CNumber_UI::Release( void )
{
	CUserInterface::Release();

	delete this;

	return 0;
}

void CNumber_UI::SetNumber( UINT iNum )
{
	if( m_iValue == iNum ) return;
	if( iNum > 12 ) return;

	m_iValue = iNum;

	m_vOption.x = ( float )m_iValue * 1.f / 12.f;
	m_vOption.y = m_vOption.x + 1.f / 12.f;
}
