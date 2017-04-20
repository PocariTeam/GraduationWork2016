#include "stdafx.h"
#include "Animator.h"
#include "AnimationSet.h"
#include "Function.h"
#include "Functor.h"
#include <fstream>
#include "Define.h"
#include "GameObject.h"
#include <NxActor.h>
#include <NxShape.h>
#include <NxMat34.h>
#include "Player.h"
#include <NxController.h>
#include "Mathematics.h"

CAnimator::CAnimator( void )
	: CBase()
	, m_dwJointCnt( 0 )
	, m_pArrJointName( nullptr )
	, m_pConstantBufferAnimation( nullptr )
	, m_pCurrentAnimationSet( nullptr )
	, m_pPreviousAnimationSet( nullptr )
	, m_bPause( false )
{
}

CAnimator::CAnimator( const CAnimator& Instance )
	: CBase( Instance )
	, m_dwJointCnt( Instance.m_dwJointCnt )
	, m_pArrJointName( Instance.m_pArrJointName )
	, m_pConstantBufferAnimation( Instance.m_pConstantBufferAnimation )
	, m_pPreviousAnimationSet( nullptr )
	, m_bPause( false )
{
	m_pCurrentAnimationSet = Instance.m_vecAnimationSet[ 0 ];
	m_mapEvent = m_pCurrentAnimationSet->GetEvent();
	for( size_t i = 0; i < Instance.m_vecAnimationSet.size(); ++i )
		m_vecAnimationSet.push_back( Instance.m_vecAnimationSet[ i ]->Clone() );
}

CAnimator* CAnimator::Clone( void )
{
	return new CAnimator( *this );
}

CAnimator* CAnimator::Create( ID3D11Device* pDevice, const char* pFilePath )
{
	CAnimator*	pAnimationController = new CAnimator;

	if( FAILED( pAnimationController->Initialize( pDevice, pFilePath ) ) )
	{
		pAnimationController->Release();
		pAnimationController = nullptr;
	}
	return pAnimationController;
}

HRESULT CAnimator::Initialize( ID3D11Device* pDevice, const char* pFilePath )
{
	if( FAILED( Load( pFilePath ) ) )
		return E_FAIL;

	if( FAILED( CreateConstantBuffer( pDevice ) ) )
		return E_FAIL;

	return S_OK;
}

HRESULT CAnimator::Load( const char* pFilePath )
{
	ifstream pIn( pFilePath );

	char szType{};
	pIn >> szType;

	if( 'j' == szType )
	{
		pIn >> m_dwJointCnt;
		m_pArrJointName = new string[ m_dwJointCnt ];

		for( DWORD i = 0; i < m_dwJointCnt; ++i )
			pIn >> m_pArrJointName[ i ];
	}

	pIn.close();
	return S_OK;
}

void CAnimator::ConnectActorShape( CGameObject* pOwner )
{
	DWORD i = 0;
	XMFLOAT4X4*	pWorld{ new XMFLOAT4X4[ m_dwJointCnt ] };
	m_pCurrentAnimationSet->GetAnimationMatrix( pWorld );
	NxController* pCharacterController = ( ( CPlayer* )pOwner )->GetCharacterController();
	NxMat34*	pActorsOriginPose = ( ( CPlayer* )pOwner )->GetActorsOriginPose();
	NxActor** dpActorArray = ( NxActor** )pCharacterController->getUserData();

	DWORD dwActorCnt = ( ( CPlayer* )pOwner )->GetActorCnt();

	// 상수 6은 각 캐릭터별 액터 수 ( 변경해야할 부분 - 인자로 )
	for( NxU32 j = 0; j < dwActorCnt; ++j )
	{
		for( ; i < m_dwJointCnt; ++i )
			if( 0 == strcmp( m_pArrJointName[ i ].c_str(), dpActorArray[ j ]->getName() ) )
				break;

		NxMat34 mtxLocal;
		XMFLOAT4X4 mtxTemp;
		XMMATRIX mtxLoadOrigin, mtxLoadAnimation{};

		mtxLoadOrigin = CMathematics::ConvertToXMMatrix( &pActorsOriginPose[ j ] );
		mtxLoadAnimation = XMLoadFloat4x4( &pWorld[ i ] );

		XMMATRIX mtxResult = XMMatrixMultiply( XMLoadFloat4x4( &pOwner->GetWorld() ), XMMatrixMultiply( mtxLoadAnimation, mtxLoadOrigin ) );
		dpActorArray[ j ]->moveGlobalPose( CMathematics::ConvertToNxMat34( mtxResult ) );
		i = 0;
	}

	delete[] pWorld;
	pWorld = nullptr;
}

