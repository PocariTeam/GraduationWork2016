#ifndef _BASE_SHADER_H_
#define _BASE_SHADER_H_

#include "Shader.h"

class CGameObject;
class CBase_Shader
	: public CShader
{
protected:
	explicit CBase_Shader();
	CBase_Shader( const CBase_Shader& Instance );
	virtual ~CBase_Shader( void );
public:
	virtual CBase_Shader*   Clone( void );
private:
	HRESULT Initialize( ID3D11Device* pDevice, CShader::INPUT_ELEMENT eInput_Element, const TCHAR* pFilePath, bool bUse_GS, bool bUse_TS );
public:
	static CShader*	Create( ID3D11Device* pDevice, CShader::INPUT_ELEMENT eInputElement, const TCHAR* pFilePath, bool bUse_GS, bool bUse_TS );
public:
	virtual DWORD Release( void );
	void Render( ID3D11DeviceContext* pDeviceContext );
};

#endif
