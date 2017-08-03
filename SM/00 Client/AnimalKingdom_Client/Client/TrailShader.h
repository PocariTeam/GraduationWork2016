#pragma once
#ifndef TrailShader_h__
#define TrailShader_h__

#define MAX_TRAIL_CNT 31
#include "Shader.h"

class CGameObject;
class CTrailShader
	: public CShader
{
private:
	explicit CTrailShader();
	explicit CTrailShader( const CTrailShader& Instance );
	virtual ~CTrailShader();
public:
	CShader*	Clone( void );
	void		Update( const float& fTimeDelta );
	void		Render( ID3D11DeviceContext* pContext );
	DWORD		Release( void );
public:
	static CTrailShader*	Create( ID3D11Device* pDevice, CShader::INPUT_TYPE eInputType, const TCHAR* pFilePath );
private:
	HRESULT		Initialize( ID3D11Device* pDevice, CShader::INPUT_TYPE eInputType, const TCHAR* pFilePath );
public:
	vector<CGameObject*>*	GetRenderObjects() { return &m_vecRenderObject; }
public:
	void		Add_RenderObject( CGameObject* pGameObject );
	void		SetConstantBuffer( ID3D11DeviceContext* pContext, LPVOID pData, UINT iDataSize );
private:
	vector<CGameObject*>		m_vecRenderObject;
};

#endif // TrailShader_h__