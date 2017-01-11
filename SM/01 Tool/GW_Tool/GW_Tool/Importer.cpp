#include "stdafx.h"
#include "Importer.h"
#include "FbxContext.h"
#include "Mesh.h"
#include "GraphicDev.h"
#include "Texture.h"
#include "Transform.h"
#include "..\ThirdParty\include\fbxsdk\scene\shading\fbxtexture.h"
#include "..\ThirdParty\include\fbxsdk\scene\shading\fbxfiletexture.h"
#include "Mesh.h"
#include "StaticMesh.h"
#include "AnimateMesh.h"
#include "Anim_Shader.h"
#include "Animation_Controller.h"
#include "BasicShader.h"
#include "Shader.h"
#include "AnimationSet.h"

CImporter::CImporter()
	:m_pGraphicDev( nullptr )
	, m_bAnimate( false )
	, m_strPath( "" )
	, m_strFileName( "" )
	, m_pFbxContext( nullptr )
	, m_pMesh( nullptr )
	, m_pTexture( nullptr )
	, m_pTransform( nullptr )
	, m_pShader( nullptr )
	, m_pAnimation_Controller( nullptr )
{
}

HRESULT CImporter::Initialize( CGraphicDev* pGraphicDev, const char * pFilePath )
{
	m_pGraphicDev = pGraphicDev;

	ConvertFilePath( m_strPath, pFilePath );
	ConvertFileName( m_strFileName, pFilePath );

	Create_FBXContext( pFilePath );
	Create_Texture();
	m_pTransform = CTransform::Create( pGraphicDev->Get_Device() );
	Create_Shader();
	Create_Mesh();
	ConvertAnimationName( m_pFbxContext->m_strAnimationName, m_strFileName.c_str() );

	if( m_pFbxContext->m_bIncludeAnimation )
		m_pAnimation_Controller = CAnimation_Controller::Create( pGraphicDev->Get_Device(), m_pFbxContext->m_strAnimationName.c_str(), m_pFbxContext->m_vecJoint, ( DWORD )m_pFbxContext->m_vecJoint.size(), 60, ( DWORD )m_pFbxContext->m_llAnimationLength );
	
	return S_OK;
}

int CImporter::Update( float fTimeDelta )
{
	if( m_pTransform ) m_pTransform->Update( fTimeDelta );
	if( m_bAnimate ) Animate( fTimeDelta );

	return 0;
}

void CImporter::Render( ID3D11DeviceContext * pContext )
{
	if( nullptr != m_pShader ) m_pShader->Render( pContext );
	if( m_pTransform ) m_pTransform->Render(pContext);
	if( nullptr != m_pAnimation_Controller && m_bAnimate )m_pAnimation_Controller->Render(pContext);
	if( nullptr != m_pTexture ) m_pTexture->Render( pContext );
	if( m_pMesh ) m_pMesh->Render( pContext );
}

