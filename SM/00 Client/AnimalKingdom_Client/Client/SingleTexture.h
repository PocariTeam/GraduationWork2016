#pragma once

#ifndef SingleTexture_h__
#define SingleTexture_h__

#include "Texture.h"
class CSingleTexture
	: public CTexture
{
public:
	explicit CSingleTexture();
	explicit CSingleTexture( const CSingleTexture& Instance );
	virtual ~CSingleTexture();
public:
	CTexture* Clone( void );
	void Render( ID3D11DeviceContext* pContext );
public:
	static CSingleTexture*	Create( ID3D11Device * pDevice, const char * pPath );
private:
	HRESULT Initialize( ID3D11Device* pDevice, const char* pPath );
private:
	ID3D11ShaderResourceView*		m_pShaderResourceView;
	ID3D11SamplerState*				m_pSamplerState;
public:
	DWORD Release( void );
};

#endif // SingleTexture_h__
