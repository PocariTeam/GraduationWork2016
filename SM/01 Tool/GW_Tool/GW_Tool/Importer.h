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
	/* �ִϸ��̼� ��� On / Off */
	bool					m_bAnimate;
	/* Fbx File ��� - Texture ��θ� ��������� �����ϱ� ���� */
	string					m_strPath;
	/* Fbx File �̸� ( ��� ����, Ȯ���ڴ� �ڸ� ) - Export �ϱ� ���� */
	string					m_strFileName;
	CGraphicDev*			m_pGraphicDev;
	/* ���������� FBX SDK �� �����͸� �̾ƿ��� Ŭ���� */
	CFbxContext*			m_pFbxContext;
	CTexture*				m_pTexture;
	CTransform*				m_pTransform;
	CShader*				m_pShader;
	CMesh*					m_pMesh;
	/* �ִϸ��̼� ����� */
	CAnimation_Controller*	m_pAnimation_Controller;
};

