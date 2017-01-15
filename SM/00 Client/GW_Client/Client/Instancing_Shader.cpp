#include "stdafx.h"
#include "D3DCompiler.h"
#include "Instancing_Shader.h"
#include "Function.h"
#include "GameObject.h"

CInstancing_Shader::CInstancing_Shader()
	: CShader()
	, m_pInstanceBuffer( nullptr )
	, m_eInputElement( CShader::INPUT_BASE )
{
}

CInstancing_Shader::CInstancing_Shader( const CInstancing_Shader& Instance )
	: CShader( Instance )
	, m_pDevice( Instance.m_pDevice )
	, m_pInstanceBuffer( Instance.m_pInstanceBuffer )
	, m_eInputElement( Instance.m_eInputElement )
{
}

CInstancing_Shader::~CInstancing_Shader( void )
{

}

CInstancing_Shader* CInstancing_Shader::Clone( void )
{
	return new CInstancing_Shader( *this );
}

HRESULT CInstancing_Shader::Initialize( ID3D11Device* pDevice, CShader::INPUT_ELEMENT eInput_Element, const TCHAR* pFilePath, bool bUse_GS, bool bUse_TS )
{
	m_pDevice = pDevice;
	m_eInputElement = eInput_Element;
	D3D11_INPUT_ELEMENT_DESC* VTX_ElementDesc{ nullptr };
	UINT iElementCnt{};

	switch( eInput_Element )
	{
	case CShader::INPUT_BASE:
	{
		D3D11_INPUT_ELEMENT_DESC	VTX_Element[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "POSINSTANCE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "POSINSTANCE", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "POSINSTANCE", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "POSINSTANCE", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		};
		VTX_ElementDesc = VTX_Element;
		iElementCnt = ARRAYSIZE( VTX_Element );
	}
	break;
	case CShader::INPUT_TANGENT:
	{
		D3D11_INPUT_ELEMENT_DESC	VTX_Element[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "POSINSTANCE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "POSINSTANCE", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "POSINSTANCE", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "POSINSTANCE", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		};
		VTX_ElementDesc = VTX_Element;
		iElementCnt = ARRAYSIZE( VTX_Element );
	}
	break;
	case CShader::INPUT_POSITION_ONLY:
	{
		D3D11_INPUT_ELEMENT_DESC	VTX_Element[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "POSINSTANCE", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
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

void CInstancing_Shader::Set_RenderObject( CGameObject* pGameObject )
{
	m_vecRenderObject.push_back( pGameObject );
	Create_InstanceBuffer( m_pDevice );
}

CShader* CInstancing_Shader::Create( ID3D11Device* pDevice, CShader::INPUT_ELEMENT eInputElement, const TCHAR* pFilePath, bool bUse_GS, bool bUse_TS )
{
	CInstancing_Shader*	pShader = new CInstancing_Shader;

	if( FAILED( pShader->Initialize( pDevice, eInputElement, pFilePath, bUse_GS, bUse_TS ) ) )
	{
		pShader->Release();
		pShader = nullptr;
	}

	return ( CShader* )pShader;
}

DWORD CInstancing_Shader::Release( void )
{
	DWORD dwRefCnt = CShader::Release();

	if( 0 == dwRefCnt )
		::Safe_Release( m_pInstanceBuffer );

	delete this;
	return dwRefCnt;
}

void CInstancing_Shader::Render( ID3D11DeviceContext* pDeviceContext )
{
	if( m_vecRenderObject.empty() )
		return;

	pDeviceContext->IASetInputLayout( m_pInputLayout );

	SetInstanceBuffer( pDeviceContext );

	pDeviceContext->VSSetShader( m_pVertexShader, NULL, 0 );
	pDeviceContext->HSSetShader( m_pHullShader, NULL, 0 );
	pDeviceContext->DSSetShader( m_pDomainShader, NULL, 0 );
	pDeviceContext->GSSetShader( m_pGeometryShader, NULL, 0 );
	pDeviceContext->PSSetShader( m_pPixelShader, NULL, 0 );

	m_vecRenderObject.front()->Render( pDeviceContext, (DWORD)m_vecRenderObject.size() );
}

void CInstancing_Shader::SetInstanceBuffer( ID3D11DeviceContext* pDeviceContext )
{
	UINT	iStride{ 0 }, iOffset{ 0 };
	D3D11_MAPPED_SUBRESOURCE MappedSubresource;
	pDeviceContext->Map( m_pInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );

	switch( m_eInputElement )
	{
	case CShader::INPUT_BASE:
	case CShader::INPUT_TANGENT:
	{
		XMFLOAT4X4A* pArr = ( XMFLOAT4X4A* )MappedSubresource.pData;
		//for( size_t i = 0; i < m_vecRenderObject.size(); ++i )
		//	XMStoreFloat4x4A( &pArr[i], XMMatrixTranspose( XMLoadFloat4x4A( &m_vecRenderObject[i]->Get_Transform()->m_mtxWorld ) ) );
		iStride = sizeof( XMFLOAT4X4A );
	}
	break;
	case CShader::INPUT_POSITION_ONLY:
	{
		XMFLOAT3* pArr = ( XMFLOAT3* )MappedSubresource.pData;
		//for( size_t i = 0; i < m_vecRenderObject.size(); ++i )
		//	memcpy( &pArr[i], &m_vecRenderObject[i]->Get_Transform()->m_vPos, sizeof( XMFLOAT3 ) );
		iStride = sizeof( XMFLOAT3 );
	}
	break;
	};
	
	pDeviceContext->Unmap( m_pInstanceBuffer, 0 );
	pDeviceContext->IASetVertexBuffers( 1, 1, &m_pInstanceBuffer, &iStride, &iOffset );
}

void CInstancing_Shader::Create_InstanceBuffer( ID3D11Device * pDevice )
{
	size_t  dwInstancedCnt = m_vecRenderObject.size();
	::Safe_Release( m_pInstanceBuffer );

	ID3D11Buffer*	pInstanceBuffer;
	/* VS ¿ë World */
	D3D11_BUFFER_DESC Buffer_Desc;
	ZeroMemory( &Buffer_Desc, sizeof( Buffer_Desc ) );
	Buffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
	switch( m_eInputElement )
	{
	case INPUT_BASE:
	case INPUT_TANGENT:
		Buffer_Desc.ByteWidth = UINT( sizeof( XMFLOAT4X4A ) * dwInstancedCnt );
		break;
	case INPUT_POSITION_ONLY:
		Buffer_Desc.ByteWidth = UINT( sizeof( XMFLOAT3 ) * dwInstancedCnt );
		break;
	}

	Buffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	Buffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	pDevice->CreateBuffer( &Buffer_Desc, NULL, &pInstanceBuffer );

	m_pInstanceBuffer = pInstanceBuffer;
}
