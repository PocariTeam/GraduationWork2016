#pragma once

#ifndef MultiTexture_h__
#define MultiTexture_h__

#include "Texture.h"
class CMultiTexture
	: public CTexture
{
private:
	explicit CMultiTexture();
	explicit CMultiTexture( const CMultiTexture& Instance );
	virtual ~CMultiTexture();
public:
	CTexture* Clone( void );
	void Render( ID3D11DeviceContext* pContext );
	DWORD Release();
public:
	static CMultiTexture* Create( ID3D11Device* pDevice, const char* pPath, DWORD dwTextureCnt );
private:
	HRESULT Initialize( ID3D11Device* pDevice, const char* pPath, DWORD dwTextureCnt );
private:
	ID3D11ShaderResourceView**	m_dpShaderResourceView;
	ID3D11SamplerState**		m_dpSamplerState;
	DWORD						m_dwTextureCnt;
};

#endif // MultiTexture_h__