/* 인덱스 버퍼는 추가하지 않음 */
void CImporter::Export_JSM()
{
	ofstream Out{ m_strFileName + "jsm"/*, ios::binary*/ };

	char szType{};

	if( m_pFbxContext->m_bIncludeAnimation ) szType = 'a';			// 애니메이션을  포함한 매쉬
	else if( nullptr != m_pTexture ) szType = 'm';					// 애니메이션이 없는 메쉬
	else szType = 'b';												// 물리엔진용 디버그 메쉬

	Out << szType << endl;

	// Texture - t, 개수, 경로
	if( nullptr != m_pTexture )
	{
		Out << "t" << endl;
		Out << m_pTexture->m_iTextureCnt << endl;
		auto Advance_TextureIter = m_pTexture->m_mapShaderResourceView.begin();

		for( int i = 0; i < m_pTexture->m_iTextureCnt; ++i, ++Advance_TextureIter )
		{
			string strTemp{};
			wstring wstrTemp{ Advance_TextureIter->first };
			strTemp.assign( wstrTemp.begin(), wstrTemp.end() );
			Out << strTemp << endl;
		}
	}

	// 정점 정보
	Out << "v" << endl;
	size_t dwVertexCnt = m_pFbxContext->m_vecVertex.size();
	Out << dwVertexCnt << endl;

	for( DWORD i = 0; i < dwVertexCnt; ++i )
	{
		Out << m_pFbxContext->m_vecVertex[ i ].tVertex.vPos.x << " " << m_pFbxContext->m_vecVertex[ i ].tVertex.vPos.y << " " << m_pFbxContext->m_vecVertex[ i ].tVertex.vPos.z << endl;
		if( szType == 'a' )
			for( int j = 0; j < 4; ++j )
				Out << m_pFbxContext->m_vecVertex[ i ].vecBlendInfo[j].dwIndex << " " << m_pFbxContext->m_vecVertex[ i ].vecBlendInfo[ j ].dWeight << endl;
	}

	// 인덱스 정보
	Out << "i" << endl;
	size_t dwIndexCnt = m_pFbxContext->m_vecIndex.size();
	Out << dwIndexCnt << endl;

	for( DWORD i = 0; i < dwIndexCnt; ++i )
		Out << m_pFbxContext->m_vecIndex[ i ] << endl;
		
	if( szType != 'a' )
	{
		Out.close();
		return;
	}

	// 관절 정보 - 애니메이션을 의미 ( j, 애니메이션 개수( 종류 ), 관절 수, *애니메이션 이름, 길이, 스피드, *행렬 )
	Out << "j" << endl;
	Out << m_pAnimation_Controller->m_iAnimationSetCnt << endl;
	size_t iJointCnt = m_pAnimation_Controller->m_vecAnimationSet.front()->m_vecJoint.size();
	Out << iJointCnt << endl;

	for( int i = 0; i < m_pAnimation_Controller->m_iAnimationSetCnt; ++i )
	{
		Out << m_pAnimation_Controller->m_vecAnimationSet[ i ]->m_strName << endl;
		Out << m_pAnimation_Controller->m_vecAnimationSet[ i ]->m_iLength << endl;
		Out << m_pAnimation_Controller->m_vecAnimationSet[ i ]->m_fSpeed << endl;
		for( size_t j = 0; j < iJointCnt; ++j )
		{
			Keyframe* pKeyframe = m_pAnimation_Controller->m_vecAnimationSet[ i ]->m_vecJoint[ j ].pAnimation;
			Keyframe* pAdvancedKeyframe = pKeyframe;
			while( nullptr != pAdvancedKeyframe )
			{
				pKeyframe = pAdvancedKeyframe;
				pAdvancedKeyframe = pAdvancedKeyframe->m_pNext;

				for( int k = 0; k < 4; k++ )
				{
					for( int l = 0; l < 4; l++ )
						Out << pKeyframe->m_mtxGlobalTransform.m[k][l] << " ";
				}
				
				Out << endl;
			}
		}
	}
	
	Out.close();
}

CImporter* CImporter::Create( CGraphicDev* pGraphicDev, const char* pFilePath )
{
	CImporter*	pImporter = new CImporter;

	if( FAILED( pImporter->Initialize( pGraphicDev, pFilePath ) ) )
	{
		pImporter->Release();
		pImporter = nullptr;
	}

	return pImporter;
}

void CImporter::Release( void )
{
	m_strFileName.erase( m_strFileName.begin(), m_strFileName.end() );
	m_strPath.erase( m_strPath.begin(), m_strPath.end() );

	if( nullptr != m_pMesh ) {
		m_pMesh->Release();
		m_pMesh = nullptr;
	}

	if( nullptr != m_pShader ) {
		m_pShader->Release();
		m_pShader = nullptr;
	}

	if( nullptr != m_pTransform ) {
		m_pTransform->Release();
		m_pTransform = nullptr;
	}

	if( nullptr != m_pTexture )
	{
		m_pTexture->Release();
		m_pTexture = nullptr;
	}

	if( nullptr != m_pAnimation_Controller )
	{
		m_pAnimation_Controller->Release();
		m_pAnimation_Controller = nullptr;
	}

	if( nullptr != m_pFbxContext ) {
		delete m_pFbxContext;
		m_pFbxContext = nullptr;
	}

	delete this;
}

void CImporter::Animate( float fTimeDelta )
{
	if( !m_pFbxContext->m_bIncludeAnimation ) return;
	m_pAnimation_Controller->Animate(fTimeDelta);
}

