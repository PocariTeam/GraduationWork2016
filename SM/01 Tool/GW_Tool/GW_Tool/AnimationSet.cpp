#include "stdafx.h"
#include "AnimationSet.h"
#include "MainFrm.h"

CAnimationSet::CAnimationSet()
{
}

CAnimationSet* CAnimationSet::Create( vector<JOINT> vecJoint, const char* strName, int iStart, int iLast )
{
	CAnimationSet*	pAnimationSet = new CAnimationSet;

	if( FAILED( pAnimationSet->Initialize( vecJoint, strName, iStart, iLast ) ) )
	{
		pAnimationSet->Release();
		pAnimationSet = nullptr;
	}

	return pAnimationSet;
}

HRESULT CAnimationSet::Initialize( vector<JOINT> vecJoint, const char* strName, int iStart, int iLast )
{
	strcpy_s( m_strName, strName );

	m_vecJoint = move( vecJoint );
	m_iLength = iLast;
	
	return S_OK;
}

void CAnimationSet::Animate( float fTimedelta )
{
	m_fTimePos += fTimedelta * m_fSpeed;

	if( m_fTimePos > m_iLength - 1 ) m_fTimePos = 0.f;

	( ( CMainFrame* )AfxGetMainWnd() )->m_pToolForm->m_mfcSlider.SetPos( ( int )m_fTimePos );
}

void CAnimationSet::Release( void )
{
	DWORD dwJointCnt = ( DWORD )m_vecJoint.size();
	for( DWORD i = 0; i < dwJointCnt; ++i )
	{
		Keyframe* pAdvance = m_vecJoint[ i ].pAnimation;
		Keyframe* pKeyframe = m_vecJoint[ i ].pAnimation;

		while( nullptr != pAdvance )
		{
			pKeyframe = pAdvance;
			pAdvance = pKeyframe->m_pNext;

			delete pKeyframe;
			pKeyframe = nullptr;
		}
	}

	delete this;
}
