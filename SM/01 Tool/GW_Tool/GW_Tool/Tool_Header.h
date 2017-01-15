#pragma once

#pragma once

#ifndef _TOOL_HEADER_H_
#define _TOOL_HEADER_H_

const unsigned short	C_WINSIZE_WIDTH = 800;
const unsigned short	C_WINSIZE_HEIGHT = 600;

#define SLOT_WORLD		0x00
#define SLOT_CAMERA		0x01
#define SLOT_LIGHT		0x02
#define SLOT_TEXTURE	0x03
#define SLOT_ANIM		0x04

extern HWND		 g_hWnd;

#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <functional>

#include <d3d11.h>
#include <d3dx11.h>

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <fbxsdk\core\arch\fbxtypes.h>
#include <fbxsdk.h>

using namespace DirectX;
using namespace std;

// 정점 구조체
struct tagVertex_Default {
	XMFLOAT3 vPos{ 0.f, 0.f, 0.f };
	XMFLOAT3 vNormal{ 0.f, 0.f, 0.f };
	XMFLOAT3 vTangent{ 0.f, 0.f, 0.f };
	XMFLOAT2 vUV{ 0.f, 0.f };
};
using VERTEX = tagVertex_Default;


typedef struct BlendInfo
{
	DWORD	dwIndex;
	double  dWeight;

	BlendInfo() :
		dwIndex( 0 ),
		dWeight( 0 )
	{}

	bool operator < ( const BlendInfo& tDest ) { return !( dWeight <= tDest.dWeight ); }
}BLEND;

typedef struct tagCtlPoint {
	VERTEX  tVertex;
	vector<BLEND> vecBlendInfo;

	tagCtlPoint()
	{
		vecBlendInfo.reserve( 4 );
	}

	~tagCtlPoint()
	{
		vecBlendInfo.erase( vecBlendInfo.begin(), vecBlendInfo.end() );
		vecBlendInfo.swap( vector< BLEND >{} );
	}

	void sort()
	{
		std::sort( vecBlendInfo.begin(), vecBlendInfo.end() );
	}
}CTLPOINT;

typedef struct tagSkinVertex {
	XMFLOAT3 vPos{ 0.f, 0.f, 0.f };
	XMFLOAT3 vNormal{ 0.f, 0.f, 0.f };
	XMFLOAT3 vTangent{ 0.f, 0.f, 0.f };
	XMFLOAT2 vUV{ 0.f, 0.f };
	DWORD	dwIndex[4];
	XMFLOAT3 vWeight;
}VERTEX_SKINNED;

// 월드 상수버퍼
struct tagConstantBuffer_World {
	XMFLOAT4X4A m_mtxWorld;
};
using CB_WORLD = tagConstantBuffer_World;

// 카메라 상수버퍼
struct tagConstantBuffer_Camera {
	XMFLOAT4X4A m_mtxView;
	XMFLOAT4X4A m_mtxProj;
	XMFLOAT4A	m_vCameraPos;
};
using CB_CAMERA = tagConstantBuffer_Camera;

// 애니메이션 테스트용 - 안슬꺼
struct tagConstantBuffer_Test {
	XMFLOAT4X4  m_mtxJoint[3];
};
using CB_TEST = tagConstantBuffer_Test;


// 카메라 상수버퍼
struct tagConstantBuffer_Light {
	XMFLOAT4   vDir;
	XMFLOAT4   vDiffuse;
	XMFLOAT4   vAmbient;
	XMFLOAT4   vSpecular;
};
using CB_LIGHT = tagConstantBuffer_Light;

typedef struct tagLight
{
	typedef enum eLightType {
		LIGHT_DIRECTIONAL
		, LIGHT_POINT
		, LIGHT_SPOT
	}LIGHT_TYPE;

	float	   fType{ 0.f };
	XMFLOAT3   vPos{ 0.f, 0.f, 0.f };
	XMFLOAT4   vDiffuse{ 1.f, 1.f, 1.f, 1.f };
	XMFLOAT4   vAmbient{ 0.3f, 0.3f, 0.3f, 1.f };
	XMFLOAT4   vSpecular{ 1.f, 1.f, 1.f, 1.f };
	XMFLOAT3   vDir{ 0.3f, -0.3f, 0.3f };
	float	   fRange{ 10.f };
	XMFLOAT3   vAttenuation{ 0.1f, 0.001f, 0.0001f };
	float	   fFalloff{ 5.f };
	float	   fPhi{ cos( XMConvertToRadians( 7.0f ) ) };
	float	   fTheta{ cos( XMConvertToRadians( 5.0f ) ) };
	XMFLOAT2   vPadding{ 0.f, 0.f };
}LIGHT;

