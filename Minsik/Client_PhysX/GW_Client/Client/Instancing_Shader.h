#ifndef _INSTANCING_SHADER_H_
#define _INSTANCING_SHADER_H_

#include "Shader.h"

class CGameObject;
class CInstancing_Shader
	: public CShader
{
protected:
	explicit CInstancing_Shader();
	CInstancing_Shader( const CInstancing_Shader& Instance );
	virtual ~CInstancing_Shader( void );
public:
	virtual CInstancing_Shader*   Clone( void );
private:
	HRESULT Initialize( ID3D11Device* pDevice, CShader::INPUT_ELEMENT eInput_Element, const TCHAR* pFilePath, bool bUse_GS, bool bUse_TS );
private:
	void	SetInstanceBuffer( ID3D11DeviceContext* pDeviceContext );
private:
	void	Create_InstanceBuffer( ID3D11Device* pDevice );
public:
	void	Set_RenderObject( CGameObject* pGameObject );
	static CShader*	Create( ID3D11Device* pDevice, CShader::INPUT_ELEMENT eInputElement, const TCHAR* pFilePath, bool bUse_GS, bool bUse_TS );
public:
	virtual DWORD Release( void );
	void Render( ID3D11DeviceContext* pDeviceContext );
private:
	ID3D11Device*			m_pDevice;
	ID3D11Buffer*			m_pInstanceBuffer;
	CShader::INPUT_ELEMENT	m_eInputElement;
};

#endif
