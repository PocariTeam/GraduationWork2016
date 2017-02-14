#pragma once

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "Base.h"
class CTexture :
	public CBase
{
protected:
	CTexture();
	explicit CTexture( const CTexture& Instance );
	virtual ~CTexture();
public:
	virtual CTexture* Clone() { return nullptr; }
	virtual void Render( ID3D11DeviceContext* pContext ) {}
public:
	virtual DWORD Release( void );
};

#endif