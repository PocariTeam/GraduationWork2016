#pragma once
#ifndef StencilShader_h__
#define StencilShader_h__

#include "Shader.h"

class CGameObject;
class CStencilShader
	: public CShader
{
private:
	explicit CStencilShader();
	explicit CStencilShader( const CStencilShader& Instance );
	virtual ~CStencilShader();
public:
	CShader*	Clone( void );
	void		Update( const float& fTimeDelta );
	void		Render( ID3D11DeviceContext* pContext );
	DWORD		Release( void );
public:
	static CStencilShader*	Create( ID3D11Device* pDevice, CShader::INPUT_TYPE eInputType, const TCHAR* pFilePath );
private:
	HRESULT		Initialize( ID3D11Device* pDevice, CShader::INPUT_TYPE eInputType, const TCHAR* pFilePath );
public:
	vector<CGameObject*>*	GetRenderObjects() { return &m_vecRenderObject; }
public:
	void		Add_RenderObject( CGameObject* pGameObject );
private:
	vector<CGameObject*>	m_vecRenderObject;
};

#endif // StencilShader_h__