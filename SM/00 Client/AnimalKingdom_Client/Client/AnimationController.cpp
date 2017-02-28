#include "stdafx.h"
#include "AnimationController.h"
#include "Animation.h"
#include "Function.h"
#include <fstream>
#include "Define.h"
#include "ChameleonIdleAnimation.h"
#include "ChameleonGlobalAnimation.h"
#include "GameObject.h"
#include <NxActor.h>
#include <NxShape.h>
#include <NxMat34.h>

void CAnimationController::Update( const float& fTimeDelta )
{
	if( m_pPreviousAnimation ) m_pPreviousAnimation->Execute( m_pOwner, fTimeDelta, m_fTimePos );
	if( m_pCurrentAnimation )
	{
		DWORD i = 0;
		m_pCurrentAnimation->Execute( m_pOwner, fTimeDelta, m_fTimePos );
		XMFLOAT4X4* pWorld = new XMFLOAT4X4[ m_dwJointCnt ];
		m_pCurrentAnimation->GetAnimationMatrix( pWorld, m_fTimePos );
		NxActor* pActor = m_pOwner->GetActor();
		NxU32 iShapeCnt = pActor->getNbShapes();
		NxShape* const* dpShape = pActor->getShapes();

		for( NxU32 j = 0; j < iShapeCnt; ++j )
		{
			for( ; i < m_dwJointCnt; ++i )
				if( 0 == strcmp( m_pArrJointName[ i ].c_str(), dpShape[ j ]->getName() ) )
					break;

			NxMat34 mtxLocal;
			NxF32	fLocal[ 16 ];
			memcpy( fLocal, &pWorld[ i ], sizeof( XMFLOAT4X4 ) );
			mtxLocal.setRowMajor44( fLocal );

			dpShape[j]->setLocalPose( mtxLocal );
			i = 0;
		}

		delete[] pWorld;
	}
}

void CAnimationController::Change_Animation( CAnimation* pAnimation )
{
	if( nullptr == pAnimation )
		MessageBox( nullptr, "Change_Animation Parameter is nullptr", nullptr, MB_OK );

	m_pPreviousAnimation = m_pCurrentAnimation;
	m_pCurrentAnimation->Exit( m_pOwner, 0.f, m_fTimePos );
	m_pCurrentAnimation = pAnimation;
	m_fTimePos = 0.f;
	m_pCurrentAnimation->Enter( m_pOwner, 0.f, m_fTimePos );
}

void CAnimationController::SetPreviousAnimation( CAnimation* pAnimation )
{
	m_pPreviousAnimation = pAnimation;
}

void CAnimationController::SetGlobalAnimation( CAnimation* pAnimation )
{
	m_pGlobalAnimation = pAnimation;
}

void CAnimationController::SetCurrentAnimation( CAnimation* pAnimation )
{
	m_pPreviousAnimation = pAnimation;
}

void CAnimationController::RevertToPreviousAnimation( void )
{
	Change_Animation( m_pPreviousAnimation );
}

CAnimationController::CAnimationController( void )
	: CBase()
	, m_pOwner( nullptr )
	, m_fTimePos( 0.f )
	, m_dwJointCnt( 0 )
	, m_pCurrentAnimation( nullptr )
	, m_pPreviousAnimation( nullptr )
	, m_pGlobalAnimation( nullptr )
	, m_pArrJointName( nullptr )
	, m_pConstantBufferAnimation( nullptr )
{
}

CAnimationController::CAnimationController( const CAnimationController& Instance, CGameObject* pOwner )
	: CBase( Instance )
	, m_pOwner( pOwner )
	, m_fTimePos( 0.f )
	, m_dwJointCnt( Instance.m_dwJointCnt )
	, m_pCurrentAnimation( Instance.m_pCurrentAnimation )
	, m_pPreviousAnimation( Instance.m_pPreviousAnimation )
	, m_pGlobalAnimation( Instance.m_pGlobalAnimation )
	, m_pArrJointName( Instance.m_pArrJointName )
	, m_pConstantBufferAnimation( Instance.m_pConstantBufferAnimation )
{
}

CAnimationController* CAnimationController::Clone( CGameObject* pOwner )
{
	return new CAnimationController( *this, pOwner );
}

CAnimationController* CAnimationController::Create( ID3D11Device* pDevice, const char* pFilePath )
{
	CAnimationController*	pAnimationController = new CAnimationController;

	if( FAILED( pAnimationController->Initialize( pDevice, pFilePath ) ) )
	{
		pAnimationController->Release();
		pAnimationController = nullptr;
	}
	return pAnimationController;
}

HRESULT CAnimationController::Initialize( ID3D11Device* pDevice, const char* pFilePath )
{
	if( FAILED( Load( pFilePath ) ) )
		return E_FAIL;

	if( FAILED( CreateConstantBuffer( pDevice ) ) )
		return E_FAIL;

	m_pCurrentAnimation = CChameleonIdleAnimation::GetInstance();
	m_pGlobalAnimation = CChameleonGlobalAnimation::GetInstance();

	return S_OK;
}

HRESULT CAnimationController::Load( const char* pFilePath )
{
	ifstream pIn( pFilePath );

	char szType{};
	pIn >> szType;

	if( 'j' == szType )
	{
		pIn >> m_dwJointCnt;


		if( nullptr == m_pArrJointName )
			m_pArrJointName = new string[ m_dwJointCnt ];
		for( DWORD i = 0; i < m_dwJointCnt; ++i )
		{
			pIn >> m_pArrJointName[ i ];
		}
	}

	pIn.close();
	return S_OK;
}

HRESULT CAnimationController::CreateConstantBuffer( ID3D11Device* pDevice )
{
	D3D11_BUFFER_DESC Buffer_Desc;
	ZeroMemory( &Buffer_Desc, sizeof( Buffer_Desc ) );
	Buffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
	Buffer_Desc.ByteWidth = sizeof( XMFLOAT4X4 ) * m_dwJointCnt;
	Buffer_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Buffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	pDevice->CreateBuffer( &Buffer_Desc, nullptr, &m_pConstantBufferAnimation );

	return S_OK;
}

DWORD CAnimationController::Release( void )
{
	DWORD dwRefCnt = CBase::Release();

	if( 0 == dwRefCnt )
	{
		::Safe_Delete_Array( m_pArrJointName );
		::Safe_Release( m_pConstantBufferAnimation );
	}
		
	delete this;

	return 0;
}

void CAnimationController::Render( ID3D11DeviceContext* pContext )
{
	D3D11_MAPPED_SUBRESOURCE MappedSubresource;
	pContext->Map( m_pConstantBufferAnimation, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );
	XMFLOAT4X4* pWorld = ( XMFLOAT4X4* )MappedSubresource.pData;
	m_pCurrentAnimation->GetAnimationMatrix( pWorld, m_fTimePos );
	pContext->Unmap( m_pConstantBufferAnimation, 0 );
	pContext->VSSetConstantBuffers( SLOT_ANIMATION, 1, &m_pConstantBufferAnimation );
}
