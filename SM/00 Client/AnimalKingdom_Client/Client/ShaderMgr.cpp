#include "stdafx.h"
#include "ShaderMgr.h"
#include <fstream>
#include "Functor.h"
#include "NormalShader.h"
#include "InstancingShader.h"
#include "ComponentShader.h"
#include "StencilShader.h"

CShaderMgr*		CSingleton<CShaderMgr>::m_pInstance;

CShader* CShaderMgr::Clone( const char* pKey )
{
	CShader*	pShader = Find( pKey );
	if( nullptr == pShader ) return nullptr;
	return pShader->Clone();
}

CShader* CShaderMgr::Find( const char* pKey )
{
	SHADERCONTAINER::iterator		iter_Find = m_mapShader.find( pKey );
	if( iter_Find == m_mapShader.end() )  return nullptr;
	return iter_Find->second;
}

HRESULT CShaderMgr::Add( ID3D11Device* pDevice, const char* pKey, CShader* pShader )
{
	CShader* pFind = Find( pKey );
	if( nullptr != pFind )
	{
		MessageBox( nullptr, "Already Shader Exists", nullptr, MB_OK );
		return E_FAIL;
	}

	if( nullptr == pShader ) return E_FAIL;

	m_mapShader.insert( make_pair( pKey, pShader ) );
	return S_OK;
}

HRESULT CShaderMgr::Add( ID3D11Device* pDevice, eShaderType eType, CShader::INPUT_TYPE eInputType, const char* pKey, const char* pFilePath, UINT dwInstanceCnt /*= 1 */ )
{
	CShader* pShader{ Find( pKey ) };

	if( nullptr != pShader )
	{
		MessageBox( nullptr, "Already Shader Exists", nullptr, MB_OK );
		return E_FAIL;
	}

	switch( eType )
	{
	case CShaderMgr::SHADER_NORMAL:
		pShader = CNormalShader::Create( pDevice, eInputType, pFilePath );
		break;
	case CShaderMgr::SHADER_INSTANCING:
		pShader = CInstancingShader::Create( pDevice, eInputType, pFilePath );
		break;
	case CShaderMgr::SHADER_COMPONENT:
		pShader = CComponentShader::Create( pDevice, eInputType, pFilePath );
		break;
	case CShaderMgr::SHADER_STENCIL:
		pShader = CStencilShader::Create( pDevice, eInputType, pFilePath );
		break;
	}

	if( nullptr == pShader )
	{
		MessageBox( nullptr, "Shader Compile Failed", nullptr, MB_OK );
		return E_FAIL;
	}

	m_mapShader.insert( make_pair( pKey, pShader ) );

	return S_OK;
}

HRESULT CShaderMgr::Load( ID3D11Device* pDevice, const char* pFilePath )
{
	HRESULT		hr{};
	ifstream	pIn{ pFilePath };
	int			iAllCnt{}, iType{}, iInputType{}, iSoleCnt{};
	char		szKey[ MAX_PATH ]{ "" }, szPath[ MAX_PATH ]{ "" };

	pIn >> iAllCnt;
	for( int i = 0; i < iAllCnt; ++i )
	{
		pIn >> szKey;
		pIn >> iType;
		pIn >> iInputType;
		pIn >> iSoleCnt;
		pIn >> szPath;

		Add( pDevice, ( eShaderType )iType, ( CShader::INPUT_TYPE )iInputType, szKey, szPath, iSoleCnt );
	}

	pIn.close();
	return S_OK;
}

DWORD CShaderMgr::Release( void )
{
	for_each( m_mapShader.begin(), m_mapShader.end(), ReleasePair() );
	delete this;
	return 0;
}