#include "stdafx.h"
#include "Animator.h"
#include "AnimationSet.h"
#include <fstream>
#include "../Player.h"
#include <NxActor.h>
#include <NxShape.h>
#include <NxMat34.h>
#include <NxController.h>

CAnimator::CAnimator( void )
	: CBase()
	, m_bPause( false )
	, m_dwJointCnt( 0 )
	, m_pArrJointName( nullptr )
	, m_pCurrentAnimationSet( nullptr )
	, m_pPreviousAnimationSet( nullptr )
{
}

CAnimator::CAnimator( const CAnimator& Instance )
	: CBase( Instance )
	, m_bPause( false )
	, m_dwJointCnt( Instance.m_dwJointCnt )
	, m_pArrJointName( Instance.m_pArrJointName )
	, m_pPreviousAnimationSet( nullptr )
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

CAnimator* CAnimator::Create( const char* pFilePath )
{
	CAnimator*	pAnimationController = new CAnimator;

	if( FAILED( pAnimationController->Initialize( pFilePath ) ) )
	{
		pAnimationController->Release();
		pAnimationController = nullptr;
	}
	return pAnimationController;
}

HRESULT CAnimator::Initialize( const char* pFilePath )
{
	if( FAILED( Load( pFilePath ) ) )
		return E_FAIL;

	return S_OK;
}

HRESULT CAnimator::Load( const char* pFilePath )
{
	ifstream pIn( pFilePath );

	if (pIn.is_open() == false)
	{
		SLog(L"! '%s' open fail! ", pFilePath);
		return S_FALSE;
	}

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

void CAnimator::ConnectActorShape( Player* pOwner )
{
	DWORD i = 0;
	XMFLOAT4X4*	pWorld{ new XMFLOAT4X4[ m_dwJointCnt ] };
	m_pCurrentAnimationSet->GetAnimationMatrix( pWorld );
	NxController* pCharacterController = pOwner->getCCT();
	NxActor** dpActorArray = pOwner->getActors();
	NxMat34* pActorsOriginPose = pOwner->getActorsOriginPose();

	DWORD dwActorCnt = ( DWORD )pOwner->getActorCount();

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

class ReleaseElement
{
public:
	template <class T> void operator() ( T& Element )
	{
		if( nullptr != Element )
		{
			Element->Release();
			Element = nullptr;
		}
	}
};

DWORD CAnimator::Release( void )
{
	DWORD dwRefCnt = CBase::Release();
	
	if( 0 == dwRefCnt )
	{
		if( m_pArrJointName )
		{
			delete[] m_pArrJointName;
			m_pArrJointName = nullptr;
		}
	}

	for_each( m_vecAnimationSet.begin(), m_vecAnimationSet.end(), ReleaseElement() );
	m_vecAnimationSet.erase( m_vecAnimationSet.begin(), m_vecAnimationSet.end() );
	m_vecAnimationSet.swap( vector<CAnimationSet*>{} );
		
	delete this;
	return 0;
}

void CAnimator::Update( Player* pOwner, const float& fTimeDelta )
{
	if( m_pCurrentAnimationSet )
	{
		int iEvent{ -1 };
		if( m_mapEvent.size() == 1 )
			int iDebuig = 0;
		auto map_iter = m_mapEvent.find( ( int )m_pCurrentAnimationSet->GetTimePos() );
		if( map_iter != m_mapEvent.end() ) iEvent = map_iter->second;

		switch( iEvent )
		{
		case 0:
			m_bPause = true;
			m_mapEvent.erase( map_iter );
			break;
		case 1:
			pOwner->ThrowBanana();
			m_mapEvent.erase( map_iter );
			break;
		case 2:
			pOwner->sweapOn();
			m_mapEvent.erase( map_iter );
			break;
		case 3:
			m_bPause = true;
			pOwner->defendOn();
			m_mapEvent.erase( map_iter );
			break;
		case 4:
			pOwner->defendOff();
			m_mapEvent.erase( map_iter );
			break;
		default:
			break;
		}

		if( !m_bPause ) m_pCurrentAnimationSet->Update( fTimeDelta );
		ConnectActorShape( pOwner );
	}
}

NxMat34 CAnimator::GetCurrentAnimationMatrix(Player* pOwner, const char* pKey)
{
	DWORD i = 0;
	XMFLOAT4X4*	pWorld{ new XMFLOAT4X4[m_dwJointCnt] };
	m_pCurrentAnimationSet->GetAnimationMatrix(pWorld);
	NxController* pCharacterController = pOwner->getCCT();

	DWORD dwActorCnt = pOwner->getActorCount();

	for (; i < m_dwJointCnt; ++i)
		if (0 == strcmp(m_pArrJointName[i].c_str(), pKey))
			break;

	XMFLOAT4X4 mtxTemp = pOwner->GetWorld();
	XMMATRIX mtxLoadOrigin, mtxLoadAnimation{};
	XMFLOAT3 vTest{};
	vTest.x = pWorld[i]._13;
	vTest.y = pWorld[i]._23;
	vTest.z = pWorld[i]._33;
	pWorld[i]._13 = pWorld[i]._12;
	pWorld[i]._23 = pWorld[i]._22;
	pWorld[i]._33 = pWorld[i]._32;
	pWorld[i]._12 = vTest.x;
	pWorld[i]._22 = vTest.y;
	pWorld[i]._32 = vTest.z;

	mtxLoadAnimation = XMLoadFloat4x4(&pWorld[i]);
	XMMATRIX mtxResult = XMMatrixMultiply(XMLoadFloat4x4(&mtxTemp), mtxLoadAnimation);
	NxMat34 mtxOut = CMathematics::ConvertToNxMat34(mtxResult);

	delete[] pWorld;
	pWorld = nullptr;

	return mtxOut;
}