void CImporter::ConvertFileName( string& Out, const char* pSrc )
{
	/* 파일 이름 자르기 */
	Out = pSrc;
	string strReverseFileName{ "" };
	strReverseFileName.assign( Out.rbegin(), Out.rend() );
	size_t iIndex = strReverseFileName.find( ".", 1 );
	Out.clear();

	strReverseFileName.erase( 0, iIndex );
	Out.assign( strReverseFileName.rbegin(), strReverseFileName.rend() );
}

void CImporter::ConvertFilePath( string& Out, const char* pSrc )
{
	/* 파일 경로 자르기 */
	Out = pSrc;
	string strReverseFileName{ "" };
	strReverseFileName.assign( Out.rbegin(), Out.rend() );
	size_t iIndex = strReverseFileName.find( "\\", 2 );
	Out.clear();

	strReverseFileName.erase( 0, iIndex );
	Out.assign( strReverseFileName.rbegin(), strReverseFileName.rend() );
}

void CImporter::ConvertAnimationName( string& Out, const char* pSrc )
{
	/* 파일 이름 자르기 */
	string OriginName = Out;
	Out = pSrc;
	string strReverseFileName{ "" };
	strReverseFileName.assign( Out.rbegin(), Out.rend() );
	size_t iIndex = strReverseFileName.find( "@", 1 );
	if( iIndex > strlen( pSrc ) )
	{
		Out = OriginName;
		return;
	}

	Out.clear();

	strReverseFileName.erase( iIndex, strReverseFileName.length() );
	Out.assign( strReverseFileName.rbegin(), strReverseFileName.rend() );
	Out.pop_back();
}

void CImporter::Create_FBXContext( const char* pFilePath )
{
	m_pFbxContext = new CFbxContext;
	m_pFbxContext->Initialize();
	m_pFbxContext->Load_Scene( pFilePath );
	m_pFbxContext->Recursive_Hierarchy();
}

void CImporter::Create_Texture()
{
	/* 텍스쳐 경로를 읽어서 생성 */
	int iTextureCnt = m_pFbxContext->m_pScene->GetTextureCount();
	FbxArray<FbxTexture*>	arrTexture;
	m_pFbxContext->m_pScene->FillTextureArray( arrTexture );

	vector<const TCHAR*> vecTexturePath;
	for( int i = 0; i < iTextureCnt; ++i )
	{
		string strRelativePath = ( ( FbxFileTexture* )arrTexture[ i ] )->GetRelativeFileName();
		string strTexturePath = m_strPath + strRelativePath;
		wstring wstrTexturePath{ 0 };
		wstrTexturePath.assign( strTexturePath.begin(), strTexturePath.end() );

		TCHAR* szTexturePath = new TCHAR[ MAX_PATH ];
		ZeroMemory( szTexturePath, sizeof( TCHAR ) * MAX_PATH );
		wsprintf( szTexturePath, wstrTexturePath.c_str() );
		vecTexturePath.push_back( szTexturePath );
	}

	m_pTexture = CTexture::Create( m_pGraphicDev->Get_Device(), vecTexturePath );
}

void CImporter::Create_Mesh()
{
	ID3D11Device* pDevice =  m_pGraphicDev->Get_Device();

	if( m_pFbxContext->m_bIncludeAnimation )
		m_pMesh = CAnimateMesh::Create( pDevice, &m_pFbxContext->m_vecVertex, &m_pFbxContext->m_vecIndex );
	else
		m_pMesh = CStaticMesh::Create( pDevice, &m_pFbxContext->m_vecVertex, &m_pFbxContext->m_vecIndex );
}

void CImporter::Create_Shader()
{
	ID3D11Device* pDevice = m_pGraphicDev->Get_Device();

	if( m_pFbxContext->m_bIncludeAnimation )
		m_pShader = CAnim_Shader::Create( pDevice, L"../Executable/Resource/HLSL/Shader_Anitest.fx" );
		
	else
		m_pShader = CBasicShader::Create( pDevice, L"../Executable/Resource/HLSL/Shader_Directional.fx" );
}
