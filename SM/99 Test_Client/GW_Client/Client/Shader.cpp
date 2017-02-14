#include "stdafx.h"
#include "D3DCompiler.h"
#include "Shader.h"
#include "Function.h"
#include "GameObject.h"

CShader::CShader()
	: CBase()
	, m_pVertexShader( nullptr )
	, m_pPixelShader( nullptr )
	, m_pInputLayout( nullptr )
	, m_pGeometryShader( nullptr )
	, m_pHullShader( nullptr )
	, m_pDomainShader( nullptr )
{
}

CShader::CShader( const CShader& Instance )
	: CBase( Instance )
	, m_pVertexShader( Instance.m_pVertexShader )
	, m_pPixelShader( Instance.m_pPixelShader )
	, m_pInputLayout( Instance.m_pInputLayout )
	, m_pGeometryShader( Instance.m_pGeometryShader )
	, m_vecRenderObject( Instance.m_vecRenderObject )
	, m_pHullShader( Instance.m_pHullShader )
	, m_pDomainShader( Instance.m_pDomainShader )
{
}

CShader::~CShader( void )
{

}

DWORD CShader::Release( void )
{
	DWORD dwRefCnt = CBase::Release();

	m_vecRenderObject.clear();
	m_vecRenderObject.swap( vector<CGameObject*>{} );

	if( 0 == dwRefCnt ) {
		::Safe_Release( m_pPixelShader );
		::Safe_Release( m_pVertexShader );
		::Safe_Release( m_pGeometryShader );
		::Safe_Release( m_pHullShader );
		::Safe_Release( m_pDomainShader );
		::Safe_Release( m_pInputLayout );
	}

	return dwRefCnt;
}

