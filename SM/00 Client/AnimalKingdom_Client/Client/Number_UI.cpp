#include "stdafx.h"
#include "Number_UI.h"
#include "Texture.h"
#include "TextureMgr.h"

CNumber_UI::CNumber_UI()
	: CUserInterface()
	, m_bHide( false )
	, m_fSpeed( 0.f )
	, m_iValue( 1000 )
	, m_iInstanceCnt( 0 )
{
}

CNumber_UI::~CNumber_UI()
{
}

HRESULT CNumber_UI::Initialize( NUMTYPE eNumType, const XMFLOAT4& vPosSize, UINT iNum, float fSpeed )
{
	switch( eNumType )
	{
	case NUMBER_MY_ID:
		m_pTexture = CTextureMgr::GetInstance()->Clone( "Texture_ID1" );
		break;
	case NUMBER_OTHER_ID:
		m_pTexture = CTextureMgr::GetInstance()->Clone( "Texture_ID0" );
		break;
	case NUMBER_TIME:
		m_pTexture = CTextureMgr::GetInstance()->Clone( "Texture_Number" );
		break;
	}

	m_eType = eNumType;
	m_vPosSize_Dest = vPosSize;
	m_fSpeed = fSpeed;
	m_vPosSize_Src = XMFLOAT4( vPosSize.x + vPosSize.z * 0.5f, vPosSize.y - vPosSize.w * 0.5f, 0.f, 0.f );
	SetNumber( iNum );

	return S_OK;
}

CNumber_UI* CNumber_UI::Create( NUMTYPE eNumType, const XMFLOAT4& vPosSize, UINT iNum, float fSpeed )
{
	CNumber_UI*	pNumber_UI = new CNumber_UI;

	if( FAILED( pNumber_UI->Initialize( eNumType, vPosSize, iNum, fSpeed ) ) )
	{
		pNumber_UI->Release();
		pNumber_UI = nullptr;
	}

	return pNumber_UI;
}

void CNumber_UI::Render( ID3D11DeviceContext* pContext )
{
	if( m_bHide ) return;

	switch( m_eType )
	{
	case NUMBER_MY_ID:
	case NUMBER_OTHER_ID:
		m_pTexture->Render( pContext );
		pContext->Draw( 6, 0 );
		break;
	case NUMBER_TIME:
		pContext->DrawInstanced( 6, m_iInstanceCnt, 0, 0 );
		break;
	}
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

	m_mtxWorld._11 = vLerp_Store.x;
	m_mtxWorld._21 = vLerp_Store.y;
	m_mtxWorld._31 = vLerp_Store.z;
	m_mtxWorld._41 = vLerp_Store.w;

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
	switch( m_eType )
	{
	case NUMBER_MY_ID:
	case NUMBER_OTHER_ID:
		if( m_iValue == iNum ) return;
		( iNum > 99 ) ? m_iValue = 99 : m_iValue = iNum;
		m_vOption.x = float( m_iValue % 10 ) * 0.1f;
		m_vOption.y = m_vOption.x + 1.f / 10.f;
		m_vOption.z = float( int( m_iValue * 0.1f ) * 0.1f );
		m_vOption.w = m_vOption.z + 1.f / 10.f;
		break;
	case NUMBER_TIME:
		if( m_iValue == iNum ) return;
		if( iNum > 12 ) return;
		m_iValue = iNum;
		m_vOption.x = ( float )m_iValue * 1.f / 12.f;
		m_vOption.y = m_vOption.x + 1.f / 12.f;
		m_vOption.z = 0.f;
		m_vOption.w = 1.f;
		break;
	}
}

void CNumber_UI::SetInstanceCnt( UINT iCnt )
{
	m_iInstanceCnt = iCnt;
}