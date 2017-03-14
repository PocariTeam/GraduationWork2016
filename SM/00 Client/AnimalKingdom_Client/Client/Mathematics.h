#pragma once

#ifndef Mathematics_h__
#define Mathematics_h__

class NxMat34;
class CMathematics
{
public:
	static XMFLOAT4X4 ConvertToXMFloat4x4( NxMat34* pMtx );
	static XMFLOAT4X4 ConvertToXMFloat4x4Transpose( NxMat34* pMtx );

	static XMMATRIX	  ConvertToXMMatrix( NxMat34* pMtx );
	static XMMATRIX	  ConvertToXMMatrixTranspose( NxMat34* pMtx );

	static NxMat34	  ConvertToNxMat34( XMFLOAT4X4& tMtx );
	static NxMat34	  ConvertToNxMat34( XMMATRIX& In );
};

#endif // Mathematics_h__
