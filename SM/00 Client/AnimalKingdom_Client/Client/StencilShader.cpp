#include "stdafx.h"
#include "StencilShader.h"
#include "Function.h"
#include "GameObject.h"
#include "Functor.h"

CStencilShader::CStencilShader()
	: CShader()
{
}

CStencilShader::CStencilShader( const CStencilShader& Instance )
	: CShader( Instance )
{
}

CStencilShader::~CStencilShader()
{
}

CShader* CStencilShader::Clone( void )
{
	return new CStencilShader( *this );
}

void CStencilShader::Update( const float& fTimeDelta )
{
}

void CStencilShader::Render( ID3D11DeviceContext* pContext )
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
		SetConstantBuffer( pContext, *m_vecRenderObject[ i ]->GetWorld(), m_vecRenderObject[ i ]->GetOption() );
		m_vecRenderObject[ i ]->Render_Stencil( pContext );
	}
}

DWORD CStencilShader::Release( void )
{
	DWORD dwRefCnt = CShader::Release();

	for_each( m_vecRenderObject.begin(), m_vecRenderObject.end(), ReleaseElement() );
	m_vecRenderObject.erase( m_vecRenderObject.begin(), m_vecRenderObject.end() );
	m_vecRenderObject.swap( vector<CGameObject*>{} );

	delete this;

	return dwRefCnt;
}

CStencilShader* CStencilShader::Create( ID3D11Device* pDevice, CShader::INPUT_TYPE eInputType, const TCHAR* pFilePath )
{
	CStencilShader* pStencil = new CStencilShader;

	if( FAILED( pStencil->Initialize( pDevice, eInputType, pFilePath ) ) )
	{
		pStencil->Release();
		pStencil = nullptr;
	}

	return pStencil;
}

HRESULT CStencilShader::Initialize( ID3D11Device* pDevice, CShader::INPUT_TYPE eInputType, const TCHAR* pFilePath )
{
	D3D11_INPUT_ELEMENT_DESC* pInputDesc{ nullptr };
	UINT iArrCnt{};

	switch( eInputType )
	{
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

	CreateConstantBuffer( pDevice );

	return S_OK;
}

void CStencilShader::Add_RenderObject( CGameObject* pGameObject )
{
	pGameObject->Add_Ref();
	m_vecRenderObject.push_back( pGameObject );
}
