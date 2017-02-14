#ifndef _SHADERMGR_H_
#define _SHADERMGR_H_

#include "Shader.h"

class CShaderMgr
{
	explicit CShaderMgr( void );
	~CShaderMgr( void );
public:
	enum SHADER_TYPE { SHADER_BASE, SHADER_INSTANCING, SHADER_PARTICLE, SHADER_END };
private:
	static CShaderMgr*	m_pInstance;
public:
	static CShaderMgr*	GetInstance( void );
	static void			DestroyInstance( void );
public: // Getter
	CShader* Clone_Shaders( const TCHAR* pShaderTag );
public:
	HRESULT Create_Shaders( ID3D11Device* pDevice, CShader::INPUT_ELEMENT eInputElement, SHADER_TYPE eShaderType, const TCHAR* pFilePath, const TCHAR* pShaderTag, bool bUse_GS = false, bool bUse_TS = false );
private:
	typedef map<const TCHAR*, CShader*>		MAPSHADERS;
	MAPSHADERS								m_mapShaders;
private:
	CShader* Find_Shader( const TCHAR* pShaderTag );
public:
	void Release( void );
};

#endif 