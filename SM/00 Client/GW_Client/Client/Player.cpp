#include "stdafx.h"
#include "Player.h"
#include "Mesh.h"
#include "Struct.h"
#include "Texture.h"
#include "GraphicDev.h"
#include "Define.h"
#include "NxActor.h"

HRESULT	CPlayer::Initialize( ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
	CMesh* pMesh, CTexture* pTexture, XMFLOAT3& vPos )
{
	if( FAILED( Add_Component( pMesh, pTexture ) ) )
		return E_FAIL;

	m_tMaterial.vDiffuse = XMFLOAT4{ 1.0f, 1.0f, 1.f, 1.f };
	m_tMaterial.vSpecular.w = 2.f;
	// m_pTransCom->m_vPos = vPos;
	CreateConstantBuffer( pDevice, pContext );

	return S_OK;
}

HRESULT CPlayer::Add_Component( CMesh* pMesh, CTexture* pTexture )
{
	CComponent*			pComponent = nullptr;

	/*pComponent = m_pTransCom = CTransform::Create();
	if( nullptr == pComponent )
		return E_FAIL;
	m_mapComponent.insert( make_pair( "Transform", pComponent ) );*/

	m_pMeshCom = pMesh;
	m_pTexture = pTexture;

	return S_OK;
}

CPlayer*	CPlayer::Create( ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
	CMesh* pMesh, CTexture* pTexture, XMFLOAT3& vPos )
{
	CPlayer*	pPlayer = new CPlayer;

	if( FAILED( pPlayer->Initialize( pDevice, pContext, pMesh, pTexture, vPos ) ) )
	{
		pPlayer->Release();
		pPlayer = nullptr;
	}

	return pPlayer;
}

int	CPlayer::Update( float fTimeDelta )
{
	/*XMVECTOR vDir = XMVector3TransformNormal( XMLoadFloat3( &XMFLOAT3{ 0.f, 0.f, 1.f } ), XMLoadFloat4x4A( &m_pTransCom->m_mtxWorld ) );
	XMStoreFloat3( &m_pTransCom->m_vDir, vDir );*/

	CGameObject::Update( fTimeDelta );

	KeyCheck( fTimeDelta );

	return 0;
}

void CPlayer::Render( CGraphicDev* pGraphicDev )
{
	ID3D11DeviceContext* pContext = pGraphicDev->Get_Context();
	SetConstantBuffer( pContext );
	m_pTexture->Render( pContext );
	m_pMeshCom->Render( pContext );
}

void CPlayer::Render( ID3D11DeviceContext * pContext, DWORD dwInstanceCnt )
{
	SetConstantBuffer( pContext );
	m_pTexture->Render( pContext );
	m_pMeshCom->Render( pContext, dwInstanceCnt );
}

DWORD	CPlayer::Release()
{
	DWORD dwRefCnt = CGameObject::Release();

	if( 0 == dwRefCnt )
		delete this;

	return dwRefCnt;
}

void	CPlayer::KeyCheck( float fTimeDelta )
{
	float fRealSpeed = m_fSpeed * fTimeDelta;

	/*if( GetAsyncKeyState( VK_NUMPAD8 ) & 0x8000 )
	{
		vPos = XMLoadFloat3( &m_pTransCom->m_vPos );
		vPos = XMVectorAdd( vPos, XMLoadFloat3( &XMFLOAT3( 0.f, 0.f, fRealSpeed ) ) );
		XMStoreFloat3( &m_pTransCom->m_vPos, vPos );
	}

	if( GetAsyncKeyState( VK_NUMPAD5 ) & 0x8000 )
	{
		vPos = XMLoadFloat3( &m_pTransCom->m_vPos );
		vPos = XMVectorAdd( vPos, XMLoadFloat3( &XMFLOAT3( 0.f, 0.f, -fRealSpeed ) ) );
		XMStoreFloat3( &m_pTransCom->m_vPos, vPos );
	}

	if( GetAsyncKeyState( VK_NUMPAD4 ) & 0x8000 )
	{
		vPos = XMLoadFloat3( &m_pTransCom->m_vPos );
		vPos = XMVectorAdd( vPos, XMLoadFloat3( &XMFLOAT3( -fRealSpeed, 0.f, 0.f ) ) );
		XMStoreFloat3( &m_pTransCom->m_vPos, vPos );
	}

	if( GetAsyncKeyState( VK_NUMPAD6 ) & 0x8000 )
	{
		vPos = XMLoadFloat3( &m_pTransCom->m_vPos );
		vPos = XMVectorAdd( vPos, XMLoadFloat3( &XMFLOAT3( fRealSpeed, 0.f, 0.f ) ) );
		XMStoreFloat3( &m_pTransCom->m_vPos, vPos );
	}

	if( GetAsyncKeyState( VK_ADD ) & 0x8000 )
	{
		vPos = XMLoadFloat3( &m_pTransCom->m_vPos );
		vPos = XMVectorAdd( vPos, XMLoadFloat3( &XMFLOAT3( 0.f, fRealSpeed, 0.f ) ) );
		XMStoreFloat3( &m_pTransCom->m_vPos, vPos );
	}

	if( GetAsyncKeyState( VK_SUBTRACT ) & 0x8000 )
	{
		vPos = XMLoadFloat3( &m_pTransCom->m_vPos );
		vPos = XMVectorAdd( vPos, XMLoadFloat3( &XMFLOAT3( 0.f, -fRealSpeed, 0.f ) ) );
		XMStoreFloat3( &m_pTransCom->m_vPos, vPos );
	}*/
}

void CPlayer::CreateConstantBuffer( ID3D11Device* pDevice, ID3D11DeviceContext* pContext )
{
	/* VS ¿ë World */
	D3D11_BUFFER_DESC Buffer_Desc;
	ZeroMemory( &Buffer_Desc, sizeof( Buffer_Desc ) );
	Buffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
	Buffer_Desc.ByteWidth = sizeof( VS_CB_WORLD_MATRIX );
	Buffer_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Buffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	pDevice->CreateBuffer( &Buffer_Desc, NULL, &m_pCBmtxWorld );
}

void CPlayer::SetConstantBuffer( ID3D11DeviceContext * pContext )
{
	if( nullptr == m_pActor ) return;

	NxF32 mtxWorld[16]{};
	D3D11_MAPPED_SUBRESOURCE MappedSubresource;
	pContext->Map( m_pCBmtxWorld, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );

	VS_CB_WORLD_MATRIX* pStruct = ( VS_CB_WORLD_MATRIX* )MappedSubresource.pData;
	m_pActor->getGlobalPose().getRowMajor44( mtxWorld );
	memcpy( &pStruct->m_mtxWorld, mtxWorld, sizeof( XMFLOAT4X4 ) );
	XMStoreFloat4x4A( &pStruct->m_mtxWorldInv, XMMatrixInverse( nullptr, XMLoadFloat4x4A( &pStruct->m_mtxWorld ) ) );
	pContext->Unmap( m_pCBmtxWorld, 0 );
	pContext->VSSetConstantBuffers( VS_SLOT_WORLD_MATRIX, 1, &m_pCBmtxWorld );
}