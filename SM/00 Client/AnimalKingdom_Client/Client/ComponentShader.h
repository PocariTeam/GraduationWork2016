#pragma once
#ifndef ComponentShader_h__
#define ComponentShader_h__

#define MAX_TRAIL_CNT 31
#include "Shader.h"

class CGameObject;
class CComponentShader
	: public CShader
{
private:
	explicit CComponentShader();
	explicit CComponentShader( const CComponentShader& Instance );
	virtual ~CComponentShader();
public:
	CShader*	Clone( void );
	void		Update( const float& fTimeDelta ) {}
	void		Render( ID3D11DeviceContext* pContext );
	DWORD		Release( void );
public:
	static CComponentShader*	Create( ID3D11Device* pDevice, CShader::INPUT_TYPE eInputType, const TCHAR* pFilePath );
private:
	HRESULT		Initialize( ID3D11Device* pDevice, CShader::INPUT_TYPE eInputType, const TCHAR* pFilePath );
public:
	void		Add_RenderObject( CGameObject* pGameObject ) {}
	void		SetConstantBuffer( ID3D11DeviceContext* pContext, LPVOID pData, UINT iDataSize );
};

#endif // ComponentShader_h__