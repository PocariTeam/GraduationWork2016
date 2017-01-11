#include "stdafx.h"
#include "Shader.h"
#include "D3DCompiler.h"

CShader::CShader()
	:m_pInputLayout( nullptr )
	, m_pVertexShader( nullptr )
	, m_pPixelShader( nullptr )
{
}

HRESULT CShader::CreateVertexShader( ID3D11Device* pDevice, const TCHAR* pFilePath, D3D11_INPUT_ELEMENT_DESC* pVTX_Element, UINT iElementCnt )
{
	DWORD		dwShaderFlag = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlag |= D3D10_SHADER_DEBUG;
	dwShaderFlag |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3DBlob *pShaderBlob = NULL, *pErrorBlob = NULL;

	/*파일(pszFileName)에서 쉐이더 함수(pszShaderName)를 컴파일하여 컴파일된 쉐이더 코드의 메모리 주소(pd3dShaderBlob)를 반환한다.*/
	if( SUCCEEDED( D3DX11CompileFromFile( pFilePath, NULL, NULL, "VS"
		, "vs_5_0", dwShaderFlag, 0, NULL, &pShaderBlob, &pErrorBlob, NULL ) ) )
	{
		//컴파일된 쉐이더 코드의 메모리 주소에서 정점-쉐이더를 생성한다. 
		pDevice->CreateVertexShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pVertexShader );
		//컴파일된 쉐이더 코드의 메모리 주소와 입력 레이아웃에서 정점 레이아웃을 생성한다. 
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
	/*파일(pszFileName)에서 쉐이더 함수(pszShaderName)를 컴파일하여 컴파일된 쉐이더 코드의 메모리 주소(pd3dShaderBlob)를 반환한다.*/
	if( SUCCEEDED( D3DX11CompileFromFile( pFilePath, NULL, NULL, "PS"
		, "ps_5_0", dwShaderFlag, 0, NULL, &pShaderBlob, &pErrorBlob, NULL ) ) )
	{
		//컴파일된 쉐이더 코드의 메모리 주소에서 픽셀-쉐이더를 생성한다. 
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

void CShader::Render( ID3D11DeviceContext* pDeviceContext )
{
	//정점의 입력-레이아웃을 디바이스 컨텍스트에 연결(설정)한다. 
	if( nullptr != m_pInputLayout )
		pDeviceContext->IASetInputLayout( m_pInputLayout );
	//정점-쉐이더를 디바이스 컨텍스트에 연결(설정)한다. 
	if( nullptr != m_pVertexShader )
		pDeviceContext->VSSetShader( m_pVertexShader, NULL, 0 );
	//픽셀-쉐이더를 디바이스 컨텍스트에 연결(설정)한다. 
	if( nullptr != m_pPixelShader )
		pDeviceContext->PSSetShader( m_pPixelShader, NULL, 0 );
}

void CShader::Release( void )
{
	if( nullptr != m_pPixelShader )
	{
		m_pPixelShader->Release();
		m_pPixelShader = nullptr;
	}

	if( nullptr != m_pVertexShader )
	{
		m_pVertexShader->Release();
		m_pVertexShader = nullptr;
	}

	if( nullptr != m_pInputLayout )
	{
		m_pInputLayout->Release();
		m_pInputLayout = nullptr;
	}
}