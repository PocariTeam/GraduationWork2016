#include "stdafx.h"
#include "ThreeD_UI.h"
#include "Texture.h"
#include "Mesh.h"
#include "Animator.h"

CThreeD_UI::CThreeD_UI()
	: CUserInterface()
	, m_pAnimator( nullptr )
{
}

CThreeD_UI::~CThreeD_UI()
{
}

HRESULT CThreeD_UI::Initialize( CMesh* pMesh, CTexture* pTexture, CAnimator* pAnimator, const XMFLOAT4& vPosSize )
{
	m_pTexture = pTexture;
	m_pMesh = pMesh;
	m_pAnimator = pAnimator;

	m_vPosSize_Dest = vPosSize;
	m_vPosSize_Src = XMFLOAT4( vPosSize.x + vPosSize.z * 0.5f, vPosSize.y - vPosSize.w * 0.5f, 0.f, 0.f );

	return S_OK;
}

CThreeD_UI* CThreeD_UI::Create( CMesh* pMesh, CTexture* pTexture, CAnimator* pAnimator, const XMFLOAT4& vPosSize )
{
	CThreeD_UI*	p3D_UI = new CThreeD_UI;

	if( FAILED( p3D_UI->Initialize( pMesh, pTexture, pAnimator, vPosSize ) ) )
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
	m_pAnimator->Render( pContext );
	m_pMesh->Render( pContext );
}

int CThreeD_UI::Update( const float& fTimeDelta )
{
	CUserInterface::Update( fTimeDelta );

	if( m_pAnimator ) m_pAnimator->Update( nullptr, fTimeDelta );

	if( m_fLerpTime == 1.f )
		return 0;

	m_fLerpTime += 5.f * fTimeDelta;
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

	::Safe_Release( m_pAnimator );

	delete this;

	return 0;
}

void CThreeD_UI::SetMesh( CMesh* pMesh )
{
	::Safe_Release( m_pMesh );
	m_pMesh = pMesh;
}

void CThreeD_UI::SetAnimator( CAnimator* pAnimator )
{
	::Safe_Release( m_pAnimator );
	m_pAnimator = pAnimator;
}

DirectX::XMFLOAT4X4 CThreeD_UI::GetWorld()
{
	XMFLOAT4X4 Out;

	XMMATRIX mtxOut = XMMatrixMultiply( XMMatrixScaling( m_mtxPosSize._31, m_mtxPosSize._41, 1.f ), XMMatrixRotationY( XM_PI ) );
	mtxOut = XMMatrixMultiplyTranspose( mtxOut, XMMatrixTranslation( m_mtxPosSize._11, m_mtxPosSize._21, 0.f ) );
	XMStoreFloat4x4( &Out, mtxOut );

	return Out;
}
