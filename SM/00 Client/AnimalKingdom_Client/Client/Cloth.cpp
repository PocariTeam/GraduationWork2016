#include "stdafx.h"
#include "Cloth.h"
#include "Texture.h"
#include "Function.h"
#include "NxActor.h"
#include <NxScene.h>
#include "Stream.h"
#include <assert.h>
#include "Cooking.h"
#include "MeshMgr.h"
#include "Mesh.h"
#include "ClothMesh.h"
#include "Player.h"
#include "Animator.h"
#include "RenderState.h"
#include "Renderer.h"
#include "Jungle.h"

#define TEAR_MEMORY_FACTOR 1

CCloth::CCloth()
	: CGameObject()
	, m_pOwner( nullptr )
	, m_pPhysXScene( nullptr )
	, m_bInitialize( false )
	, m_pClothMesh( nullptr )
	, m_pCloth( nullptr )
	, m_pVertexArray( nullptr )
	, m_pIndexArray( nullptr )
	, m_dwVTXCnt( 0 )
	, m_dwIDXCnt( 0 )
{
}

CCloth::~CCloth()
{
}

HRESULT CCloth::Initialize( ID3D11Device* pDevice, NxScene* pScene, NxActor* pActor, /*CTexture* pTexture, */XMFLOAT3& vScale )
{
	m_pActor = pActor;
	m_pPhysXScene = pScene;
	m_vScale = vScale;
	m_vOption.x = 1.f;		// 카툰 한다
	m_vOption.w = 0.1f;

	//////////////////////////////////////////////////////////////////////////

	CreateCloth( pDevice );

	// 고정된 점에 천을 부착하기 ( 전역 ), attachVertexToShape 를 사용하면 셰이프에 부착 가능함, 셰이프에서 떼어낼 때는 디테치 함수를 호출해야함
	for( int i = 0; i < 28; ++i )
		m_pCloth->attachVertexToGlobalPosition( i, NxVec3( -0.7f + float( i ) / 28.f * 1.4f, 0.f, 0.f ) );

	return S_OK;
}

