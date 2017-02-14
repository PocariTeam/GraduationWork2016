#include "stdafx.h"
#include "Resources.h"


CResources::CResources()
	: CBase()
{
}

CResources::CResources( const CResources& Instance )
	: CBase( Instance )
{
}

CResources::~CResources()
{
}

DWORD CResources::Release( void )
{
	return CBase::Release();
}
