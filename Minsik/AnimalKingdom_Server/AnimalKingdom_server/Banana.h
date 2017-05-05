#pragma once

#ifndef Banana_h__
#define Banana_h__

class CBanana
{
private:
	explicit CBanana();
	virtual ~CBanana();
private:
	HRESULT		Initialize( NxActor* pActor, NxVec3& vDir );
public:
	static CBanana* Create( NxActor* pActor, NxVec3& vDir );
public:
	XMFLOAT4X4 GetWorld();
public:
	DWORD	Release( void );
private:
	NxActor*	m_pActor;
};

#endif // Banana_h__