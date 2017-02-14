#pragma once

#ifndef Texture_h__
#define Texture_h__

#include "Resources.h"

class CTexture
	: public CResources
{
protected:
	explicit CTexture();
	explicit CTexture( const CTexture& Instance );
	virtual ~CTexture();
public:
	virtual CTexture* Clone( void )PURE;
	virtual void Render( ID3D11DeviceContext* pContext )PURE;
public:
	virtual DWORD Release();
};

#endif // Texture_h__