#pragma once
#ifndef ParticleShader_h__
#define ParticleShader_h__

#include "Shader.h"

class CGameObject;
class CParticleShader
	: public CShader
{
private:
	explicit CParticleShader();
	explicit CParticleShader( const CParticleShader& Instance );
	virtual ~CParticleShader();
public:
	CShader*	Clone( void );
	void		Update( const float& fTimeDelta );
	void		Render( ID3D11DeviceContext* pContext );
	DWORD		Release( void );
public:
	static CParticleShader*	Create( ID3D11Device* pDevice, CShader::INPUT_TYPE eInputType, const TCHAR* pFilePath );
private:
	HRESULT		Initialize( ID3D11Device* pDevice, CShader::INPUT_TYPE eInputType, const TCHAR* pFilePath );
public:
	vector<CGameObject*>*	GetRenderObjects() { return &m_vecRenderObject; }
public:
	void		Add_RenderObject( CGameObject* pGameObject );
private:
	vector<CGameObject*>		m_vecRenderObject;
	ID3D11VertexShader*			m_pSOVertexShader;
	ID3D11GeometryShader*		m_pSOGeometryShader;
	ID3D11DepthStencilState*	m_pDSState;
	ID3D11DepthStencilState*	m_pOriginDSState;
	ID3D11BlendState*			m_pBlendState;
};

#endif // NormalShader_h__