typedef struct tagMaterial {
	tagMaterial()
		: vDiffuse( 1.f, 1.f, 1.f, 1.f )
		, vAmbient( 0.3f, 0.3f, 0.3f, 0.3f )
		, vSpecular( 1.f, 1.f, 1.f, 4.f )
	{
	}
	tagMaterial( XMFLOAT4   vDiffuse )
		: vDiffuse( vDiffuse )
		, vAmbient( 0.3f, 0.3f, 0.3f, 0.3f )
		, vSpecular( 1.f, 1.f, 1.f, 4.f )
	{
	}
	XMFLOAT4   vDiffuse;
	XMFLOAT4   vAmbient;
	XMFLOAT4   vSpecular;	// w : power
}MATERIAL;

typedef struct Keyframe
{
	int				m_iFrameNum;
	XMFLOAT4X4		m_mtxGlobalTransform;
	Keyframe*		m_pNext;

	Keyframe() :
		m_pNext( nullptr )
	{}

	Keyframe( const Keyframe& t )
		: m_iFrameNum( t.m_iFrameNum )
		, m_mtxGlobalTransform( t.m_mtxGlobalTransform )
		, m_pNext( t.m_pNext )
	{}
}KEYFRAME;

typedef struct tagJoint {
	tagJoint()
		: iParentIndex( 0 )
		, strName( "" )
		, pAnimation( nullptr )
		, pNode( nullptr )
	{
		mtxGlobalBindposeInverse.SetIdentity();
	}

	tagJoint( const tagJoint& t )
		: iParentIndex( t.iParentIndex )
		, strName( t.strName )
		, pAnimation( t.pAnimation )
		, mtxGlobalBindposeInverse( t.mtxGlobalBindposeInverse )
		, pNode( t.pNode )
	{
	}

	tagJoint( int iParentIndex, const char* pName )
		: iParentIndex( iParentIndex )
		, strName( pName )
		, pAnimation( nullptr )
		, pNode( nullptr )
	{
		mtxGlobalBindposeInverse.SetIdentity();
	}

	XMFLOAT4X4 GetGlobalBindPoseInverse()
	{
		XMFLOAT4X4 mtxTemp;

		mtxTemp._11 = (float)mtxGlobalBindposeInverse.Get(0, 0); 		mtxTemp._12 = (float)mtxGlobalBindposeInverse.Get(1, 0);
		mtxTemp._13 = (float)mtxGlobalBindposeInverse.Get(2, 0); 		mtxTemp._14 = (float)mtxGlobalBindposeInverse.Get(3, 0);

		mtxTemp._21 = (float)mtxGlobalBindposeInverse.Get(0, 1); 		mtxTemp._22 = (float)mtxGlobalBindposeInverse.Get(1, 1);
		mtxTemp._23 = (float)mtxGlobalBindposeInverse.Get(2, 1); 		mtxTemp._24 = (float)mtxGlobalBindposeInverse.Get(3, 1);

		mtxTemp._31 = (float)mtxGlobalBindposeInverse.Get(0, 2); 		mtxTemp._32 = (float)mtxGlobalBindposeInverse.Get(1, 2);
		mtxTemp._33 = (float)mtxGlobalBindposeInverse.Get(2, 2); 		mtxTemp._34 = (float)mtxGlobalBindposeInverse.Get(3, 2);

		mtxTemp._41 = (float)mtxGlobalBindposeInverse.Get(0, 3); 		mtxTemp._42 = (float)mtxGlobalBindposeInverse.Get(1, 3);
		mtxTemp._43 = (float)mtxGlobalBindposeInverse.Get(2, 3); 		mtxTemp._44 = (float)mtxGlobalBindposeInverse.Get(3, 3);

		return mtxTemp;
	}

	int		   iParentIndex;
	string	   strName;
	FbxAMatrix mtxGlobalBindposeInverse;
	Keyframe*  pAnimation;
	FbxNode*   pNode;
}JOINT;

