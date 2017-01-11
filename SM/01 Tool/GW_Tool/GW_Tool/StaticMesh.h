#ifndef _STATICMESH_H_
#define _STATICMESH_H_

#include "Mesh.h"

class CStaticMesh
	: public CMesh
{
private:
	explicit CStaticMesh();
	~CStaticMesh() = default;
private:
	HRESULT CreateBuffer( ID3D11Device* pDevice, const vector<CTLPOINT>* pvecCtrlPoint, const vector<int>* pvecIndex );
public:
	static CMesh* Create( ID3D11Device* pDevice, const vector<CTLPOINT>* pvecCtrlPoint, const vector<int>* pvecIndex );
public:
	void  Release( void );
};

#endif