HRESULT CCloth::CreateCloth( ID3D11Device* pDevice )
{
	//// 옷감
	NxClothDesc tClothDesc;
	tClothDesc.globalPose.t = NxVec3( -5.f, -5.f, 0.f );
	tClothDesc.pressure = 0.1f;
	tClothDesc.thickness = 0.5f;					// 두께를 통해 견고성을 제어할 수 있음 ( 너무 낮으면 충돌 처리가 잘 되지 않음 )
	tClothDesc.density = 3.0f;						// 밀도를 통해 각 입자의 질량을 나타냄
	tClothDesc.bendingStiffness = 0.5f;				// 굽힘 : 1 에 가까우면 딱딱, 0 에 가까우면 종이처럼 쉽게 구부러짐
	tClothDesc.stretchingStiffness = 1.f;			// 늘어남 : 1 에 가까우면 데님, 0 에 가까우면 나일론과 같이 늘어남
	tClothDesc.dampingCoefficient = 0.3f;			// 감쇠에 대한 값을 제어, 각 입자의 속도에 영향
	tClothDesc.windAcceleration = NxVec3( 0.5f, 0.5f, 0.5f ); // 바람 가속 : 외부 가속은 각 입자들에 적용되지만 이는 꼭지점의 법선 방향에서만 적용
	tClothDesc.externalAcceleration = NxVec3( 0.f, 0.f, 0.f );	// 외부 가속 : 물체에 부착되지 않은 다른 입자들을 움직임
	tClothDesc.friction = 0.f;						// 마찰 계수 : 낮으면 표면을 따라 미끄러지는 느낌을 연출 가능, 실크와 같은 느낌
	//tClothDesc.collisionResponseCoefficient = 1.0f;// 충돌 시에 응답 계수, NX_CLF_DISABLE_COLLISION 일 때 적용 X, NX_CLF_COLLISION_TWOWAY 일 때 적용
	tClothDesc.attachmentResponseCoefficient = 0.f;// 천이 부착된 강체를 움직일 때 전달하는 운동량의 강도, 낮을수록 강체는 적은 영향을 받음
	tClothDesc.solverIterations = 5;				// 천의 시뮬레이션 정도를 제어, 높아질수록 많은 비용
	
	
	// tClothDesc.flags |= NX_CLF_STATIC;			// 시뮬레이션 비활성화
	tClothDesc.flags |= NX_CLF_DISABLE_COLLISION;	// 충돌 체크 사용 금지
	//tClothDesc.flags |= NX_CLF_VISUALIZATION;		// 디버그 렌더링 ( 의미 없음 )
	tClothDesc.flags |= NX_CLF_GRAVITY;				// 중력 적용
	tClothDesc.flags |= NX_CLF_BENDING;				// 구부러짐
	//tClothDesc.flags |= NX_CLF_BENDING_ORTHO;		// 정도가 적은 구부러짐
	//tClothDesc.flags |= NX_CLF_DAMPING;				// 천 입자에 대한 감쇠 적용
	//tClothDesc.flags |= NX_CLF_COMDAMPING;		// 질량 중심의 감쇠가 아닌( ?? )
	//tClothDesc.flags |= NX_CLF_COLLISION_TWOWAY;	// 천의 힘을 강체에 적용 ( ex : 천을 잡아당기면 연결된 강체가 따라옴 )
	//tClothDesc.flags |= NX_CLF_TEARABLE;			// 찢어짐
	//tClothDesc.flags |= NX_CLF_PRESSURE;			// 천 메쉬에 내부 압력 적용
	// tClothDesc.flags |= NX_CLF_SELFCOLLISION;	// 옷감 내에서 자기 충돌 처리
	tClothDesc.flags |= NX_CLF_VALIDBOUNDS;			// 천에 유효 범위 사용

	tClothDesc.validBounds.include(tClothDesc.globalPose.t);
	tClothDesc.validBounds.min -= NxVec3(30, 30, 30);
	tClothDesc.validBounds.max += NxVec3(30, 30, 30);

	if( NX_SIMULATION_HW == m_pPhysXScene->getSimType() )
		tClothDesc.flags |= NX_CLF_HARDWARE;

	//////////////////////////////////////////////////////////////////////////

	NxClothMeshDesc tMeshDesc;
	float fWidth{ 4.0f }, fHeight{ 6.0f }, fDepth{ 0.15f };

	DWORD dwNumX{ DWORD( fWidth / fDepth ) + 1 };
	DWORD dwNumY{ DWORD( fHeight / fDepth ) + 1 };

	tMeshDesc.numVertices = ( dwNumX + 1 ) * ( dwNumY + 1 );
	tMeshDesc.numTriangles = dwNumX * dwNumY * 2;
	tMeshDesc.pointStrideBytes = sizeof( XMFLOAT3 );
	tMeshDesc.triangleStrideBytes = 3 * sizeof( DWORD );
	tMeshDesc.vertexMassStrideBytes = sizeof( float );
	tMeshDesc.vertexFlagStrideBytes = sizeof( DWORD );
	tMeshDesc.points = new XMFLOAT3[ tMeshDesc.numVertices ];
	tMeshDesc.triangles = new DWORD[ tMeshDesc.numTriangles * 3 ];
	tMeshDesc.vertexMasses = 0;
	tMeshDesc.vertexFlags = 0;
	tMeshDesc.flags = 0;

	DWORD dwMaxVTXCnt{ TEAR_MEMORY_FACTOR * tMeshDesc.numVertices };
	DWORD dwMaxIDXCnt{ 3 * tMeshDesc.numTriangles };

	//////////////////////////////////////////////////////////////////////////
	// 버텍스 포지션 설정

	m_pVertexArray = new VERTEX_PN/*T*/[ dwMaxVTXCnt ]{};
	m_pIndexArray = new DWORD[ dwMaxIDXCnt ]{};
	DWORD i, j;
	XMFLOAT3* pPositionArr = ( XMFLOAT3* )tMeshDesc.points;
	for( i = 0; i <= dwNumY; i++ ) {
		for( j = 0; j <= dwNumX; j++ ) {
			*pPositionArr = m_pVertexArray[ i * ( dwNumX + 1 ) + j ].m_vPos = XMFLOAT3{ fDepth * j, fDepth * i, 0.f };
			pPositionArr++;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 텍스쳐 좌표 설정

	/*float fDeltaU = 0.f;
	if( dwNumX > 0 ) fDeltaU = 1.f / dwNumX;
	float fDeltaV = 0.f;
	if( dwNumY > 0 ) fDeltaV = 1.f / dwNumY;
	for( i = 0; i <= dwNumY; i++ ) {
		for( j = 0; j <= dwNumX; j++ ) {
			m_pVertexArray[ i * ( dwNumX + 1 ) + j ].m_vUV.x = j * fDeltaU;
			m_pVertexArray[ i * ( dwNumX + 1 ) + j ].m_vUV.y = i * fDeltaV;
		}
	}*/

	//////////////////////////////////////////////////////////////////////////
	// 인덱스 설정

	DWORD *pIndexArray = ( DWORD* )tMeshDesc.triangles;
	for( i = 0; i < dwNumY; i++ ) {
		for( j = 0; j < dwNumX; j++ ) {
			DWORD i0{ i * ( dwNumX + 1 ) + j };
			DWORD i1{ i0 + 1 };
			DWORD i2{ i0 + ( dwNumX + 1 ) };
			DWORD i3{ i2 + 1 };
			if( ( j + i ) % 2 ) {
				*pIndexArray++ = i0; *pIndexArray++ = i2; *pIndexArray++ = i1;
				*pIndexArray++ = i1; *pIndexArray++ = i2; *pIndexArray++ = i3;
			}
			else {
				*pIndexArray++ = i0; *pIndexArray++ = i2; *pIndexArray++ = i3;
				*pIndexArray++ = i0; *pIndexArray++ = i3; *pIndexArray++ = i1;
			}
		}
	}

	memcpy_s( m_pIndexArray, sizeof( DWORD ) * dwMaxIDXCnt, tMeshDesc.triangles, sizeof( DWORD ) * dwMaxIDXCnt );

	//////////////////////////////////////////////////////////////////////////
	// allocate flag buffer
	if( tMeshDesc.vertexFlags == 0 )
		tMeshDesc.vertexFlags = new NxU32[ tMeshDesc.numVertices ]{};

	// create tear lines
	/*NxU32* flags = ( NxU32* )tMeshDesc.vertexFlags;
	NxU32 y;
	for( y = 0; y < dwNumY + 1; y++ )
	{
		NxU32 x;
		for( x = 0; x < dwNumX + 1; x++ )
		{
			if( ( ( x + y ) % 16 == 0 ) || ( ( x - y + 16 ) % 16 == 0 ) )
				flags[ y * ( dwNumX + 1 ) + x ] = NX_CLOTH_VERTEX_TEARABLE;
			else
				flags[ y * ( dwNumX + 1 ) + x ] = 0;
		}
	}*/

	//////////////////////////////////////////////////////////////////////////
	// 피직스로부터 

	// if we want tearing we must tell the cooker
	// this way it will generate some space for particles that will be generated during tearing
	// 티어링 : 찢기는 효과

	//if( m_pPhysXScene->getSimType() == NX_SIMULATION_HW ) {
		// if we want tearing on the PPU we must tell the cooker
		// this way it will generate some space for particles that will be generated during tearing
		if( tClothDesc.flags & NX_CLF_TEARABLE )
			tMeshDesc.flags |= NX_CLOTH_MESH_TEARABLE;
	//}
	else {
		tClothDesc.flags &= ~NX_CLF_HARDWARE;
	}

	//////////////////////////////////////////////////////////////////////////
	// 쿸킹
	// Store correct number to detect tearing mesh in time
	// mLastNumVertices = tMeshDesc.numVertices;

	// we cook the mesh on the fly through a memory stream
	// we could also use a file stream and pre-cook the mesh
	MemoryWriteBuffer wb;
	assert( tMeshDesc.isValid() );
	bool bSuccess = CookClothMesh/*NxCookClothMesh*/( tMeshDesc, wb );

	if( !bSuccess )	return E_FAIL;

	MemoryReadBuffer rb( wb.data );
	m_pClothMesh = m_pPhysXScene->getPhysicsSDK().createClothMesh( rb );

	//////////////////////////////////////////////////////////////////////////
	ReleaseMeshDescBuffers( tMeshDesc );
	AllocateReceiveBuffers( pDevice, tMeshDesc.numVertices, tMeshDesc.numTriangles );

	tClothDesc.clothMesh = m_pClothMesh;
	tClothDesc.meshData = m_tReceiveBuffers;
	m_pCloth = m_pPhysXScene->createCloth( tClothDesc );
	m_bInitialize = true;

	return S_OK;
}

void CCloth::ReleaseMeshDescBuffers( const NxClothMeshDesc& rDesc )
{
	XMFLOAT3* pPositionArray = ( XMFLOAT3* )rDesc.points;
	DWORD*	  pIndexArray = ( DWORD* )rDesc.triangles;
	NxReal*	  pVertexMassArray = ( NxReal* )rDesc.vertexMasses;
	NxU32*	  pVertexFlagArray = ( NxU32* )rDesc.vertexFlags;
	
	::Safe_Delete_Array( pPositionArray );
	::Safe_Delete_Array( pIndexArray );
	::Safe_Delete_Array( pVertexMassArray );
	::Safe_Delete_Array( pVertexFlagArray );
}

void CCloth::AllocateReceiveBuffers( ID3D11Device* pDevice, DWORD dwVTXCnt, DWORD dwIDXCnt )
{
	// here we setup the buffers through which the SDK returns the dynamic cloth data
	// we reserve more memory for vertices than the initial mesh takes
	// because tearing creates new vertices
	// the SDK only tears cloth as long as there is room in these buffers
	
	DWORD	dwMaxVTXCnt{ TEAR_MEMORY_FACTOR * dwVTXCnt }, dwMaxIDXCnt{ 3 * dwIDXCnt };

	if( nullptr == m_pVertexArray )
		// Allocate Render Buffer for Vertices if it hasn't been done before
		m_pVertexArray = new VERTEX_PN/*T*/[ dwMaxVTXCnt ]{};

	if( nullptr == m_pIndexArray )
		// Allocate Render Buffer for Indices if it hasn't been done before
		m_pIndexArray = new DWORD[ dwMaxIDXCnt ]{};

	m_tReceiveBuffers.verticesPosBegin = new XMFLOAT3[ dwMaxVTXCnt ];
	m_tReceiveBuffers.verticesNormalBegin = new XMFLOAT3[ dwMaxVTXCnt ];
	m_tReceiveBuffers.verticesPosByteStride = sizeof( XMFLOAT3 );
	m_tReceiveBuffers.verticesNormalByteStride = sizeof( XMFLOAT3 );
	m_tReceiveBuffers.maxVertices = dwMaxVTXCnt;
	m_tReceiveBuffers.numVerticesPtr = new NxU32{};

	// the number of triangles is constant, even if the cloth is torn
	
	m_tReceiveBuffers.indicesBegin = new DWORD[ dwMaxIDXCnt ];
	m_tReceiveBuffers.indicesByteStride = sizeof( DWORD );
	m_tReceiveBuffers.maxIndices = dwMaxIDXCnt;
	m_tReceiveBuffers.numIndicesPtr = new NxU32{};

	// the parent index information would be needed if we used textured cloth
	m_tReceiveBuffers.parentIndicesBegin = new DWORD[ dwMaxIDXCnt ];
	m_tReceiveBuffers.parentIndicesByteStride = sizeof( DWORD );
	m_tReceiveBuffers.maxParentIndices = dwMaxIDXCnt;
	m_tReceiveBuffers.numParentIndicesPtr = new NxU32{};

	// m_tReceiveBuffers.dirtyBufferFlagsPtr = ( NxU32* )&dwMeshDirtyFlags;

	CMeshMgr::GetInstance()->Add( pDevice, "Mesh_Cloth", m_pVertexArray, m_dwVTXCnt = dwMaxVTXCnt, m_pIndexArray, m_dwIDXCnt = dwMaxIDXCnt );
	m_pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Cloth" );

	// init the buffers in case we want to draw the mesh 
	// before the SDK as filled in the correct values
	*m_tReceiveBuffers.numVerticesPtr = 0;
	*m_tReceiveBuffers.numIndicesPtr = 0;

	// dwMeshDirtyFlags = 0;
	// mNumVertices = 0;
	// mNumIndices = 0;
	// dwParentIndexCnt = 0;
}

CCloth* CCloth::Create( ID3D11Device* pDevice, NxScene* pScene, NxActor* pActor, /*CTexture* pTexture, */XMFLOAT3& vScale )
{
	CCloth* pCloth = new CCloth;

	if( FAILED( pCloth->Initialize( pDevice, pScene, pActor, /*pTexture, */vScale ) ) )
	{
		pCloth->Release();
		pCloth = nullptr;
	}
	return pCloth;
}

void CCloth::SetOwner( CPlayer* pPlayer )
{
	m_pOwner = pPlayer;
}

XMFLOAT4X4* CCloth::GetWorld()
{
	NxF32 mtxWorld[ 16 ]{};
	XMFLOAT4X4 Out{};

	if( nullptr != m_pOwner )
	{
		m_pActor->getGlobalPose().getRowMajor44( mtxWorld );
		memcpy( &Out, mtxWorld, sizeof( XMFLOAT4X4 ) );
		XMMATRIX mtxScale = XMMatrixScaling( m_vScale.x, m_vScale.y, m_vScale.z );
		mtxScale = XMMatrixMultiply( XMLoadFloat4x4( &Out ), mtxScale );
		XMStoreFloat4x4( &m_mtxWorld, mtxScale );
	}

	return &m_mtxWorld;
}

int CCloth::Update( const float& fTimeDelta )
{
	if( nullptr != m_pOwner )
	{
		NxMat34 mtxAnimation = m_pOwner->GetAnimator()->GetCurrentAnimationMatrix( m_pOwner, "Head" );
		NxMat33 mtxRotationX{};
		NxMat34 mtxRotationY{};
		NxVec3	vOffset{};
		float fRadian = m_pOwner->GetRotateY();
		NxVec3 vAcc{};

		switch( m_pOwner->GetCharacterType() )
		{
		case CHARACTER_CHM:
			mtxRotationX.rotX( -XM_PIDIV4 );
			vOffset = NxVec3( 0.f, 2.75f, 2.f );
			// vAcc = NxVec3( sin(fRadian), 0.f, -cos(fRadian));
			break;
		case CHARACTER_MON:
			mtxRotationX.rotX( XM_PIDIV4 + XM_PI );
			vOffset = NxVec3( 0.f, -0.2f, 0.08f );
			// vAcc = NxVec3( sin( fRadian ), 0.f, -cos( fRadian ) );
			break;
		case CHARACTER_BAT:
			mtxRotationX.rotX( -XM_PIDIV4 );
			vOffset = NxVec3( 0.f, 80.f, 30.f );
			// vAcc = NxVec3( -cos( fRadian ), 0.f, sin( fRadian ) );
			break;
		default:
			break;
		}

		NxMat34 mtxLocal{ mtxRotationX, vOffset };
		m_pActor->moveGlobalPose( mtxAnimation * mtxLocal );

		NxBounds3 bounds;
		m_pCloth->getWorldBounds( bounds );
		NxVec3 center;
		bounds.getCenter( center );
		NxReal radius = bounds.min.distance( bounds.max );
		NxReal range = 5.f;
		
		m_pCloth->setExternalAcceleration( NxVec3(NxMath::rand(-0.5f, 0.5f), NxMath::rand(0.f, 0.5f), NxMath::rand(0.f, 0.5f)) );
		m_pCloth->addForceAtPos(center, NxMath::rand(0.f, pow(radius, 3.f) * 5.f)/*pow(radius, 3.f) * 2.f*/, radius);
	}

	return 0;
}

void CCloth::Render( ID3D11DeviceContext* pContext )
{
	if( nullptr == m_pOwner ) return;
	if( CJungle::m_bFocusIncave != m_pOwner->InCave() )
		return;

	DWORD dwVTXCnt = *m_tReceiveBuffers.numVerticesPtr;
	DWORD dwIDXCnt = *m_tReceiveBuffers.numIndicesPtr / 3;

	/*NxU32 numVertices = *m_tReceiveBuffers.numVerticesPtr;
	NxU32 *parent = ( NxU32 * )m_tReceiveBuffers.parentIndicesBegin + dwVTXCnt;
	for( NxU32 i = dwVTXCnt; i < numVertices; i++ )
	{
		m_pVertexArray[ i ].m_vUV = m_pVertexArray[ ( *parent ) ].m_vUV;
	}*/

	XMFLOAT3* pPos = ( XMFLOAT3* )m_tReceiveBuffers.verticesPosBegin;
	XMFLOAT3* pNormal = ( XMFLOAT3* )m_tReceiveBuffers.verticesNormalBegin;
	DWORD*	  pIndex = ( DWORD* )m_tReceiveBuffers.indicesBegin;

	for( DWORD i = 0; i < m_dwVTXCnt; ++i )
	{
		m_pVertexArray[ i ].m_vPos = *pPos++;
		m_pVertexArray[ i ].m_vNormal = *pNormal++;
	}

	for( DWORD i = 0; i < m_dwIDXCnt; ++i )
		m_pIndexArray[ i ] = *pIndex++;

	bool bWireFrame = CRenderer::GetInstance()->m_bWireFrame;

	if( !bWireFrame )	CRenderState::Set_Rasterize( pContext, CRenderState::RS_NO_CULL );

	( ( CClothMesh* )m_pMesh )->UpdateGeometryInformation( pContext, m_pVertexArray, dwVTXCnt, m_pIndexArray, *m_tReceiveBuffers.numIndicesPtr );
	m_pMesh->Render( pContext );

	if( !bWireFrame )	CRenderState::Set_Rasterize( pContext, CRenderState::RS_NULL );
}

DWORD CCloth::Release( void )
{
	if( 0 == CGameObject::Release() )
	{
		if( m_pCloth )
		{
			m_pPhysXScene->releaseCloth( *m_pCloth );
			m_pCloth = nullptr;
		}

		if( m_pClothMesh )
		{
			m_pPhysXScene->getPhysicsSDK().releaseClothMesh( *m_pClothMesh );
			m_pClothMesh = nullptr;
		}

		::Safe_Delete_Array( m_tReceiveBuffers.verticesPosBegin );
		::Safe_Delete_Array( m_tReceiveBuffers.verticesNormalBegin );
		::Safe_Delete_Array( m_tReceiveBuffers.numVerticesPtr );
		::Safe_Delete_Array( m_tReceiveBuffers.indicesBegin );
		::Safe_Delete_Array( m_tReceiveBuffers.numIndicesPtr );
		::Safe_Delete_Array( m_tReceiveBuffers.parentIndicesBegin );
		::Safe_Delete_Array( m_tReceiveBuffers.numParentIndicesPtr );

		 m_tReceiveBuffers.setToDefault();

		// delete the rendering buffers
		::Safe_Delete_Array( m_pVertexArray );
		::Safe_Delete_Array( m_pIndexArray );

		CMeshMgr::GetInstance()->Pop( "Mesh_Cloth" );
		delete this;
	}

	return 0;
}