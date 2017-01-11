#ifndef _BASICSHADER_H_
#define _BASICSHADER_H_

#include "Shader.h"

class CBasicShader
	: public CShader
{
private:
	CBasicShader();
	~CBasicShader() = default;
private:
	HRESULT Initialize( ID3D11Device* pDevice, const TCHAR* pFilePath );
public:
	static CBasicShader* Create( ID3D11Device* pDevice, const TCHAR* pFilePath );
	void Release( void );
};

#endif
