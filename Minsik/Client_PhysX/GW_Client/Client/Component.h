#pragma once

#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include "Base.h"
class CComponent :
	public CBase
{
protected:
	explicit CComponent() = default;
	explicit CComponent( const CComponent& Instance ) = default;
	virtual ~CComponent() = default;

public:
	virtual void Update( void ) {}
public:
	virtual DWORD Release( void );
};

#endif