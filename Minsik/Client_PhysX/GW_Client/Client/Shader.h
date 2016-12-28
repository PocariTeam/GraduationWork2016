#ifndef _SHADER_H_
#define _SHADER_H_

#include "Base.h"

class CGameObject;
class CShader
	: public CBase
{
public:
	enum INPUT_ELEMENT { INPUT_BASE, INPUT_TANGENT, INPUT_POSITION_ONLY, INPUT_PARTICLE, INPUT_END };
protected:
	explicit CShader();
	CShader( const CShader& Instance );
	virtual ~CShader( void );
protected:
	virtual HRESULT CreateVertexShader( ID3D11Device* pDevice, const TCHAR* pFilePath, D3D11_INPUT_ELEMENT_DESC* pVTX_Element, UINT iElementCnt );
	virtual HRESULT CreatePixelShader( ID3D11Device* pDevice, const TCHAR* pFilePath );
	virtual HRESULT CreateGeometryShader( ID3D11Device* pDevice, const TCHAR* pFilePath );
	virtual HRESULT CreateHullDomainShader( ID3D11Device* pDevice, const TCHAR* pFilePath );
public:
	virtual CShader*   Clone( void ) = 0;
public:
	virtual void	Set_RenderObject( CGameObject* pGameObject ) { m_vecRenderObject.push_back( pGameObject ); }
	virtual void	Render( ID3D11DeviceContext* pContext ) = 0;
protected:
	ID3D11VertexShader*		m_pVertexShader;
	ID3D11PixelShader*		m_pPixelShader;
	ID3D11InputLayout*		m_pInputLayout;
	ID3D11GeometryShader*	m_pGeometryShader;
	ID3D11HullShader*		m_pHullShader;
	ID3D11DomainShader*		m_pDomainShader;

	vector<CGameObject*>	m_vecRenderObject;
public:
	virtual DWORD Release( void );
};

#endif
