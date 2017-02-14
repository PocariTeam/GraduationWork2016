#pragma once
#ifndef Struct_h__
#define Struct_h__

typedef struct tagViewport
{
	int	m_iLeft{ 0 };
	int	m_iTop{ 0 };
	int	m_iRight{ 800 };
	int	m_iBottom{ 600 };
}VIEWPORT;

typedef struct tagLight
{
	typedef enum eLightType {
		LIGHT_DIRECTIONAL
		, LIGHT_POINT
		, LIGHT_SPOT
	}LIGHT_TYPE;

	float	   fType;
	XMFLOAT3   vPos{ 0.f, 0.f, 0.f };
	XMFLOAT4   vDiffuse{ 1.f, 1.f, 1.f, 1.f };
	XMFLOAT4   vAmbient{ 0.3f, 0.3f, 0.3f, 1.f };
	XMFLOAT4   vSpecular{ 1.f, 1.f, 1.f, 1.f };
	XMFLOAT3   vDir{ 0.f, 0.f, 1.f };
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

typedef struct tagDirectional_Light {
	XMFLOAT4   vDiffuse;
	XMFLOAT4   vAmbient;
	XMFLOAT4   vSpecular;
	XMFLOAT3   vDir;
	float	   fTemp;
}VS_CB_DIRECTIONAL;

typedef struct Vertex
{
	XMFLOAT3 vPos;
	XMFLOAT3 vNormal{ 0.f, 0.f, 0.f };
	XMFLOAT2 vUV{ 0.f, 0.f };
}VERTEX;

typedef struct Vertex_T
{
	XMFLOAT3 vPos;
	XMFLOAT3 vNormal{ 0.f, 0.f, 0.f };
	XMFLOAT3 vTangent{ 0.f, 0.f, 0.f };
	XMFLOAT2 vUV{ 0.f, 0.f };
}VERTEX_TANGENT;

typedef struct Vertex_P
{
	XMFLOAT3 vPos;
	XMFLOAT3 vVelocity{ 0.f, 0.f, 0.f };
	XMFLOAT2 vSize{ 0.f, 0.f };
	float	 fAge;
	UINT	 iType;
}VERTEX_PARTICLE;

typedef struct
{
	XMFLOAT4X4A	m_mtxWorld;
	XMFLOAT4X4A	m_mtxWorldInv;
}VS_CB_WORLD_MATRIX;

typedef struct
{
	XMFLOAT4X4A  m_mtxView;
	XMFLOAT4X4A  m_mtxProj;
	XMFLOAT4A	 m_vCameraPos;
}VS_CB_CAMERA;

struct RENDERTHREADINFO {
	int		m_iThreadID;
	HANDLE	m_hThread;
	HANDLE	m_hBeginEvent;
	HANDLE	m_hEndEvent;
	ID3D11DeviceContext*	m_pDeferredContext;
	ID3D11CommandList*		m_pCommandList;
};

#endif // Struct_h__