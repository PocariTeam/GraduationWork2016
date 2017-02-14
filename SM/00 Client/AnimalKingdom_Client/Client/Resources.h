#pragma once

#ifndef Resources_h__
#define Resources_h__

#include "Base.h"

class CResources
	: public CBase
{
protected:
	explicit CResources();
	explicit CResources( const CResources& Instance );
	virtual ~CResources();
public:
	virtual DWORD Release( void );
};

#endif // Resources_h__