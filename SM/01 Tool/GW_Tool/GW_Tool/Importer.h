#pragma once

#include "Tool_Header.h"

class CFbxContext;
class CGraphicDev;
class CMesh;
class CTexture;
class CTransform;
class CShader;
class CAnimation_Controller;
class CImporter
{
private:
	CImporter();
	~CImporter() = default;

public:
	HRESULT Initialize( CGraphicDev* pGraphicDev, const char* pFilePath );
	int		Update( float fTimeDelta );
	void	Render( ID3D11DeviceContext* pContext );
	void	Export_JSM();
public:
	static CImporter* Create( CGraphicDev* pGraphicDev, const char* pFilePath );
public:
	void	Release( void );
private:
	void	Animate( float fTimeDelta );
	void	ConvertFileName( string& Out, const char* pSrc );
	void	ConvertFilePath( string& Out, const char* pSrc );
	void	ConvertAnimationName( string& Out, const char* pSrc );
private:
	void	Create_FBXContext( const char* pFilePath );
	void	Create_Texture();
	void	Create_Mesh();
	void    Create_Shader();
public:
	/* 애니메이션 재생 On / Off */
	bool					m_bAnimate;
	/* Fbx File 경로 - Texture 경로를 상대적으로 접근하기 위함 */
	string					m_strPath;
	/* Fbx File 이름 ( 경로 포함, 확장자는 자름 ) - Export 하기 위함 */
	string					m_strFileName;
	CGraphicDev*			m_pGraphicDev;
	/* 실제적으로 FBX SDK 로 데이터를 뽑아오는 클래스 */
	CFbxContext*			m_pFbxContext;
	CTexture*				m_pTexture;
	CTransform*				m_pTransform;
	CShader*				m_pShader;
	CMesh*					m_pMesh;
	/* 애니메이션 재생기 */
	CAnimation_Controller*	m_pAnimation_Controller;
};

