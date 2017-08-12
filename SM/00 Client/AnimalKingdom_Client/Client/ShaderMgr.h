#pragma once

#ifndef ShaderMgr_h__
#define ShaderMgr_h__

#include "Singleton.h"
#include "Shader.h"

class CShaderMgr
	: public CSingleton<CShaderMgr>
{
public:
	enum eShaderType{ SHADER_NORMAL, SHADER_INSTANCING, SHADER_COMPONENT, SHADER_STENCIL, SHADER_END };
	using SHADER_TYPE = eShaderType;
public:
	CShader* Clone( const char* pKey );
	CShader* Find( const char* pKey );
public:
	HRESULT Add( ID3D11Device* pDevice, const char* pKey, CShader* pShader );
	HRESULT Add( ID3D11Device* pDevice, eShaderType eType, CShader::INPUT_TYPE eInputType, const char* pKey, const char* pFilePath, UINT dwInstanceCnt = 1 );
	HRESULT Load( ID3D11Device* pDevice, const char* pFilePath );
	DWORD	Release( void );
private:
	using SHADERCONTAINER = map<const string, CShader*>;
	SHADERCONTAINER			m_mapShader;
};

#endif // ShaderMgr_h__