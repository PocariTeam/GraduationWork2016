#pragma once
#ifndef NormalShader_h__
#define NormalShader_h__

#include "Shader.h"

class CGameObject;
class CNormalShader
	: public CShader
{
private:
	explicit CNormalShader();
	explicit CNormalShader( const CNormalShader& Instance );
	virtual ~CNormalShader();
public:
	CShader*	Clone( void );
	void		Update( const float& fTimeDelta );
	void		Render( ID3D11DeviceContext* pContext );
	DWORD		Release( void );
public:
	static CNormalShader*	Create( ID3D11Device* pDevice, CShader::INPUT_TYPE eInputType, const TCHAR* pFilePath );
private:
	HRESULT		Initialize( ID3D11Device* pDevice, CShader::INPUT_TYPE eInputType, const TCHAR* pFilePath );
public:
	void		Add_RenderObject( CGameObject* pGameObject );
private:
	vector<CGameObject*>	m_vecRenderObject;
};

#endif // NormalShader_h__