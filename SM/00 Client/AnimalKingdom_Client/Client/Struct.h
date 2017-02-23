#pragma once

#ifndef Struct_h__
#define Struct_h__

/*	                            Light	                                     */

typedef struct tagLight_Info 
{
	tagLight_Info()
		: m_fType( 0 )
		, m_vPos( 0.f, 0.f, 0.f )
		, m_vDiffuse( 1.f, 1.f, 1.f, 1.f )
		, m_vAmbient( 0.3f, 0.3f, 0.3f, 1.f )
		, m_vSpecular( 0.5f, 0.5f, 0.5f, 50.f )
		, m_vDir( 0.f, 0.f, 1.f )
		, m_fRange( 1000.f )
		, m_vAttenuation( 0.1f, 0.001f, 0.0001f )
		, m_fFalloff( 5.f )
		, m_fPhi( cos( XMConvertToRadians( 7.0f ) ) )
		, m_fTheta( cos( XMConvertToRadians( 5.0f ) ) )
		, m_vPadding( 0.f, 0.f )
	{ }

	float	   m_fType;
	XMFLOAT3   m_vPos;
	XMFLOAT4   m_vDiffuse;
	XMFLOAT4   m_vAmbient;
	XMFLOAT4   m_vSpecular;
	XMFLOAT3   m_vDir;
	float	   m_fRange;
	XMFLOAT3   m_vAttenuation;
	float	   m_fFalloff;
	float	   m_fPhi;
	float	   m_fTheta;
	XMFLOAT2   m_vPadding;
}LIGHT;

/*                             Vertex Type                                     */

typedef struct tagVertex_PNTT
{
	XMFLOAT3	m_vPos;
	XMFLOAT3	m_vNormal;
	XMFLOAT3	m_vTangent;
	XMFLOAT2	m_vUV;
}VERTEX_PNTT;

typedef struct tagVertex_PNTTB {
	XMFLOAT3	m_vPos;
	XMFLOAT3	m_vNormal;
	XMFLOAT3	m_vTangent;
	XMFLOAT2	m_vUV;
	int			m_vIndex[4];
	XMFLOAT4	m_vWeight;

	tagVertex_PNTTB()
	{
		ZeroMemory( m_vIndex, 4 * ( sizeof( int ) + sizeof( float ) ) );
	}
}VERTEX_PNTTB;

/*                           Constant Buffer                                   */

typedef struct tagConstantBuffer_Camera
{
	XMFLOAT4X4  m_mtxView;
	XMFLOAT4X4  m_mtxProj;
	XMFLOAT4	m_vCameraPos;
}CB_CAMERA;

/*                             Thread                                         */

typedef struct tagThreadInfo
{
	tagThreadInfo() : m_hThread( nullptr ), m_hBeginEvent( nullptr ), m_hEndEvent( nullptr ), m_dwType{ 0 } {}
	virtual ~tagThreadInfo()
	{
		CloseHandle( m_hThread );
		CloseHandle( m_hBeginEvent );
		CloseHandle( m_hEndEvent );
	}
	HANDLE	m_hThread;
	HANDLE	m_hBeginEvent;
	HANDLE	m_hEndEvent;
	DWORD	m_dwType;		// THREAD TYPE
}THREAD_INFO;

typedef struct tagRenderThread
	: public tagThreadInfo
{
	tagRenderThread() : m_pCommandList( nullptr ), m_pDeferredContext( nullptr ) {}
	ID3D11DeviceContext*	m_pDeferredContext;
	ID3D11CommandList*		m_pCommandList;
}THREAD_RENDERING;

typedef struct tagLoadThread
	: public tagThreadInfo
{
	tagLoadThread() : m_pDevice( nullptr ), m_strPath( "" ) {}
	~tagLoadThread() {}
	ID3D11Device*	m_pDevice;
	char			m_strPath[ MAX_PATH ];
}THREAD_LOADING;



#endif // Struct_h__