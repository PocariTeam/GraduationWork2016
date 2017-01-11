#pragma once

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "Tool_Header.h"

class CTexture
{
private:
	HRESULT Initialize( ID3D11Device* pDevice, vector<const TCHAR*>& vecPath );
	void	CreateConstantBuffer( ID3D11Device* pDevice );
public:
	void	SetConstantBuffer( ID3D11DeviceContext* pContext );
public:
	static CTexture* Create( ID3D11Device* pDevice, vector<const TCHAR*>& vecPath );
public:
	virtual void Render( ID3D11DeviceContext* pContext );
public:
	using MAP_TEXTURE = vector<pair<const TCHAR*, ID3D11ShaderResourceView*>>;
	MAP_TEXTURE						m_mapShaderResourceView;
	ID3D11SamplerState*				m_pSamplerState{ nullptr };
	size_t							m_iTextureCnt{ 0 };
	ID3D11Buffer*					m_pCBTexture{ nullptr };
public:
	virtual DWORD Release( void );
};

#endif