HRESULT CAnimator::Add( STATE eState, const char* pFilePath )
{
	if( m_vecAnimationSet.size() != ( size_t )eState )
		return E_FAIL;

	m_vecAnimationSet.emplace_back( CAnimationSet::Create( pFilePath ) );

	return S_OK;
}

void CAnimator::Change_Animation( STATE eState )
{
	if( m_pPreviousAnimationSet ) m_pPreviousAnimationSet->ResetTimePos();
	m_pPreviousAnimationSet = m_pCurrentAnimationSet;
	m_pCurrentAnimationSet = m_vecAnimationSet[ eState ];
	if( !m_mapEvent.empty() )
		m_mapEvent.erase( m_mapEvent.begin(), m_mapEvent.end() );
	m_mapEvent = m_pCurrentAnimationSet->GetEvent();
}

bool CAnimator::GetCurrentAnimationFinished()
{
	if( m_pCurrentAnimationSet->GetLoop() )
		return true;
	else
		return m_pCurrentAnimationSet->GetFinished();
}

float CAnimator::GetPerFinish()
{
	return m_pCurrentAnimationSet->GetPerFinish();
}

void CAnimator::Pause()
{
	m_bPause = true;
}

void CAnimator::Play()
{
	m_bPause = false;
}

HRESULT CAnimator::CreateConstantBuffer( ID3D11Device* pDevice )
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

DWORD CAnimator::Release( void )
{
	DWORD dwRefCnt = CBase::Release();

	if( 0 == dwRefCnt )
	{
		::Safe_Delete_Array( m_pArrJointName );
		::Safe_Release( m_pConstantBufferAnimation );
	}

	for_each( m_vecAnimationSet.begin(), m_vecAnimationSet.end(), ReleaseElement() );
	m_vecAnimationSet.erase( m_vecAnimationSet.begin(), m_vecAnimationSet.end() );
	m_vecAnimationSet.swap( vector<CAnimationSet*>{} );

	delete this;
	return 0;
}

void CAnimator::Render( ID3D11DeviceContext* pContext )
{
	D3D11_MAPPED_SUBRESOURCE MappedSubresource;
	pContext->Map( m_pConstantBufferAnimation, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );
	XMFLOAT4X4* pWorld = ( XMFLOAT4X4* )MappedSubresource.pData;
	m_pCurrentAnimationSet->GetAnimationMatrix( pWorld );
	pContext->Unmap( m_pConstantBufferAnimation, 0 );
	pContext->VSSetConstantBuffers( SLOT_ANIMATION, 1, &m_pConstantBufferAnimation );
}

void CAnimator::Update( CGameObject* pOwner, const float& fTimeDelta )
{
	if( m_pCurrentAnimationSet )
	{
		int iEvent{ -1 };
		auto map_iter = m_mapEvent.find( ( int )m_pCurrentAnimationSet->GetTimePos() );
		if( map_iter != m_mapEvent.end() ) iEvent = map_iter->second;

		switch( iEvent )
		{
		case 0:
			m_bPause = true;
			m_mapEvent.erase( map_iter );
			break;
		default:
			break;
		}

		if( !m_bPause ) m_pCurrentAnimationSet->Update( fTimeDelta );
		if( pOwner ) ConnectActorShape( pOwner );
	}
}
