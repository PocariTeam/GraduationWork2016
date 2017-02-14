#pragma once

#ifndef _SINGLETEXTURE_H_
#define _SINGLETEXTURE_H_

#include "Texture.h"
class CSingleTexture :
	public CTexture
{
protected:
	CSingleTexture();
	explicit CSingleTexture( const CSingleTexture& Instance );
	virtual ~CSingleTexture();
private:
	HRESULT Initialize( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* pPath );
public:
	static CSingleTexture* Create( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* pPath );
public:
	virtual void Render( ID3D11DeviceContext* pContext );
	virtual CTexture* Clone() { return ( CTexture* )new CSingleTexture( *this ); }
private:
	ID3D11ShaderResourceView*		m_pShaderResourceView;
	ID3D11SamplerState*				m_pSamplerState;
public:
	virtual DWORD Release( void );
};

#endif