#include "stdafx.h"
#include "ShaderMgr.h"
#include "Instancing_Shader.h"
#include "Base_Shader.h"

CShaderMgr*	CShaderMgr::m_pInstance = nullptr;

CShaderMgr*	CShaderMgr::GetInstance()
{
	if( nullptr == m_pInstance )
		m_pInstance = new CShaderMgr;

	return m_pInstance;
}
void CShaderMgr::DestroyInstance()
{
	if( nullptr != m_pInstance )
	{
		m_pInstance->Release();
		m_pInstance = nullptr;
	}
}

CShaderMgr::CShaderMgr( void )
{

}

CShaderMgr::~CShaderMgr( void )
{

}

CShader* CShaderMgr::Clone_Shaders( const TCHAR* pShaderTag )
{
	CShader* pShader = Find_Shader( pShaderTag );

	if( nullptr == pShader )
		return nullptr;

	return pShader->Clone();
}

HRESULT CShaderMgr::Create_Shaders( ID3D11Device* pDevice, CShader::INPUT_ELEMENT eInputElement, SHADER_TYPE eShaderType,const TCHAR* pFilePath, const TCHAR* pShaderTag, bool bUse_GS, bool bUse_TS )
{
	CShader*		pShader = Find_Shader( pShaderTag );

	if( nullptr != pShader )
		return E_FAIL;

	switch( eShaderType )
	{
	case SHADER_BASE:
		pShader = CBase_Shader::Create( pDevice, eInputElement, pFilePath, bUse_GS, bUse_TS );
		break;
	case SHADER_INSTANCING:
		pShader = CInstancing_Shader::Create( pDevice, eInputElement, pFilePath, bUse_GS, bUse_TS );
		break;
	case SHADER_PARTICLE:
		// pShader = CParticle_Shader::Create( pDevice, eInputElement, pFilePath, bUse_GS, bUse_TS );
		break;
	default:
		return E_FAIL;
	}

	if( nullptr == pShader )
		return E_FAIL;

	m_mapShaders.insert( MAPSHADERS::value_type( pShaderTag, pShader ) );

	return S_OK;
}

CShader* CShaderMgr::Find_Shader( const TCHAR* pShaderTag )
{
	MAPSHADERS::iterator	iter = m_mapShaders.find( pShaderTag );

	if( iter == m_mapShaders.end() )
		return NULL;

	return iter->second;
}

void CShaderMgr::Release( void )
{
	auto map_iter = m_mapShaders.begin();
	auto map_Enditer = m_mapShaders.end();

	for( ; map_iter != map_Enditer; ++map_iter ) {
		if( nullptr != ( *map_iter ).second ) {
			( *map_iter ).second->Release();
			( *map_iter ).second = nullptr;
		}
	}

	m_mapShaders.clear();

	delete this;
}

