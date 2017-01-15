#include "stdafx.h"
#include "BasicShader.h"

CBasicShader::CBasicShader()
	: CShader()
{

}

HRESULT CBasicShader::Initialize( ID3D11Device* pDevice, const TCHAR* pFilePath )
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

	UINT iElementCnt = ARRAYSIZE( VTX_Element );

	if( FAILED( CreateVertexShader( pDevice, pFilePath, VTX_Element, iElementCnt ) ) )
		return E_FAIL;

	if( FAILED( CreatePixelShader( pDevice, pFilePath ) ) )
		return E_FAIL;

	return S_OK;
}

CBasicShader* CBasicShader::Create( ID3D11Device* pDevice, const TCHAR* pFilePath )
{
	CBasicShader*	pInstance{ nullptr };

	pInstance = new CBasicShader;
	if( FAILED( pInstance->Initialize( pDevice, pFilePath ) ) )
	{
		pInstance->Release();
		pInstance = nullptr;
	}

	return ( CBasicShader* )pInstance;
}

void CBasicShader::Release( void )
{
	CShader::Release();

	delete this;
}