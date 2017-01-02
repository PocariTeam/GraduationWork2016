#include "stdafx.h"
#include "Loader.h"
#include "ResourceMgr.h"
#include "Function.h"
#include "ShaderMgr.h"
#include "RenderState.h"
#include "PhysicsMgr.h"
#include <process.h>

bool CLoader::m_bLoading = false;

CLoader::CLoader( ID3D11Device* pDevice, ID3D11DeviceContext* pContext )
	: m_pDevice( pDevice )
	, m_pContext( pContext )
	, m_bComplete( false )
{
	pDevice->AddRef();
	pContext->AddRef();
}

HRESULT CLoader::Initialize( void )
{
	if( !m_bLoading ) {
		InitializeCriticalSection( &m_CS );
		m_hThread = CreateThread( NULL, 0, Loading_Func, this, 0, NULL );
	}

	return S_OK;
}

void CLoader::Load_Resource( void )
{
	if( FAILED( CPhysicsMgr::GetInstance()->Initialize() ) )
		MessageBox( NULL, "CPhysicsMgr::Initialize Failed", 0, MB_OK );

	CRenderState::Initialize( m_pDevice );

	CResourceMgr::GetInstance()->AddMesh( m_pDevice, m_pContext, CResourceMgr::MESH_POINT, "Mesh_Point" );

	CResourceMgr::GetInstance()->AddMesh( m_pDevice, m_pContext, CResourceMgr::MESH_DEBUG, "Mesh_Sphere", XMFLOAT3(), "../Executable/Resources/Mesh/Sphere.jsm" );

	CResourceMgr::GetInstance()->AddMesh( m_pDevice, m_pContext, CResourceMgr::MESH_DEBUG, "Mesh_Box", XMFLOAT3(), "../Executable/Resources/Mesh/Box.jsm" );

	CResourceMgr::GetInstance()->AddMesh( m_pDevice, m_pContext, CResourceMgr::MESH_DEBUG, "Mesh_Capsule", XMFLOAT3(), "../Executable/Resources/Mesh/Capsule.jsm" );

	// Texture Load
	CResourceMgr::GetInstance()->AddTexture( m_pDevice, m_pContext
		, CResourceMgr::TEXTURE_SINGLE, "../Executable/Resources/Texture/metal.png", "Texture_Metal" );

	CResourceMgr::GetInstance()->AddTexture( m_pDevice, m_pContext
		, CResourceMgr::TEXTURE_SINGLE, "../Executable/Resources/Texture/Terrain.jpg", "Texture_Terrain" );

	CShaderMgr::GetInstance()->Create_Shaders( m_pDevice, CShader::INPUT_POSITION_ONLY
		, CShaderMgr::SHADER_BASE, "../Executable/Resources/Shader/Shader_Cube.fx", "Shader_Cube", true );

	CShaderMgr::GetInstance()->Create_Shaders( m_pDevice, CShader::INPUT_POSITION_ONLY
		, CShaderMgr::SHADER_BASE, "../Executable/Resources/Shader/Shader_Terrain.fx", "Shader_Terrain", true );
	
#ifdef _DEBUG
	CShaderMgr::GetInstance()->Create_Shaders( m_pDevice, CShader::INPUT_POSITION_ONLY
		, CShaderMgr::SHADER_BASE, "../Executable/Resources/Shader/Shader_Debug.fx", "Shader_Debug" );
#endif
	m_bLoading = true;
	m_bComplete = true;
}

CLoader* CLoader::Create( ID3D11Device* pDevice, ID3D11DeviceContext*	pContext )
{
	CLoader*	pLoading = new CLoader( pDevice, pContext );
	if( FAILED( pLoading->Initialize() ) ) {
		pLoading->Release();
		pLoading = nullptr;
	}

	return pLoading;
}

DWORD WINAPI CLoader::Loading_Func( void* pArg )
{
	CLoader*	pLoading = static_cast< CLoader* >( pArg );

	EnterCriticalSection( pLoading->Get_CS() );

	pLoading->Load_Resource();

	LeaveCriticalSection( pLoading->Get_CS() );

	return 0;
}

void CLoader::Release( void )
{
	::Safe_Release( m_pContext );
	::Safe_Release( m_pDevice );

	if( !m_bLoading ) {
		WaitForSingleObject( m_hThread, INFINITE );
		DeleteCriticalSection( &m_CS );
		CloseHandle( m_hThread );
	}

	delete this;
}