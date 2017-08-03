#include "stdafx.h"
#include "TrailShader.h"
#include "Function.h"
#include "GameObject.h"
#include "Functor.h"
#include "GameObject.h"
#include "ParticleObject.h"
#include "Define.h"

CTrailShader::CTrailShader()
	: CShader()
{
}

CTrailShader::CTrailShader( const CTrailShader& Instance )
	: CShader( Instance )
{
}

CTrailShader::~CTrailShader()
{
}

CShader* CTrailShader::Clone( void )
{
	return new CTrailShader( *this );
}

void CTrailShader::Update( const float& fTimeDelta )
{
	if( m_vecRenderObject.empty() )
		return;

	DWORD dwObjectCnt = ( DWORD )m_vecRenderObject.size();

	for( DWORD i = 0; i < dwObjectCnt; ++i )
		m_vecRenderObject[ i ]->Update( fTimeDelta );
}

void CTrailShader::Render( ID3D11DeviceContext* pContext )
{
	pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
	pContext->IASetInputLayout( m_pInputLayout );
	pContext->VSSetShader( m_pVS, NULL, 0 );
	pContext->HSSetShader( m_pHS, NULL, 0 );
	pContext->DSSetShader( m_pDS, NULL, 0 );
	pContext->GSSetShader( m_pGS, NULL, 0 );
	pContext->PSSetShader( m_pPS, NULL, 0 );

	if( m_vecRenderObject.empty() )
		return;

	DWORD dwObjectCnt = ( DWORD )m_vecRenderObject.size();

	for( DWORD i = 0; i < dwObjectCnt; ++i )
	{
		SetConstantBuffer( pContext, ( ( CParticleObject* )m_vecRenderObject[ i ] )->GetParticleData(), ( ( CParticleObject* )m_vecRenderObject[ i ] )->GetParticleDataCnt() * sizeof( CB_PARTICLE ) );
		m_vecRenderObject[ i ]->Render( pContext );
	}
}

DWORD CTrailShader::Release( void )
{
	DWORD dwRefCnt = CShader::Release();

	for_each( m_vecRenderObject.begin(), m_vecRenderObject.end(), ReleaseElement() );
	m_vecRenderObject.erase( m_vecRenderObject.begin(), m_vecRenderObject.end() );
	m_vecRenderObject.swap( vector<CGameObject*>{} );

	delete this;

	return dwRefCnt;
}

CTrailShader* CTrailShader::Create( ID3D11Device* pDevice, CShader::INPUT_TYPE eInputType, const TCHAR* pFilePath )
{
	CTrailShader* pTrailShader = new CTrailShader;

	if( FAILED( pTrailShader->Initialize( pDevice, eInputType, pFilePath ) ) )
	{
		pTrailShader->Release();
		pTrailShader = nullptr;
	}

	return pTrailShader;
}

HRESULT CTrailShader::Initialize( ID3D11Device* pDevice, CShader::INPUT_TYPE eInputType, const TCHAR* pFilePath )
{
	D3D11_INPUT_ELEMENT_DESC* pInputDesc{ nullptr };
	UINT iArrCnt{};

	switch( eInputType )
	{
	case CShader::INPUT_POS_ONLY:
	{
		D3D11_INPUT_ELEMENT_DESC	VTX_Element[] = { { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 } };
		pInputDesc = VTX_Element;
		iArrCnt = ARRAYSIZE( VTX_Element );
	}
	break;
	case CShader::INPUT_PT:
	{
		D3D11_INPUT_ELEMENT_DESC	VTX_Element[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0
			, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT
			, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		pInputDesc = VTX_Element;
		iArrCnt = ARRAYSIZE( VTX_Element );
	}
	break;
	}

	if( FAILED( CreateVS( pDevice, pFilePath, pInputDesc, iArrCnt ) ) )
		return E_FAIL;

	if( FAILED( CreatePS( pDevice, pFilePath ) ) )
		return E_FAIL;

	CreateGS( pDevice, pFilePath );
	CreateHS( pDevice, pFilePath );
	CreateDS( pDevice, pFilePath );

	CreateConstantBuffer( pDevice, sizeof( CB_PARTICLE ) * MAX_TRAIL_CNT );

	return S_OK;
}
void CTrailShader::Add_RenderObject( CGameObject* pGameObject )
{
	pGameObject->Add_Ref();
	m_vecRenderObject.push_back( pGameObject );
}

void CTrailShader::SetConstantBuffer( ID3D11DeviceContext* pContext, LPVOID pData, UINT iDataSize )
{
	UINT	iStride{ iDataSize }, iOffset{ 0 };
	D3D11_MAPPED_SUBRESOURCE MappedSubresource;
	pContext->Map( m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );

	CB_PARTICLE* pStruct = ( CB_PARTICLE* )MappedSubresource.pData;

	memcpy_s( pStruct, iStride, pData, iStride );

	pContext->Unmap( m_pConstantBuffer, 0 );
	pContext->VSSetConstantBuffers( SLOT_WORLD, 1, &m_pConstantBuffer );
	pContext->GSSetConstantBuffers( SLOT_WORLD, 1, &m_pConstantBuffer );
	pContext->PSSetConstantBuffers( SLOT_WORLD, 1, &m_pConstantBuffer );
}