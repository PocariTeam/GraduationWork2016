#ifndef _ANIM_SHADER_H_
#define _ANIM_SHADER_H_

#include "Shader.h"

class CAnim_Shader
	: public CShader
{
private:
	HRESULT Initialize( ID3D11Device* pDevice, const TCHAR* pFilePath );
public:
	static CAnim_Shader* Create( ID3D11Device* pDevice, const TCHAR* pFilePath );
	virtual void Release( void );
};

#endif
