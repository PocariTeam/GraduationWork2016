#include "stdafx.h"
#include "ParticleShader.h"
#include "Function.h"
#include "GameObject.h"
#include "Functor.h"
#include "GameObject.h"

CParticleShader::CParticleShader()
	: CShader()
	, m_pDevice( nullptr )
	, m_pInstancingBuffer( nullptr )
	, m_eInputElement( INPUT_NO )
{
}

CParticleShader::CParticleShader( const CParticleShader& Instance )
	: CShader( Instance )
	, m_pDevice( Instance.m_pDevice )
	, m_pInstancingBuffer( Instance.m_pInstancingBuffer )
	, m_eInputElement( Instance.m_eInputElement )
{
}

CParticleShader::~CParticleShader()
{
}

CShader* CParticleShader::Clone( void )
{
	return new CParticleShader( *this );
}

void CParticleShader::Update( const float& fTimeDelta )
{
	if( m_listRenderObject.empty() )
		return;

	auto Advance_iter = m_listRenderObject.begin();

	for( Advance_iter; Advance_iter != m_listRenderObject.end(); )
	{
		if( 1 == ( *Advance_iter )->Update( fTimeDelta ) )
		{
			// 셰이더 삭제 시점 ( 인스턴스 수, 버퍼 수정 )
			::Safe_Release( *Advance_iter );
			Advance_iter = m_listRenderObject.erase( Advance_iter );
			CreateConstantBuffer( m_pDevice );
		}

		else
			Advance_iter++;
	}
}

void CParticleShader::Render( ID3D11DeviceContext* pContext )
{
	pContext->IASetInputLayout( m_pInputLayout );
	pContext->VSSetShader( m_pVS, NULL, 0 );
	pContext->HSSetShader( m_pHS, NULL, 0 );
	pContext->DSSetShader( m_pDS, NULL, 0 );
	pContext->GSSetShader( m_pGS, NULL, 0 );
	pContext->PSSetShader( m_pPS, NULL, 0 );

	if( m_listRenderObject.empty() )
		return;

	SetConstantBuffer( pContext, *m_listRenderObject.front()->GetWorld(), m_listRenderObject.front()->GetOption() );
	m_listRenderObject.back()->Render( pContext );
}

DWORD CParticleShader::Release( void )
{
	DWORD dwRefCnt = CShader::Release();

	for_each( m_listRenderObject.begin(), m_listRenderObject.end(), ReleaseElement() );
	m_listRenderObject.erase( m_listRenderObject.begin(), m_listRenderObject.end() );
	
	if( 0 == dwRefCnt )	::Safe_Release( m_pInstancingBuffer );
	else ::Safe_Release( m_pDevice );
	delete this;

	return dwRefCnt;
}

CParticleShader* CParticleShader::Create( ID3D11Device* pDevice, CShader::INPUT_TYPE eInputType, const TCHAR* pFilePath )
{
	CParticleShader* pParticleShader = new CParticleShader;

	if( FAILED( pParticleShader->Initialize( pDevice, eInputType, pFilePath ) ) )
	{
		pParticleShader->Release();
		pParticleShader = nullptr;
	}

	return pParticleShader;
}

HRESULT CParticleShader::Initialize( ID3D11Device* pDevice, CShader::INPUT_TYPE eInputType, const TCHAR* pFilePath )
{
	m_pDevice = pDevice;
	D3D11_INPUT_ELEMENT_DESC* pInputDesc{ nullptr };
	UINT iArrCnt{};
	m_eInputElement = eInputType;

	switch( eInputType )
	{
	case CShader::INPUT_NO:
	{
		D3D11_INPUT_ELEMENT_DESC	VTX_Element[] = { { "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "OPTION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "VELOCITY", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		};
		pInputDesc = VTX_Element;
		iArrCnt = ARRAYSIZE( VTX_Element );
	}
	break;
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

	CreateConstantBuffer( pDevice );

	return S_OK;
}

HRESULT CParticleShader::CreateConstantBuffer( ID3D11Device* pDevice, UINT iBufferSize /*= sizeof( CB_WORLD ) */ )
{
	size_t  dwInstancedCnt = m_listRenderObject.size();
	::Safe_Release( m_pInstancingBuffer );

	if( 0 == dwInstancedCnt ) return S_OK;

	ID3D11Buffer*	pInstanceBuffer;
	/* VS 용 World */
	D3D11_BUFFER_DESC Buffer_Desc;
	ZeroMemory( &Buffer_Desc, sizeof( Buffer_Desc ) );
	Buffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
	switch( m_eInputElement )
	{
	case INPUT_NO:
	case INPUT_PNTT:
		Buffer_Desc.ByteWidth = UINT( sizeof( CB_PARTICLE ) * dwInstancedCnt );
		break;
	}

	Buffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	Buffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	pDevice->CreateBuffer( &Buffer_Desc, NULL, &pInstanceBuffer );

	m_pInstancingBuffer = pInstanceBuffer;

	return S_OK;
}

void CParticleShader::Add_RenderObject( CGameObject* pGameObject )
{
	pGameObject->Add_Ref();
	m_listRenderObject.push_back( pGameObject );
	CreateConstantBuffer( m_pDevice, sizeof( CB_PARTICLE ) );
	pGameObject->SetInstanceCnt( ( UINT )m_listRenderObject.size() );
}

void CParticleShader::SetConstantBuffer( ID3D11DeviceContext* pContext, XMFLOAT4X4 mtxWorld, XMFLOAT4 vOption /*= XMFLOAT4( 0.f, 0.f, 0.f, 0.f ) */ )
{
	UINT	iStride{ 0 }, iOffset{ 0 };
	D3D11_MAPPED_SUBRESOURCE MappedSubresource;
	pContext->Map( m_pInstancingBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );

	switch( m_eInputElement )
	{
	case CShader::INPUT_NO:
	case CShader::INPUT_PNTT:
	{
		CB_PARTICLE* pStruct = ( CB_PARTICLE* )MappedSubresource.pData;

		if( m_listRenderObject.empty() )
			return;

		auto Advance_iter = m_listRenderObject.begin();
		int i = 0;
		for( Advance_iter ; Advance_iter != m_listRenderObject.end(); Advance_iter++, i++ )
		{
			pStruct[ i ].m_mtxWorld = *( *Advance_iter )->GetWorld();
			pStruct[ i ].m_vOption = ( *Advance_iter )->GetOption();
			pStruct[ i ].m_vVelocity = ( *Advance_iter )->GetVelocity();
		}
		iStride = sizeof( CB_PARTICLE );
	}
	break;
	};

	pContext->Unmap( m_pInstancingBuffer, 0 );
	pContext->IASetVertexBuffers( 1, 1, &m_pInstancingBuffer, &iStride, &iOffset );
}
