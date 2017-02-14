#include "stdafx.h"
#include "Shader.h"
#include "D3DCompiler.h"
#include "Function.h"

CShader::CShader()
	: CResources()
	, m_pInputLayout( nullptr )
	, m_pVS( nullptr )
	, m_pPS( nullptr )
	, m_pGS( nullptr )
	, m_pHS( nullptr )
	, m_pDS( nullptr )
{
}


CShader::CShader( const CShader& Instance )
	: CResources( Instance )
	, m_pInputLayout( Instance.m_pInputLayout )
	, m_pVS( Instance.m_pVS )
	, m_pPS( Instance.m_pPS )
	, m_pGS( Instance.m_pGS )
	, m_pHS( Instance.m_pHS )
	, m_pDS( Instance.m_pDS )
{
}

CShader::~CShader()
{
}

HRESULT CShader::CreateVS( ID3D11Device* pDevice, const char* pFilePath, D3D11_INPUT_ELEMENT_DESC* pInputElement, UINT iComponentCnt )
{
	DWORD		dwShaderFlag = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlag |= D3D10_SHADER_DEBUG;
	dwShaderFlag |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3DBlob *pShaderBlob = NULL, *pErrorBlob = NULL;

	if( SUCCEEDED( D3DX11CompileFromFile( pFilePath, NULL, NULL, "VS", "vs_5_0", dwShaderFlag, 0, NULL, &pShaderBlob, &pErrorBlob, NULL ) ) )
	{
		pDevice->CreateVertexShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pVS );
		pDevice->CreateInputLayout( pInputElement, iComponentCnt, pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), &m_pInputLayout );
		pShaderBlob->Release();
	}

	else
	{
		if( nullptr != pErrorBlob )
		{
//			MessageBoxA( 0, ( char* )pErrorBlob->GetBufferPointer(), 0, 0 );
			pErrorBlob->Release();
			pErrorBlob = nullptr;
		}

//		return E_FAIL;
	}

	return S_OK;
}

HRESULT CShader::CreatePS( ID3D11Device* pDevice, const char* pFilePath )
{
	DWORD		dwShaderFlag = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlag |= D3D10_SHADER_DEBUG;
	dwShaderFlag |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3DBlob *pShaderBlob = NULL, *pErrorBlob = NULL;
	if( SUCCEEDED( D3DX11CompileFromFile( pFilePath, NULL, NULL, "PS", "ps_5_0", dwShaderFlag, 0, NULL, &pShaderBlob, &pErrorBlob, NULL ) ) )
	{
		pDevice->CreatePixelShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pPS );
		pShaderBlob->Release();
	}

	else
	{
		if( NULL != pErrorBlob )
		{
//			MessageBoxA( 0, ( char* )pErrorBlob->GetBufferPointer(), 0, 0 );
			pErrorBlob->Release();
			pErrorBlob = nullptr;
		}

//		return E_FAIL;
	}

	return S_OK;
}

HRESULT CShader::CreateGS( ID3D11Device* pDevice, const char* pFilePath )
{
	DWORD		dwShaderFlag = D3DCOMPILE_ENABLE_STRICTNESS;

	ID3DBlob *pShaderBlob = NULL, *pErrorBlob = NULL;
	if( SUCCEEDED( D3DX11CompileFromFile( pFilePath, NULL, NULL, "GS", "gs_5_0", dwShaderFlag, 0, NULL, &pShaderBlob, &pErrorBlob, NULL ) ) )
	{
		pDevice->CreateGeometryShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pGS );
		pShaderBlob->Release();
	}

	else
	{
		if( NULL != pErrorBlob )
		{
//			MessageBoxA( 0, ( char* )pErrorBlob->GetBufferPointer(), 0, 0 );
			pErrorBlob->Release();
			pErrorBlob = nullptr;
		}

//		return E_FAIL;
	}

	return S_OK;
}

HRESULT CShader::CreateHS( ID3D11Device* pDevice, const char* pFilePath )
{
	DWORD		dwShaderFlag = D3DCOMPILE_ENABLE_STRICTNESS;

	ID3DBlob *pShaderBlob = NULL, *pErrorBlob = NULL;
	if( SUCCEEDED( D3DX11CompileFromFile( pFilePath, NULL, NULL, "HS", "hs_5_0", dwShaderFlag, 0, NULL, &pShaderBlob, &pErrorBlob, NULL ) ) )
	{
		pDevice->CreateHullShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pHS );
		pShaderBlob->Release();
	}

	else
	{
		if( NULL != pErrorBlob )
		{
//			MessageBoxA( 0, ( char* )pErrorBlob->GetBufferPointer(), 0, 0 );
			pErrorBlob->Release();
			pErrorBlob = nullptr;
		}

//		return E_FAIL;
	}

	

	return S_OK;
}

HRESULT CShader::CreateDS( ID3D11Device* pDevice, const char* pFilePath )
{
	DWORD		dwShaderFlag = D3DCOMPILE_ENABLE_STRICTNESS;
	ID3DBlob *pShaderBlob = NULL, *pErrorBlob = NULL;

	if( SUCCEEDED( D3DX11CompileFromFile( pFilePath, NULL, NULL, "DS", "ds_5_0", dwShaderFlag, 0, NULL, &pShaderBlob, &pErrorBlob, NULL ) ) )
	{
		pDevice->CreateDomainShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pDS );
		pShaderBlob->Release();
	}

	else
	{
		if( NULL != pErrorBlob )
		{
//			MessageBoxA( 0, ( char* )pErrorBlob->GetBufferPointer(), 0, 0 );
			pErrorBlob->Release();
			pErrorBlob = nullptr;
		}

//		return E_FAIL;
	}

	return S_OK;
}

DWORD CShader::Release()
{
	DWORD dwRefCnt = CResources::Release();

	if( 0 == dwRefCnt )
	{
		::Safe_Release( m_pInputLayout );
		::Safe_Release( m_pVS );
		::Safe_Release( m_pPS );
		::Safe_Release( m_pGS );
		::Safe_Release( m_pHS );
		::Safe_Release( m_pDS );
	}

	return dwRefCnt;
}
