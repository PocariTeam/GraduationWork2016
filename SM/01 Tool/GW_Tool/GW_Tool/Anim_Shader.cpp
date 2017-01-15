#include "stdafx.h"
#include "Anim_Shader.h"

HRESULT CAnim_Shader::Initialize( ID3D11Device* pDevice, const TCHAR* pFilePath )
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
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT
		, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT iElementCnt = ARRAYSIZE( VTX_Element );

	if( FAILED( CreateVertexShader( pDevice, pFilePath, VTX_Element, iElementCnt ) ) )
		return E_FAIL;

	if( FAILED( CreatePixelShader( pDevice, pFilePath ) ) )
		return E_FAIL;

	return S_OK;
}

CAnim_Shader* CAnim_Shader::Create( ID3D11Device* pDevice, const TCHAR* pFilePath )
{
	CAnim_Shader*	pInstance{ nullptr };

	pInstance = new CAnim_Shader;
	if( FAILED( pInstance->Initialize( pDevice, pFilePath ) ) )
	{
		pInstance->Release();
		pInstance = nullptr;
	}

	return ( CAnim_Shader* )pInstance;
}

void CAnim_Shader::Release( void )
{
	CShader::Release();

	delete this;
}