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
	HRESULT		CreateConstantBuffer( ID3D11Device* pDevice, UINT iBufferSize = sizeof( CB_WORLD ) );
public:
	list<CGameObject*>*	GetRenderObjects() { return &m_listRenderObject; }
public:
	void		Add_RenderObject( CGameObject* pGameObject );
	void		SetConstantBuffer( ID3D11DeviceContext* pContext, XMFLOAT4X4 mtxWorld, XMFLOAT4 vOption = XMFLOAT4( 0.f, 0.f, 0.f, 0.f ) );
private:
	list<CGameObject*>			m_listRenderObject;
	ID3D11Device*				m_pDevice;
	ID3D11Buffer*				m_pInstancingBuffer;
	INPUT_TYPE					m_eInputElement;
};

#endif // NormalShader_h__