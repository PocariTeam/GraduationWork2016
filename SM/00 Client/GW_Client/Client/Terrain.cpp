#include "stdafx.h"
#include "Terrain.h"
#include "Transform.h"
#include "Mesh.h"
#include "Struct.h"
#include "Texture.h"
#include "GraphicDev.h"
#include "Define.h"
#include "NxActor.h"

HRESULT	CTerrain::Initialize( ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
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

HRESULT CTerrain::Add_Component( CMesh* pMesh, CTexture* pTexture )
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

CTerrain*	CTerrain::Create( ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
	CMesh* pMesh, CTexture* pTexture, XMFLOAT3& vPos )
{
	CTerrain*	pTerrain = new CTerrain;

	if( FAILED( pTerrain->Initialize( pDevice, pContext, pMesh, pTexture, vPos ) ) )
	{
		pTerrain->Release();
		pTerrain = nullptr;
	}

	return pTerrain;
}

int	CTerrain::Update( float fTimeDelta )
{
	//XMVECTOR vDir = XMVector3TransformNormal( XMLoadFloat3( &XMFLOAT3{ 0.f, 0.f, 1.f } ), XMLoadFloat4x4A( &m_pTransCom->m_mtxWorld ) );
	//XMStoreFloat3( &m_pTransCom->m_vDir, vDir );

	CGameObject::Update( fTimeDelta );

	return 0;
}

void CTerrain::Render( CGraphicDev* pGraphicDev )
{
	ID3D11DeviceContext* pContext = pGraphicDev->Get_Context();
	SetConstantBuffer( pContext );
	m_pTexture->Render( pContext );
	m_pMeshCom->Render( pContext );
}

void CTerrain::Render( ID3D11DeviceContext * pContext, DWORD dwInstanceCnt )
{
	SetConstantBuffer( pContext );
	m_pTexture->Render( pContext );
	m_pMeshCom->Render( pContext, dwInstanceCnt );
}

DWORD	CTerrain::Release()
{
	DWORD dwRefCnt = CGameObject::Release();

	if( 0 == dwRefCnt )
		delete this;

	return dwRefCnt;
}

void CTerrain::CreateConstantBuffer( ID3D11Device* pDevice, ID3D11DeviceContext* pContext )
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

void CTerrain::SetConstantBuffer( ID3D11DeviceContext * pContext )
{
	NxF32 mtxWorld[16]{};
	D3D11_MAPPED_SUBRESOURCE MappedSubresource;
	pContext->Map( m_pCBmtxWorld, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );

	VS_CB_WORLD_MATRIX* pStruct = ( VS_CB_WORLD_MATRIX* )MappedSubresource.pData;
	m_pActor->getGlobalPose().getColumnMajor44( mtxWorld );
	memcpy( &pStruct->m_mtxWorld, mtxWorld, sizeof( XMFLOAT4X4 ) );
	XMStoreFloat4x4A( &pStruct->m_mtxWorldInv, XMMatrixInverse( nullptr, XMLoadFloat4x4A( &pStruct->m_mtxWorld ) ) );
	pContext->Unmap( m_pCBmtxWorld, 0 );
	pContext->VSSetConstantBuffers( VS_SLOT_WORLD_MATRIX, 1, &m_pCBmtxWorld );
}