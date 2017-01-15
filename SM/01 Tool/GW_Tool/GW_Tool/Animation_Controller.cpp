#include "stdafx.h"
#include "Animation_Controller.h"
#include "AnimationSet.h"
#include "MainFrm.h"

CAnimation_Controller::CAnimation_Controller()
	: m_dwAnimationTickPerSecond( 0 )
	, m_iAnimationSetCnt( 0 )
	, m_pActiveAnimationSet( nullptr )
	, m_pCBAnimate( nullptr )
{
}

CAnimation_Controller::~CAnimation_Controller()
{
}

CAnimation_Controller* CAnimation_Controller::Create(ID3D11Device* pDeivce, const char* pAnimationName, vector<JOINT> vecJoint, DWORD dwFrameCnt, DWORD dwTickPerSecond, DWORD dwAnimationLength )
{
	CAnimation_Controller* pController = new CAnimation_Controller;

	if (FAILED(pController->Initialize(pDeivce, pAnimationName, vecJoint, dwFrameCnt, dwTickPerSecond, dwAnimationLength )))
	{
		pController->Release();
		pController = nullptr;
	}

	return pController;
}

HRESULT CAnimation_Controller::Initialize(ID3D11Device* pDevice, const char* pAnimationName, vector<JOINT> vecJoint, DWORD dwFrameCnt, DWORD dwTickPerSecond, DWORD dwAnimationLength )
{
	m_dwAnimationTickPerSecond = dwTickPerSecond;
	CreateAnimationSet( vecJoint, pAnimationName, 0, dwAnimationLength );
	CreateConstantBuffer(pDevice, dwFrameCnt);
	m_pActiveAnimationSet = m_vecAnimationSet.front();

	return S_OK;
}

void CAnimation_Controller::CreateAnimationSet( vector<JOINT> vecJoint, const char* pName, DWORD dwStart, DWORD dwLast )
{
	m_iAnimationSetCnt++;
	m_vecAnimationSet.emplace_back( CAnimationSet::Create( vecJoint, pName, dwStart, dwLast ) );
}

void CAnimation_Controller::Animate( float fTimeDelta )
{
	if( m_pActiveAnimationSet ) m_pActiveAnimationSet->Animate( fTimeDelta );
}

void CAnimation_Controller::Render(ID3D11DeviceContext* pContext)
{
	SetConstantBuffer(pContext);
}

void CAnimation_Controller::Release()
{
	if( nullptr != m_pCBAnimate ) {
		m_pCBAnimate->Release();
		m_pCBAnimate = nullptr;
	}

	for( size_t i = 0; i < m_vecAnimationSet.size(); ++i )
		m_vecAnimationSet[ i ]->Release();
	m_vecAnimationSet.erase( m_vecAnimationSet.begin(), m_vecAnimationSet.end() );
	m_vecAnimationSet.swap( vector<CAnimationSet*>{} );
}

void CAnimation_Controller::CreateConstantBuffer(ID3D11Device* pDevice, DWORD dwFrameCnt)
{
	D3D11_BUFFER_DESC Buffer_Desc;
	ZeroMemory(&Buffer_Desc, sizeof(Buffer_Desc));
	Buffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
	Buffer_Desc.ByteWidth = sizeof(XMFLOAT4X4) * ( UINT )dwFrameCnt;
	Buffer_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Buffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pDevice->CreateBuffer(&Buffer_Desc, NULL, &m_pCBAnimate);
}

void CAnimation_Controller::SetConstantBuffer(ID3D11DeviceContext* pContext)
{
	D3D11_MAPPED_SUBRESOURCE MappedSubresource;

	pContext->Map( m_pCBAnimate, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );
	CB_TEST* pCBWorld = ( CB_TEST* )MappedSubresource.pData;

	Keyframe** pTemp = new Keyframe*[ m_pActiveAnimationSet->m_vecJoint.size() ];
	XMFLOAT4X4* mtxArray = new XMFLOAT4X4[ m_pActiveAnimationSet->m_vecJoint.size() ];
	XMMATRIX	mtxCompute;

	for( DWORD i = 0; i < ( DWORD )m_pActiveAnimationSet->m_vecJoint.size(); ++i )
	{
		if( nullptr == m_pActiveAnimationSet->m_vecJoint[ i ].pAnimation ) continue;
		pTemp[ i ] = m_pActiveAnimationSet->m_vecJoint[ i ].pAnimation;
		for( int j = 0; j < ( int )m_pActiveAnimationSet->m_fTimePos; ++j )
			pTemp[ i ] = pTemp[ i ]->m_pNext;

		mtxArray[ i ] = pTemp[ i ]->m_mtxGlobalTransform;
	}

	memcpy( pCBWorld->m_mtxJoint, mtxArray, sizeof( XMFLOAT4X4 ) * m_pActiveAnimationSet->m_vecJoint.size() );

	pContext->Unmap( m_pCBAnimate, 0 );
	pContext->VSSetConstantBuffers( SLOT_ANIM, 1, &m_pCBAnimate );

	delete[] pTemp;
	delete[] mtxArray;
}
