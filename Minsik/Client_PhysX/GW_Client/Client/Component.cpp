#include "stdafx.h"
#include "Component.h"

DWORD CComponent::Release( void )
{
	DWORD dwRefCnt = CBase::Release();

	return dwRefCnt;
}
