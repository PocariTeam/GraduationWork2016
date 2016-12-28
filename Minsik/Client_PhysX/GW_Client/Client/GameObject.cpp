#include "stdafx.h"
#include "GameObject.h"
#include "Component.h"
#include "Function.h"
#include "Mesh.h"
#include "Texture.h"

int	CGameObject::Update( float fTimeDelta )
{
	map< const char*, CComponent*>::iterator	iter = m_mapComponent.begin();
	map< const char*, CComponent*>::iterator	iter_end = m_mapComponent.end();

	for( ; iter != iter_end; ++iter )
	{
		iter->second->Update();
	}

	return 0;
}

DWORD	CGameObject::Release()
{
	map< const char*, CComponent*>::iterator	iter = m_mapComponent.begin();
	map< const char*, CComponent*>::iterator	iter_end = m_mapComponent.end();

	for( ; iter != iter_end; ++iter )
		iter->second->Release();
	m_mapComponent.clear();

	::Safe_Release( m_pMeshCom );
	::Safe_Release( m_pTexture );
	::Safe_Release( m_pCBmtxWorld );
	::Safe_Release( m_pCBMaterial );

	return 0;
}