#pragma once

#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "Component.h"

class CTransform
	: public CComponent
{
private:
	CTransform();
	virtual ~CTransform();
public:
	void	Update( void );
	DWORD	Release( void );
public:
	static	CTransform*	Create( void );

public:
	XMFLOAT3		m_vScale;
	XMFLOAT3		m_vPos;
	XMFLOAT3		m_vRotate;
	XMFLOAT3		m_vDir;
	XMFLOAT4X4A		m_mtxWorld;
};

#endif