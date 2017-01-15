#ifndef _SHADER_H_
#define _SHADER_H_

#include "Tool_Header.h"

class CShader
{
protected:
	CShader();
	virtual ~CShader() = default;
protected:
	HRESULT CreateVertexShader( ID3D11Device* pDevice, const TCHAR* pFilePath, D3D11_INPUT_ELEMENT_DESC* pVTX_Element, UINT iElementCnt );
	HRESULT CreatePixelShader( ID3D11Device* pDevice, const TCHAR* pFilePath );
public:
	virtual void	Render( ID3D11DeviceContext* pDeviceContext );
protected:
	ID3D11InputLayout*		m_pInputLayout;
	ID3D11VertexShader*		m_pVertexShader;
	ID3D11PixelShader*		m_pPixelShader;
public:
	virtual void Release( void );
};

#endif
