#include "stdafx.h"
#include "ThreeD_UI.h"
#include "Texture.h"
#include "Mesh.h"

CThreeD_UI::CThreeD_UI()
{
}

CThreeD_UI::~CThreeD_UI()
{
}

HRESULT CThreeD_UI::Initialize( const XMFLOAT4& vOffset )
{
	m_vPosSize_Dest = vOffset;
	m_vPosSize_Src = XMFLOAT4( vOffset.x + vOffset.z * 0.5f, vOffset.y - vOffset.w * 0.5f, 0.f, 0.f );

	return S_OK;
}

CThreeD_UI* CThreeD_UI::Create( const XMFLOAT4& vOffset )
{
	CThreeD_UI*	p3D_UI = new CThreeD_UI;

	if( FAILED( p3D_UI->Initialize( vOffset ) ) )
	{
		p3D_UI->Release();
		p3D_UI = nullptr;
	}

	return p3D_UI;
}

void CThreeD_UI::Render( ID3D11DeviceContext* pContext )
{
	if( nullptr == m_pTexture ) return;
	m_pTexture->Render( pContext );
	m_pMesh->Render( pContext );
}

int CThreeD_UI::Update( const float& fTimeDelta )
{
	CUserInterface::Update( fTimeDelta );

	if( m_fLerpTime == 1.f )
		return 0;

	m_fLerpTime += 2.f * fTimeDelta;
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

DWORD CThreeD_UI::Release( void )
{
	CUserInterface::Release();

	delete this;

	return 0;
}