typedef struct tagBoneAnimation {
	float GetStartFrame()const
	{
		return ( float )vecKeyframes.front().m_iFrameNum;
	}

	float GetEndFrame()const
	{
		return ( float )vecKeyframes.back().m_iFrameNum;
	}

	void Interpolate( float t, XMFLOAT4X4& M )const
	{
		if( t <= ( float )vecKeyframes.front().m_iFrameNum )
		{
			/*XMVECTOR S = XMLoadFloat3( &Keyframes.front().Scale );
			XMVECTOR P = XMLoadFloat3( &Keyframes.front().Translation );
			XMVECTOR Q = XMLoadFloat4( &Keyframes.front().RotationQuat );

			XMVECTOR zero = XMVectorSet( 0.0f, 0.0f, 0.0f, 1.0f );
			XMStoreFloat4x4( &M, XMMatrixAffineTransformation( S, zero, Q, P ) );*/

			XMFLOAT4X4 mtxGlobalTransform;
			XMMATRIX   mtxTemp;
			memcpy( &mtxGlobalTransform, &vecKeyframes.front().m_mtxGlobalTransform, sizeof( FbxMatrix ) );
			mtxTemp = XMLoadFloat4x4( &mtxGlobalTransform );

			XMStoreFloat4x4( &M, XMMatrixTranspose( mtxTemp ) );
		}
		else if( t >= ( float )vecKeyframes.back().m_iFrameNum )
		{
			/*XMVECTOR S = XMLoadFloat3( &Keyframes.back().Scale );
			XMVECTOR P = XMLoadFloat3( &Keyframes.back().Translation );
			XMVECTOR Q = XMLoadFloat4( &Keyframes.back().RotationQuat );

			XMVECTOR zero = XMVectorSet( 0.0f, 0.0f, 0.0f, 1.0f );
			XMStoreFloat4x4( &M, XMMatrixAffineTransformation( S, zero, Q, P ) );*/
			XMFLOAT4X4 mtxGlobalTransform;
			XMMATRIX   mtxTemp;
			memcpy( &mtxGlobalTransform, &vecKeyframes.back().m_mtxGlobalTransform, sizeof( FbxMatrix ) );
			mtxTemp = XMLoadFloat4x4( &mtxGlobalTransform );

			XMStoreFloat4x4( &M, XMMatrixTranspose( mtxTemp ) );
		}
		else
		{
			for( UINT i = 0; i < ( UINT )vecKeyframes.size() - 1; ++i )
			{
				if( t >= vecKeyframes[i].m_iFrameNum && t <= vecKeyframes[i + 1].m_iFrameNum )
				{
					float lerpPercent = ( t - vecKeyframes[i].m_iFrameNum ) / ( vecKeyframes[i + 1].m_iFrameNum - vecKeyframes[i].m_iFrameNum );

					XMFLOAT4X4 mtxSrc, mtxDest;
					memcpy( &mtxSrc, &vecKeyframes[i].m_mtxGlobalTransform, sizeof( FbxMatrix ) );
					memcpy( &mtxDest, &vecKeyframes[i + 1].m_mtxGlobalTransform, sizeof( FbxMatrix ) );

					XMMATRIX mtxResult, mtxTemp, mtxTemp2;
					mtxTemp = XMLoadFloat4x4( &mtxSrc );
					mtxTemp2 = XMLoadFloat4x4( &mtxDest );
					
					mtxResult = ( 1.f - lerpPercent ) * mtxTemp + ( lerpPercent )* mtxTemp2;
					XMStoreFloat4x4( &M, mtxResult );

					break;
				}
			}
			/*for( UINT i = 0; i < Keyframes.size() - 1; ++i )
			{
				if( t >= Keyframes[i].TimePos && t <= Keyframes[i + 1].TimePos )
				{
					float lerpPercent = ( t - Keyframes[i].TimePos ) / ( Keyframes[i + 1].TimePos - Keyframes[i].TimePos );

					XMVECTOR s0 = XMLoadFloat3( &Keyframes[i].Scale );
					XMVECTOR s1 = XMLoadFloat3( &Keyframes[i + 1].Scale );

					XMVECTOR p0 = XMLoadFloat3( &Keyframes[i].Translation );
					XMVECTOR p1 = XMLoadFloat3( &Keyframes[i + 1].Translation );

					XMVECTOR q0 = XMLoadFloat4( &Keyframes[i].RotationQuat );
					XMVECTOR q1 = XMLoadFloat4( &Keyframes[i + 1].RotationQuat );

					XMVECTOR S = XMVectorLerp( s0, s1, lerpPercent );
					XMVECTOR P = XMVectorLerp( p0, p1, lerpPercent );
					XMVECTOR Q = XMQuaternionSlerp( q0, q1, lerpPercent );

					XMVECTOR zero = XMVectorSet( 0.0f, 0.0f, 0.0f, 1.0f );
					XMStoreFloat4x4( &M, XMMatrixAffineTransformation( S, zero, Q, P ) );

					break;
				}
			}*/
		}
	}

	std::vector<Keyframe> vecKeyframes;
}BONE_ANIMATION;