HRESULT CShader::CreateVertexShader( ID3D11Device* pDevice, const TCHAR* pFilePath, D3D11_INPUT_ELEMENT_DESC* pVTX_Element, UINT iElementCnt )
{
	DWORD		dwShaderFlag = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlag |= D3D10_SHADER_DEBUG;
	dwShaderFlag |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3DBlob *pShaderBlob = NULL, *pErrorBlob = NULL;

	/*����(pszFileName)���� ���̴� �Լ�(pszShaderName)�� �������Ͽ� �����ϵ� ���̴� �ڵ��� �޸� �ּ�(pd3dShaderBlob)�� ��ȯ�Ѵ�.*/
	if( SUCCEEDED( D3DX11CompileFromFile( pFilePath, NULL, NULL, "VS"
		, "vs_5_0", dwShaderFlag, 0, NULL, &pShaderBlob, &pErrorBlob, NULL ) ) )
	{
		//�����ϵ� ���̴� �ڵ��� �޸� �ּҿ��� ����-���̴��� �����Ѵ�. 
		pDevice->CreateVertexShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pVertexShader );
		//�����ϵ� ���̴� �ڵ��� �޸� �ּҿ� �Է� ���̾ƿ����� ���� ���̾ƿ��� �����Ѵ�. 
		pDevice->CreateInputLayout( pVTX_Element, iElementCnt, pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), &m_pInputLayout );
		pShaderBlob->Release();
	}

	else
	{
		if( NULL != pErrorBlob )
		{
			MessageBoxA( 0, ( char* )pErrorBlob->GetBufferPointer(), 0, 0 );
			pErrorBlob->Release();
			pErrorBlob = nullptr;
		}

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CShader::CreatePixelShader( ID3D11Device* pDevice, const TCHAR* pFilePath )
{
	DWORD		dwShaderFlag = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlag |= D3D10_SHADER_DEBUG;
	dwShaderFlag |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3DBlob *pShaderBlob = NULL, *pErrorBlob = NULL;
	/*����(pszFileName)���� ���̴� �Լ�(pszShaderName)�� �������Ͽ� �����ϵ� ���̴� �ڵ��� �޸� �ּ�(pd3dShaderBlob)�� ��ȯ�Ѵ�.*/
	if( SUCCEEDED( D3DX11CompileFromFile( pFilePath, NULL, NULL, "PS"
		, "ps_5_0", dwShaderFlag, 0, NULL, &pShaderBlob, &pErrorBlob, NULL ) ) )
	{
		//�����ϵ� ���̴� �ڵ��� �޸� �ּҿ��� �ȼ�-���̴��� �����Ѵ�. 
		pDevice->CreatePixelShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pPixelShader );
		pShaderBlob->Release();
	}

	else
	{
		if( NULL != pErrorBlob )
		{
			MessageBoxA( 0, ( char* )pErrorBlob->GetBufferPointer(), 0, 0 );
			pErrorBlob->Release();
			pErrorBlob = nullptr;
		}

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CShader::CreateGeometryShader( ID3D11Device* pDevice, const TCHAR* pFilePath )
{
	DWORD		dwShaderFlag = D3DCOMPILE_ENABLE_STRICTNESS;

	ID3DBlob *pShaderBlob = NULL, *pErrorBlob = NULL;
	/*����(pszFileName)���� ���̴� �Լ�(pszShaderName)�� �������Ͽ� �����ϵ� ���̴� �ڵ��� �޸� �ּ�(pd3dShaderBlob)�� ��ȯ�Ѵ�.*/
	if( SUCCEEDED( D3DX11CompileFromFile( pFilePath, NULL, NULL, "GS"
		, "gs_5_0", dwShaderFlag, 0, NULL, &pShaderBlob, &pErrorBlob, NULL ) ) )
	{
		//�����ϵ� ���̴� �ڵ��� �޸� �ּҿ��� �ȼ�-���̴��� �����Ѵ�. 
		pDevice->CreateGeometryShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pGeometryShader );
		pShaderBlob->Release();
	}

	else
	{
		if( NULL != pErrorBlob )
		{
			MessageBoxA( 0, ( char* )pErrorBlob->GetBufferPointer(), 0, 0 );
			pErrorBlob->Release();
			pErrorBlob = nullptr;
		}

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CShader::CreateHullDomainShader( ID3D11Device* pDevice, const TCHAR* pFilePath )
{
	DWORD		dwShaderFlag = D3DCOMPILE_ENABLE_STRICTNESS;

	ID3DBlob *pShaderBlob = NULL, *pErrorBlob = NULL;
	/*����(pszFileName)���� ���̴� �Լ�(pszShaderName)�� �������Ͽ� �����ϵ� ���̴� �ڵ��� �޸� �ּ�(pd3dShaderBlob)�� ��ȯ�Ѵ�.*/
	if( SUCCEEDED( D3DX11CompileFromFile( pFilePath, NULL, NULL, "HS"
		, "hs_5_0", dwShaderFlag, 0, NULL, &pShaderBlob, &pErrorBlob, NULL ) ) )
	{
		//�����ϵ� ���̴� �ڵ��� �޸� �ּҿ��� �ȼ�-���̴��� �����Ѵ�. 
		pDevice->CreateHullShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pHullShader );
		pShaderBlob->Release();
	}

	else
	{
		if( NULL != pErrorBlob )
		{
			MessageBoxA( 0, ( char* )pErrorBlob->GetBufferPointer(), 0, 0 );
			pErrorBlob->Release();
			pErrorBlob = nullptr;
		}

		return E_FAIL;
	}

	pErrorBlob = pShaderBlob = nullptr;

	if( SUCCEEDED( D3DX11CompileFromFile( pFilePath, NULL, NULL, "DS"
		, "ds_5_0", dwShaderFlag, 0, NULL, &pShaderBlob, &pErrorBlob, NULL ) ) )
	{
		//�����ϵ� ���̴� �ڵ��� �޸� �ּҿ��� �ȼ�-���̴��� �����Ѵ�. 
		pDevice->CreateDomainShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pDomainShader );
		pShaderBlob->Release();
	}

	else
	{
		if( NULL != pErrorBlob )
		{
			MessageBoxA( 0, ( char* )pErrorBlob->GetBufferPointer(), 0, 0 );
			pErrorBlob->Release();
			pErrorBlob = nullptr;
		}

		return E_FAIL;
	}

	return S_OK;
}
