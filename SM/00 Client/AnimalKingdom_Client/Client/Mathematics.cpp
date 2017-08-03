#include "stdafx.h"
#include "Mathematics.h"
#include <NxMat34.h>

XMFLOAT4X4 CMathematics::ConvertToXMFloat4x4( NxMat34* pMtx )
{
	NxF32 fLocal[ 16 ]{};
	XMFLOAT4X4 mtxOut{};
	pMtx->getRowMajor44( fLocal );
	memcpy_s( &mtxOut, sizeof( XMFLOAT4X4 ), fLocal, sizeof( NxF32 ) * 16 );

	return mtxOut;
}

XMFLOAT4X4 CMathematics::ConvertToXMFloat4x4Transpose( NxMat34* pMtx )
{
	NxF32 fLocal[ 16 ]{};
	XMFLOAT4X4 mtxOut{};
	pMtx->getColumnMajor44( fLocal );
	memcpy_s( &mtxOut, sizeof( XMFLOAT4X4 ), fLocal, sizeof( NxF32 ) * 16 );

	return mtxOut;
}

XMMATRIX CMathematics::ConvertToXMMatrix( NxMat34* pMtx )
{
	NxF32 fLocal[ 16 ]{};
	XMFLOAT4X4 mtxTemp{};
	pMtx->getRowMajor44( fLocal );
	memcpy_s( &mtxTemp, sizeof( XMFLOAT4X4 ), fLocal, sizeof( NxF32 ) * 16 );
	
	return XMLoadFloat4x4( &mtxTemp );
}

XMMATRIX CMathematics::ConvertToXMMatrixTranspose( NxMat34* pMtx )
{
	NxF32 fLocal[ 16 ]{};
	XMFLOAT4X4 mtxTemp{};
	pMtx->getColumnMajor44( fLocal );
	memcpy_s( &mtxTemp, sizeof( XMFLOAT4X4 ), fLocal, sizeof( NxF32 ) * 16 );

	return XMLoadFloat4x4( &mtxTemp );
}

NxMat34 CMathematics::ConvertToNxMat34( XMFLOAT4X4& tMtx )
{
	NxF32 fLocal[ 16 ]{};
	NxMat34 Out{};
	memcpy_s( &fLocal, sizeof( NxF32 ) * 16, &tMtx, sizeof( XMFLOAT4X4 ) );
	Out.setRowMajor44( fLocal );

	return Out;
}

NxMat34 CMathematics::ConvertToNxMat34( XMMATRIX& In )
{
	NxF32 fLocal[ 16 ]{};
	XMFLOAT4X4 tMtx;
	XMStoreFloat4x4( &tMtx, In );
	NxMat34 Out{};
	memcpy_s( &fLocal, sizeof( NxF32 ) * 16, &tMtx, sizeof( XMFLOAT4X4 ) );
	Out.setRowMajor44( fLocal );

	return Out;
}

DirectX::XMFLOAT3 CMathematics::ConvertToXMFloat3( NxVec3& v3 )
{
	XMFLOAT3 vTemp;
	memcpy_s( &vTemp, sizeof( XMFLOAT3 ), &v3, sizeof( NxVec3 ) );

	return vTemp;
}

DirectX::XMFLOAT4X4 CMathematics::LerpXMFloat4x4( XMFLOAT4X4* pMtx1, XMFLOAT4X4* pMtx2, float f )
{
	XMFLOAT4X4	mtxOut{};
	XMFLOAT4	vTemp1{}, vTemp2{};
	XMVECTOR	vResult{}, v1{}, v2{};

	for( int i = 0; i < 4; ++i )
	{
		memcpy_s( &vTemp1, sizeof( XMFLOAT4 ), pMtx1->m[ i ], sizeof( XMFLOAT4 ) );
		memcpy_s( &vTemp2, sizeof( XMFLOAT4 ), pMtx2->m[ i ], sizeof( XMFLOAT4 ) );

		v1 = XMLoadFloat4( &vTemp1 );
		v2 = XMLoadFloat4( &vTemp2 );
		vResult = XMQuaternionSlerp( v1, v2, f );
		XMStoreFloat4( &vTemp1, vResult );
		memcpy_s( mtxOut.m[ i ], sizeof( XMFLOAT4 ), &vTemp1, sizeof( XMFLOAT4 ) );
	}

	return mtxOut;
}