typedef struct tagAnimationClip
{
	float GetClipStartFrame()const
	{
		float t = ( float ) 0xffffffff;
		for( UINT i = 0; i < vecBoneAnimations.size(); ++i )
		{
			t = min( t, vecBoneAnimations[i].GetStartFrame() );
		}

		return t;
	}

	float GetClipEndFrame()const
	{
		float t = 0.0f;
		for( UINT i = 0; i < vecBoneAnimations.size(); ++i )
		{
			t = max( t, vecBoneAnimations[i].GetEndFrame() );
		}

		return t;
	}

	void Interpolate( float t, std::vector<XMFLOAT4X4>& boneTransforms )const
	{
		for( UINT i = 0; i < vecBoneAnimations.size(); ++i )
		{
			vecBoneAnimations[i].Interpolate( t, boneTransforms[i] );
		}
	}

	std::vector<BONE_ANIMATION> vecBoneAnimations;
}ANIMATION_CLIP;


class Utilities
{
public:

	static void Utilities::WriteMatrix( std::ostream& inStream, FbxMatrix& inMatrix, bool inIsRoot )
	{
		inStream << "<mat>" << static_cast< float >( inMatrix.Get( 0, 0 ) ) << "," << static_cast< float >( inMatrix.Get( 0, 1 ) ) << "," << static_cast< float >( inMatrix.Get( 0, 2 ) ) << "," << static_cast< float >( inMatrix.Get( 0, 3 ) ) << ","
			<< static_cast< float >( inMatrix.Get( 1, 0 ) ) << "," << static_cast< float >( inMatrix.Get( 1, 1 ) ) << "," << static_cast< float >( inMatrix.Get( 1, 2 ) ) << "," << static_cast< float >( inMatrix.Get( 1, 3 ) ) << ","
			<< static_cast< float >( inMatrix.Get( 2, 0 ) ) << "," << static_cast< float >( inMatrix.Get( 2, 1 ) ) << "," << static_cast< float >( inMatrix.Get( 2, 2 ) ) << "," << static_cast< float >( inMatrix.Get( 2, 3 ) ) << ","
			<< static_cast< float >( inMatrix.Get( 3, 0 ) ) << "," << static_cast< float >( inMatrix.Get( 3, 1 ) ) << "," << static_cast< float >( inMatrix.Get( 3, 2 ) ) << "," << static_cast< float >( inMatrix.Get( 3, 3 ) ) << "</mat>\n";
	}

	static FbxAMatrix Utilities::GetGeometryTransformation( FbxNode* inNode )
	{
		if( !inNode )
		{
			throw std::exception( "Null for mesh geometry" );
		}

		const FbxVector4 lT = inNode->GetGeometricTranslation( FbxNode::eSourcePivot );
		const FbxVector4 lR = inNode->GetGeometricRotation( FbxNode::eSourcePivot );
		const FbxVector4 lS = inNode->GetGeometricScaling( FbxNode::eSourcePivot );

		return FbxAMatrix( lT, lR, lS );
	}

	static std::string Utilities::GetFileName( const std::string& inInput )
	{
		std::string seperator( "\\" );
		unsigned int pos = ( unsigned int )inInput.find_last_of( seperator );
		if( pos != std::string::npos )
		{
			return inInput.substr( pos + 1 );
		}
		else
		{
			return inInput;
		}
	}

	static std::string Utilities::RemoveSuffix( const std::string& inInput )
	{
		std::string seperator( "." );
		unsigned int pos = ( unsigned int )inInput.find_last_of( seperator );
		if( pos != std::string::npos )
		{
			return inInput.substr( 0, pos );
		}
		else
		{
			return inInput;
		}
	}
};


#endif