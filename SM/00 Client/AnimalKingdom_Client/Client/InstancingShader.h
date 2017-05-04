#pragma once
#ifndef InstancingShader_h__
#define InstancingShader_h__

#include "Shader.h"

class CGameObject;
class CInstancingShader
	: public CShader
{
private:
	explicit CInstancingShader();
	explicit CInstancingShader( const CInstancingShader& Instance );
	virtual ~CInstancingShader();
public:
	CShader*	Clone( void );
	void		Update( const float& fTimeDelta );
	void		Render( ID3D11DeviceContext* pContext );
	DWORD		Release( void );
public:
	static CInstancingShader*	Create( ID3D11Device* pDevice, CShader::INPUT_TYPE eInputType, const TCHAR* pFilePath );
private:
	HRESULT		Initialize( ID3D11Device* pDevice, CShader::INPUT_TYPE eInputType, const TCHAR* pFilePath );
	HRESULT		CreateConstantBuffer( ID3D11Device* pDevice, UINT iBufferSize = sizeof( CB_WORLD ) );
public:
	vector<CGameObject*>*	GetRenderObjects() { return &m_vecRenderObject; }
public:
	void		Add_RenderObject( CGameObject* pGameObject );
	void		SetConstantBuffer( ID3D11DeviceContext* pContext, XMFLOAT4X4 mtxWorld, XMFLOAT4 vOption = XMFLOAT4( 0.f, 0.f, 0.f, 0.f ) );
private:
	vector<CGameObject*>	m_vecRenderObject;
	ID3D11Buffer*			m_pInstancingBuffer;
	INPUT_TYPE				m_eInputElement;
};

#endif // InstancingShader_h__