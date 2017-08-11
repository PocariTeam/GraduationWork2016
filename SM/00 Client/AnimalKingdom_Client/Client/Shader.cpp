#include "stdafx.h"
#include "Shader.h"
#include "D3DCompiler.h"
#include "Function.h"
#include "Define.h"

CShader::CShader()
	: CResources()
	, m_pInputLayout( nullptr )
	, m_pVS( nullptr )
	, m_pPS( nullptr )
	, m_pGS( nullptr )
	, m_pHS( nullptr )
	, m_pDS( nullptr )
	, m_pConstantBuffer( nullptr )
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
	, m_pConstantBuffer( Instance.m_pConstantBuffer )
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
		if( iComponentCnt > 0 )
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
			// MessageBoxA( 0, ( char* )pErrorBlob->GetBufferPointer(), 0, 0 );
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

HRESULT CShader::CreateConstantBuffer( ID3D11Device* pDevice, UINT iBufferSize )
{
	::Safe_Release( m_pConstantBuffer );

	D3D11_BUFFER_DESC Buffer_Desc;
	ZeroMemory( &Buffer_Desc, sizeof( Buffer_Desc ) );
	Buffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
	Buffer_Desc.ByteWidth = iBufferSize;
	Buffer_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Buffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	pDevice->CreateBuffer( &Buffer_Desc, nullptr, &m_pConstantBuffer );

	return S_OK;
}

void CShader::SetConstantBuffer( ID3D11DeviceContext* pContext, XMFLOAT4X4 mtxWorld, XMFLOAT4 vOption/* = XMFLOAT4( 0.f, 0.f, 0.f, 0.f )*/ )
{
	D3D11_MAPPED_SUBRESOURCE MappedSubresource;

	pContext->Map( m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );
	CB_WORLD* pConstantBuffer = ( CB_WORLD* )MappedSubresource.pData;
	pConstantBuffer->m_mtxWorld = mtxWorld;
	pConstantBuffer->m_vOption = vOption;

	pContext->Unmap( m_pConstantBuffer, 0 );
	pContext->VSSetConstantBuffers( SLOT_WORLD, 1, &m_pConstantBuffer );
	pContext->PSSetConstantBuffers( SLOT_WORLD, 1, &m_pConstantBuffer );
}

DWORD CShader::Release()
{
	DWORD dwRefCnt = CResources::Release();

	if( 0 == dwRefCnt )
	{
		::Safe_Release( m_pConstantBuffer );
		::Safe_Release( m_pInputLayout );
		::Safe_Release( m_pVS );
		::Safe_Release( m_pPS );
		::Safe_Release( m_pGS );
		::Safe_Release( m_pHS );
		::Safe_Release( m_pDS );
	}

	return dwRefCnt;
}
