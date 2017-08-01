#include "stdafx.h"
#include "InstancingShader.h"
#include "Function.h"
#include "GameObject.h"
#include "Texture.h"
#include "Functor.h"

CInstancingShader::CInstancingShader()
	: CShader()
	, m_pDevice( nullptr )
	, m_pInstancingBuffer( nullptr )
	, m_eInputElement( INPUT_NO )
{
}

CInstancingShader::CInstancingShader( const CInstancingShader& Instance )
	: CShader( Instance )
	, m_pDevice( Instance.m_pDevice )
	, m_pInstancingBuffer( Instance.m_pInstancingBuffer )
	, m_eInputElement( Instance.m_eInputElement )
{
}

CInstancingShader::~CInstancingShader()
{
}

CShader* CInstancingShader::Clone( void )
{
	return new CInstancingShader( *this );
}

void CInstancingShader::Update( const float& fTimeDelta )
{
	if( m_vecRenderObject.empty() )
		return;

	for( size_t i = 0; i < m_vecRenderObject.size(); ++i )
		m_vecRenderObject[ i ]->Update( fTimeDelta );
}

void CInstancingShader::Render( ID3D11DeviceContext* pContext )
{
	pContext->IASetInputLayout( m_pInputLayout );
	pContext->VSSetShader( m_pVS, NULL, 0 );
	pContext->HSSetShader( m_pHS, NULL, 0 );
	pContext->DSSetShader( m_pDS, NULL, 0 );
	pContext->GSSetShader( m_pGS, NULL, 0 );
	pContext->PSSetShader( m_pPS, NULL, 0 );

	if( m_vecRenderObject.empty() )
		return;

	SetConstantBuffer( pContext, *m_vecRenderObject.front()->GetWorld(), m_vecRenderObject.front()->GetOption() );
	m_vecRenderObject.back()->Render( pContext );
}

DWORD CInstancingShader::Release( void )
{
	DWORD dwRefCnt = CShader::Release();

	for_each( m_vecRenderObject.begin(), m_vecRenderObject.end(), ReleaseElement() );
	m_vecRenderObject.erase( m_vecRenderObject.begin(), m_vecRenderObject.end() );
	m_vecRenderObject.swap( vector<CGameObject*>{} );

	if( 0 == dwRefCnt )	::Safe_Release( m_pInstancingBuffer );
	else ::Safe_Release( m_pDevice );

	delete this;

	return dwRefCnt;
}

CInstancingShader* CInstancingShader::Create( ID3D11Device* pDevice, CShader::INPUT_TYPE eInputType, const TCHAR* pFilePath )
{
	CInstancingShader* pInstancingShader = new CInstancingShader;

	if( FAILED( pInstancingShader->Initialize( pDevice, eInputType, pFilePath ) ) )
	{
		pInstancingShader->Release();
		pInstancingShader = nullptr;
	}

	return pInstancingShader;
}

HRESULT CInstancingShader::Initialize( ID3D11Device* pDevice, CShader::INPUT_TYPE eInputType, const TCHAR* pFilePath )
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
		};
		pInputDesc = VTX_Element;
		iArrCnt = ARRAYSIZE( VTX_Element );
	}
	break;
	case CShader::INPUT_PNTT:
	{
		D3D11_INPUT_ELEMENT_DESC	VTX_Element[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "OPTION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
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

HRESULT CInstancingShader::CreateConstantBuffer( ID3D11Device* pDevice, UINT iBufferSize /*= sizeof( CB_WORLD ) */ )
{
	size_t  dwInstancedCnt = m_vecRenderObject.size();
	::Safe_Release( m_pInstancingBuffer );

	ID3D11Buffer*	pInstanceBuffer;
	/* VS ¿ë World */
	D3D11_BUFFER_DESC Buffer_Desc;
	ZeroMemory( &Buffer_Desc, sizeof( Buffer_Desc ) );
	Buffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
	switch( m_eInputElement )
	{
	case INPUT_NO:
	case INPUT_PNTT:
		Buffer_Desc.ByteWidth = UINT( sizeof( CB_WORLD ) * dwInstancedCnt );
		break;
	}

	Buffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	Buffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	pDevice->CreateBuffer( &Buffer_Desc, NULL, &pInstanceBuffer );

	m_pInstancingBuffer = pInstanceBuffer;

	return S_OK;
}

void CInstancingShader::Add_RenderObject( CGameObject* pGameObject )
{
	pGameObject->Add_Ref();
	m_vecRenderObject.push_back( pGameObject );
	CreateConstantBuffer( m_pDevice, sizeof( CB_WORLD ) );
	pGameObject->SetInstanceCnt( ( UINT )m_vecRenderObject.size() );
}

void CInstancingShader::SetConstantBuffer( ID3D11DeviceContext* pContext, XMFLOAT4X4 mtxWorld, XMFLOAT4 vOption /*= XMFLOAT4( 0.f, 0.f, 0.f, 0.f ) */ )
{
	UINT	iStride{ 0 }, iOffset{ 0 };
	D3D11_MAPPED_SUBRESOURCE MappedSubresource;
	pContext->Map( m_pInstancingBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );

	switch( m_eInputElement )
	{
	case CShader::INPUT_NO:
	case CShader::INPUT_PNTT:
	{
		CB_WORLD* pStruct = ( CB_WORLD* )MappedSubresource.pData;
		for( size_t i = 0; i < m_vecRenderObject.size(); ++i )
		{
			pStruct[ i ].m_mtxWorld = *m_vecRenderObject[ i ]->GetWorld();
			pStruct[ i ].m_vOption = m_vecRenderObject[ i ]->GetOption();
		}
		iStride = sizeof( CB_WORLD );
	}
	break;
	};

	pContext->Unmap( m_pInstancingBuffer, 0 );
	pContext->IASetVertexBuffers( 1, 1, &m_pInstancingBuffer, &iStride, &iOffset );
}
