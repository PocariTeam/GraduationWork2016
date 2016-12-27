#include "stdafx.h"
#include "D3DCompiler.h"
#include "Base_Shader.h"
#include "Function.h"
#include "GameObject.h"

CBase_Shader::CBase_Shader()
	: CShader()
{
}

CBase_Shader::CBase_Shader( const CBase_Shader& Instance )
	: CShader( Instance )
{
}

CBase_Shader::~CBase_Shader( void )
{

}

CBase_Shader* CBase_Shader::Clone( void )
{
	return new CBase_Shader( *this );
}

HRESULT CBase_Shader::Initialize( ID3D11Device* pDevice, CShader::INPUT_ELEMENT eInput_Element, const TCHAR* pFilePath, bool bUse_GS, bool bUse_TS )
{
	D3D11_INPUT_ELEMENT_DESC* VTX_ElementDesc{ nullptr };
	UINT iElementCnt{};

	switch( eInput_Element )
	{
	case CShader::INPUT_BASE:
	{
		D3D11_INPUT_ELEMENT_DESC	VTX_Element[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0
			, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT
			, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT
			, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		VTX_ElementDesc = VTX_Element;
		iElementCnt = ARRAYSIZE( VTX_Element );
	}
	break;
	case CShader::INPUT_TANGENT:
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
		VTX_ElementDesc = VTX_Element;
		iElementCnt = ARRAYSIZE( VTX_Element );
	}
	break;
	case CShader::INPUT_POSITION_ONLY:
	{
		D3D11_INPUT_ELEMENT_DESC	VTX_Element[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0
			, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		VTX_ElementDesc = VTX_Element;
		iElementCnt = ARRAYSIZE( VTX_Element );
	}
	break;
	}

	if( FAILED( CreateVertexShader( pDevice, pFilePath, VTX_ElementDesc, iElementCnt ) ) )
		return E_FAIL;

	if( FAILED( CreatePixelShader( pDevice, pFilePath ) ) )
		return E_FAIL;

	if( bUse_GS )
		if( FAILED( CreateGeometryShader( pDevice, pFilePath ) ) )
			return E_FAIL;

	if( bUse_TS )
		if( FAILED( CreateHullDomainShader( pDevice, pFilePath ) ) )
			return E_FAIL;

	return S_OK;
}

CShader* CBase_Shader::Create( ID3D11Device* pDevice, CShader::INPUT_ELEMENT eInputElement, const TCHAR* pFilePath, bool bUse_GS, bool bUse_TS )
{
	CBase_Shader*	pShader = new CBase_Shader;

	if( FAILED( pShader->Initialize( pDevice, eInputElement, pFilePath, bUse_GS, bUse_TS ) ) )
	{
		pShader->Release();
		pShader = nullptr;
	}

	return ( CShader* )pShader;
}

DWORD CBase_Shader::Release( void )
{
	DWORD dwRefCnt = CShader::Release();
	delete this;

	return dwRefCnt;
}

void CBase_Shader::Render( ID3D11DeviceContext* pDeviceContext )
{
	if( m_vecRenderObject.empty() )
		return;

	pDeviceContext->IASetInputLayout( m_pInputLayout );
	pDeviceContext->VSSetShader( m_pVertexShader, NULL, 0 );
	pDeviceContext->HSSetShader( m_pHullShader, NULL, 0 );
	pDeviceContext->DSSetShader( m_pDomainShader, NULL, 0 );
	pDeviceContext->GSSetShader( m_pGeometryShader, NULL, 0 );
	pDeviceContext->PSSetShader( m_pPixelShader, NULL, 0 );

	for( size_t i = 0; i < m_vecRenderObject.size(); ++i )
		m_vecRenderObject[i]->Render( pDeviceContext );
}