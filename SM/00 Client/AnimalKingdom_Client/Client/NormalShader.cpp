#include "stdafx.h"
#include "NormalShader.h"
#include "Function.h"
#include "GameObject.h"
#include "Functor.h"

CNormalShader::CNormalShader()
	: CShader()
{
}

CNormalShader::CNormalShader( const CNormalShader& Instance )
	: CShader( Instance )
{
}

CNormalShader::~CNormalShader()
{
}

CShader* CNormalShader::Clone( void )
{
	return new CNormalShader( *this );
}

void CNormalShader::Update( const float& fTimeDelta )
{
	if( m_vecRenderObject.empty() )
		return;

	for( size_t i = 0; i < m_vecRenderObject.size(); ++i )
		m_vecRenderObject[ i ]->Update( fTimeDelta );
}

void CNormalShader::Render( ID3D11DeviceContext* pContext )
{
	pContext->IASetInputLayout( m_pInputLayout );
	pContext->VSSetShader( m_pVS, NULL, 0 );
	pContext->HSSetShader( m_pHS, NULL, 0 );
	pContext->DSSetShader( m_pDS, NULL, 0 );
	pContext->GSSetShader( m_pGS, NULL, 0 );
	pContext->PSSetShader( m_pPS, NULL, 0 );

	if( m_vecRenderObject.empty() )
		return;

	for( size_t i = 0; i < m_vecRenderObject.size(); ++i )
	{
		SetConstantBuffer( pContext, m_vecRenderObject[ i ]->GetWorld(), m_vecRenderObject[ i ]->GetOption() );
		m_vecRenderObject[ i ]->Render( pContext );
	}
}

DWORD CNormalShader::Release( void )
{
	DWORD dwRefCnt = CShader::Release();

	for_each( m_vecRenderObject.begin(), m_vecRenderObject.end(), ReleaseElement() );
	m_vecRenderObject.erase( m_vecRenderObject.begin(), m_vecRenderObject.end() );
	m_vecRenderObject.swap( vector<CGameObject*>{} );

	delete this;

	return dwRefCnt;
}

CNormalShader* CNormalShader::Create( ID3D11Device* pDevice, CShader::INPUT_TYPE eInputType, const TCHAR* pFilePath )
{
	CNormalShader* pNormalShader = new CNormalShader;

	if( FAILED( pNormalShader->Initialize( pDevice, eInputType, pFilePath ) ) )
	{
		pNormalShader->Release();
		pNormalShader = nullptr;
	}

	return pNormalShader;
}

HRESULT CNormalShader::Initialize( ID3D11Device* pDevice, CShader::INPUT_TYPE eInputType, const TCHAR* pFilePath )
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
	case CShader::INPUT_PNT:
	{
		D3D11_INPUT_ELEMENT_DESC	VTX_Element[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0
			, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT
			, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT
			, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		pInputDesc = VTX_Element;
		iArrCnt = ARRAYSIZE( VTX_Element );
	}
	break;
	case CShader::INPUT_PNTT:
	{
		D3D11_INPUT_ELEMENT_DESC	VTX_Element[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0
			, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT
			, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT
			, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT
			, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		pInputDesc = VTX_Element;
		iArrCnt = ARRAYSIZE( VTX_Element );
	}
	break;
	case CShader::INPUT_ANIMATE:
	{
		D3D11_INPUT_ELEMENT_DESC	VTX_Element[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0
			, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT
			, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT
			, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT
			, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, D3D10_APPEND_ALIGNED_ELEMENT
			, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT
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

void CNormalShader::Add_RenderObject( CGameObject* pGameObject )
{
	m_vecRenderObject.push_back( pGameObject );
}
