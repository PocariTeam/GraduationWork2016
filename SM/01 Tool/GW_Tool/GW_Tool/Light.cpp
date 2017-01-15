#include "stdafx.h"
#include "Light.h"

CLight::CLight( void )
	: m_pLightInfo( nullptr )
	, m_pCbVSLight( nullptr )
	, m_pCbPSLight( nullptr )
{

}

CLight::~CLight( void )
{
}

HRESULT CLight::Initialize( ID3D11Device* pDevice )
{
	m_pLightInfo = new LIGHT;
	CreateConstantBuffer( pDevice, ( int )m_pLightInfo->fType );

	return S_OK;
}

CLight* CLight::Create( ID3D11Device* pDevice )
{
	CLight*		pLight = new CLight;
	if( FAILED( pLight->Initialize( pDevice ) ) ) {
		pLight->Release();
		pLight = nullptr;
	}

	return pLight;
}

void CLight::Release( void )
{
		if( nullptr != m_pLightInfo ) {
			delete m_pLightInfo;
			m_pLightInfo = nullptr;
		}

		if( nullptr != m_pCbVSLight ) {
			m_pCbVSLight->Release();
			m_pCbVSLight = nullptr;
		}

		if( nullptr != m_pCbPSLight ) {
			m_pCbPSLight->Release();
			m_pCbPSLight = nullptr;
		}

		delete this;
}


void CLight::CreateConstantBuffer( ID3D11Device* pDevice, int iLightType )
{
	if( iLightType == LIGHT::LIGHT_POINT
		|| iLightType == LIGHT::LIGHT_SPOT )
		return;

	D3D11_BUFFER_DESC Buffer_Desc, Buffer_Desc2;
	ZeroMemory( &Buffer_Desc, sizeof( D3D11_BUFFER_DESC ) );
	ZeroMemory( &Buffer_Desc2, sizeof( D3D11_BUFFER_DESC ) );

	switch( iLightType ) {
	case LIGHT::LIGHT_DIRECTIONAL:
		Buffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
		Buffer_Desc.ByteWidth = sizeof( XMFLOAT4A );
		Buffer_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		Buffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		pDevice->CreateBuffer( &Buffer_Desc, NULL, &m_pCbVSLight );
		break;
	}

	switch( iLightType ) {
	case LIGHT::LIGHT_DIRECTIONAL:
		Buffer_Desc2.Usage = D3D11_USAGE_DYNAMIC;
		Buffer_Desc2.ByteWidth = sizeof( CB_LIGHT );
		Buffer_Desc2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		Buffer_Desc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		pDevice->CreateBuffer( &Buffer_Desc2, NULL, &m_pCbPSLight );
		break;
	}
}

void CLight::SetConstantBuffer( ID3D11DeviceContext* pContext )
{
	if( m_pLightInfo->fType == ( float )LIGHT::LIGHT_POINT
		|| m_pLightInfo->fType == ( float )LIGHT::LIGHT_SPOT )
		return;

	D3D11_MAPPED_SUBRESOURCE MappedSubresource, MappedSubresource2;
	ZeroMemory( &MappedSubresource, sizeof( D3D11_MAPPED_SUBRESOURCE ) );
	ZeroMemory( &MappedSubresource2, sizeof( D3D11_MAPPED_SUBRESOURCE ) );

	/* VS */
	switch( ( int )m_pLightInfo->fType ) {
	case LIGHT::LIGHT_DIRECTIONAL:
		pContext->Map( m_pCbVSLight, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );
		XMFLOAT4A* pCbLightDir = ( XMFLOAT4A * )MappedSubresource.pData;
		*pCbLightDir = XMFLOAT4A( m_pLightInfo->vDir.x, m_pLightInfo->vDir.y, m_pLightInfo->vDir.z, 0.f );
		pContext->Unmap( m_pCbVSLight, 0 );
		pContext->VSSetConstantBuffers( SLOT_LIGHT, 1, &m_pCbVSLight );
		break;
	}


	/* PS */
	switch( ( int )m_pLightInfo->fType ) {
	case LIGHT::LIGHT_DIRECTIONAL:
		pContext->Map( m_pCbPSLight, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource2 );
		CB_LIGHT* pCbLightInfo = ( CB_LIGHT * )MappedSubresource2.pData;
		pCbLightInfo->vDir = XMFLOAT4( m_pLightInfo->vDir.x, m_pLightInfo->vDir.y, m_pLightInfo->vDir.z, 0.f );
		pCbLightInfo->vDiffuse = m_pLightInfo->vDiffuse;
		pCbLightInfo->vAmbient = m_pLightInfo->vAmbient;
		pCbLightInfo->vSpecular = m_pLightInfo->vSpecular;
		pContext->Unmap( m_pCbPSLight, 0 );
		pContext->PSSetConstantBuffers( SLOT_LIGHT, 1, &m_pCbPSLight );
		break;
	}
}