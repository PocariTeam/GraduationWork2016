#ifndef _ANIMATEMESH_H_
#define _ANIMATEMESH_H_

#include "Mesh.h"

class CAnimateMesh
	: public CMesh
{
private:
	explicit CAnimateMesh();
	~CAnimateMesh() = default;
private:
	HRESULT CreateBuffer( ID3D11Device* pDevice, const vector<CTLPOINT>* pvecCtrlPoint, const vector<int>* pvecIndex );
public:
	static CMesh* Create( ID3D11Device* pDevice, const vector<CTLPOINT>* pvecCtrlPoint, const vector<int>* pvecIndex );
public:
	void  Release( void );
};

#endif