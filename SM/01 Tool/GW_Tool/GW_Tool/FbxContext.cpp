#include "stdafx.h"
#include "FbxContext.h"
#include "..\ThirdParty\include\fbxsdk\scene\geometry\fbxlayer.h"
#include <fbxsdk\scene\geometry\fbxnodeattribute.h>

CFbxContext::CFbxContext()
	: m_pManager( nullptr )
	, m_pScene( nullptr )
	, m_pImporter( nullptr )
	, m_pRootNode( nullptr )
	, m_bIncludeAnimation( true )
	, m_llAnimationLength( 0 )
	, m_Cache_Start( FBXSDK_TIME_INFINITE )
	, m_Cache_Stop( FBXSDK_TIME_MINUS_INFINITE )
{
	ZeroMemory( m_pFileName, sizeof( char ) * MAX_PATH );
	ZeroMemory( m_pMeshName, sizeof( char ) * MAX_PATH );
}

CFbxContext::~CFbxContext()
{
	// Unload the cache and free the memory
	if( m_pScene )
	{
		UnloadCacheRecursive( m_pScene );
	}

	// Delete the FBX SDK manager. All the objects that have been allocated 
	// using the FBX SDK manager and that haven't been explicitly destroyed 
	// are automatically destroyed at the same time.
	DestroySdkObjects( m_pManager, true );

	ReleaseExcludeJoint();
}

HRESULT CFbxContext::Initialize( void )
{
	m_pManager = FbxManager::Create();
	if( !m_pManager )	return E_FAIL;

	FbxIOSettings* fbxIOSettings = FbxIOSettings::Create( m_pManager, IOSROOT );
	m_pManager->SetIOSettings( fbxIOSettings );

	m_pScene = FbxScene::Create( m_pManager, "myScene" );

	return S_OK;
}

HRESULT CFbxContext::Load_Scene( const char* pFileName )
{
	if( nullptr == pFileName )
	{
		AfxMessageBox( L"Load Scene Failed" );
		return E_FAIL;
	}

	strcpy_s( m_pFileName, MAX_PATH, pFileName );

	m_pImporter = FbxImporter::Create( m_pManager, "myImporter" );

	if( !m_pImporter )
	{
		AfxMessageBox( L"Load Scene Failed" );
		return E_FAIL;
	}

	if( !m_pImporter->Initialize( pFileName, -1, m_pManager->GetIOSettings() ) )
	{
		AfxMessageBox( L"Load Scene Failed" );
		return E_FAIL;
	}

	if( !m_pImporter->Import( m_pScene ) )
	{
		AfxMessageBox( L"Load Scene Failed" );
		return E_FAIL;
	}

	m_pImporter->Destroy();

	return S_OK;
}

HRESULT CFbxContext::Recursive_Hierarchy()
{
	m_pRootNode = m_pScene->GetRootNode();

	ProcessSkeletonHierarchy( m_pRootNode );
	if( m_vecJoint.empty() ) m_bIncludeAnimation = false;
	ProcessGeometry( m_pRootNode );
	Optimize();

	return S_OK;
}

void CFbxContext::ReleaseExcludeJoint()
{
	m_pScene = nullptr;
	m_pManager = nullptr;
	m_pImporter = nullptr;
	m_pRootNode = nullptr;

	m_vecIndex.clear();
	m_vecIndex.swap( vector<int>() );

	m_vecVertex.clear();
	m_vecVertex.erase( m_vecVertex.begin(), m_vecVertex.end() );
	m_vecVertex.swap( vector< CTLPOINT >{} );
}

void CFbxContext::ProcessGeometry( FbxNode* pNode )
{
	if( pNode->GetNodeAttribute() )
	{
		if( FbxNodeAttribute::eMesh
			== pNode->GetNodeAttribute()->GetAttributeType() )
		{
			ProcessControlPoints( pNode );
			if( m_bIncludeAnimation )
				ProcessJointsAndAnimations( pNode );
			ProcessMesh( pNode );
		}
	}

	for( int i = 0; i < pNode->GetChildCount(); ++i )
		ProcessGeometry( pNode->GetChild( i ) );
}

void CFbxContext::ProcessJointsAndAnimations( FbxNode* pNode )
{
	FbxMesh* currMesh = pNode->GetMesh();
	DWORD dwDeformerCnt = ( DWORD )currMesh->GetDeformerCount();
	// This geometry transform is something I cannot understand
	// I think it is from MotionBuilder
	// If you are using Maya for your models, 99% this is just an
	// identity matrix
	// But I am taking it into account anyways......
	FbxAMatrix geometryTransform = Utilities::GetGeometryTransformation( pNode );

	geometryTransform.SetS( FbxVector4( 1.0, -1.0, 1.0 ) );

	// A deformer is a FBX thing, which contains some clusters
	// A cluster contains a link, which is basically a joint
	// Normally, there is only one deformer in a mesh
	for( DWORD i = 0; i < dwDeformerCnt; ++i )
	{
		// There are many types of deformers in Maya,
		// We are using only skins, so we see if this is a skin
		FbxSkin* currSkin = reinterpret_cast< FbxSkin* >( currMesh->GetDeformer( i, FbxDeformer::eSkin ) );
		if( !currSkin )	continue;

		DWORD dwClusterCnt = ( DWORD )currSkin->GetClusterCount();
		for( DWORD j = 0; j < dwClusterCnt; ++j )
		{
			FbxCluster* currCluster = currSkin->GetCluster( j );
			string currJointName = currCluster->GetLink()->GetName();
			unsigned int currJointIndex = FindJointIndexUsingName( currJointName );
			FbxAMatrix transformMatrix;
			FbxAMatrix transformLinkMatrix;
			FbxAMatrix globalBindposeInverseMatrix;

			currCluster->GetTransformMatrix( transformMatrix );	// The transformation of the mesh at binding time
			currCluster->GetTransformLinkMatrix( transformLinkMatrix );	// The transformation of the cluster(joint) at binding time from joint space to world space

			globalBindposeInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix * geometryTransform;
			FbxVector4 vS = globalBindposeInverseMatrix.GetS();
			globalBindposeInverseMatrix.SetS( FbxVector4( vS.mData[ 0 ], -vS.mData[ 1 ], vS.mData[ 2 ], vS.mData[ 3 ] ) );

			// Update the information in mSkeleton 
			m_vecJoint[ currJointIndex ].mtxGlobalBindposeInverse = globalBindposeInverseMatrix;
			m_vecJoint[ currJointIndex ].pNode = currCluster->GetLink();

			// Associate each joint with the control points it affects
			DWORD dwIndexCnt = ( DWORD )currCluster->GetControlPointIndicesCount();
			for( DWORD k = 0; k < dwIndexCnt; ++k )
			{
				BLEND currBlendInfo;
				currBlendInfo.dwIndex = currJointIndex;
				currBlendInfo.dWeight = currCluster->GetControlPointWeights()[ k ];
				m_vecCtrlPoint[ currCluster->GetControlPointIndices()[ k ] ].vecBlendInfo.push_back( currBlendInfo );
			}

			// Get animation information
			// Now only supports one take
			FbxAnimStack* currAnimStack = m_pScene->GetSrcObject<FbxAnimStack>( 0 );
			FbxString animStackName = currAnimStack->GetName();
			m_strAnimationName = animStackName.Buffer();
			FbxTakeInfo* takeInfo = m_pScene->GetTakeInfo( animStackName );
			FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
			FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
			m_llAnimationLength = end.GetFrameCount( FbxTime::eDefaultMode ) - start.GetFrameCount( FbxTime::eDefaultMode ) + 1;
			Keyframe** currAnim = &m_vecJoint[ currJointIndex ].pAnimation;

			for( FbxLongLong i = start.GetFrameCount( FbxTime::eDefaultMode ); i <= end.GetFrameCount( FbxTime::eDefaultMode ); ++i )
			{
				FbxTime currTime;
				currTime.SetFrame( i, FbxTime::eDefaultMode );
				*currAnim = new Keyframe();
				( *currAnim )->m_iFrameNum = ( int )i;

				FbxAMatrix currentTransformOffset = pNode->EvaluateGlobalTransform( currTime ) * geometryTransform;

				FbxVector4 vR = currentTransformOffset.GetR();

				currentTransformOffset.SetR( FbxVector4( vR.mData[ 0 ] + 90.0, vR.mData[ 1 ], vR.mData[ 2 ], vR.mData[ 3 ] ) );

				FbxAMatrix mtxTemp = currentTransformOffset.Inverse() * currCluster->GetLink()->EvaluateGlobalTransform( currTime );

				( *currAnim )->m_mtxGlobalTransform._11 = ( float )mtxTemp.Get( 0, 0 ); ( *currAnim )->m_mtxGlobalTransform._12 = ( float )mtxTemp.Get( 1, 0 );
				( *currAnim )->m_mtxGlobalTransform._13 = ( float )mtxTemp.Get( 2, 0 ); ( *currAnim )->m_mtxGlobalTransform._14 = ( float )mtxTemp.Get( 3, 0 );

				( *currAnim )->m_mtxGlobalTransform._21 = ( float )mtxTemp.Get( 0, 1 ); ( *currAnim )->m_mtxGlobalTransform._22 = ( float )mtxTemp.Get( 1, 1 );
				( *currAnim )->m_mtxGlobalTransform._23 = ( float )mtxTemp.Get( 2, 1 ); ( *currAnim )->m_mtxGlobalTransform._24 = ( float )mtxTemp.Get( 3, 1 );

				( *currAnim )->m_mtxGlobalTransform._31 = ( float )mtxTemp.Get( 0, 2 ); ( *currAnim )->m_mtxGlobalTransform._32 = ( float )mtxTemp.Get( 1, 2 );
				( *currAnim )->m_mtxGlobalTransform._33 = ( float )mtxTemp.Get( 2, 2 ); ( *currAnim )->m_mtxGlobalTransform._34 = ( float )mtxTemp.Get( 3, 2 );

				( *currAnim )->m_mtxGlobalTransform._41 = ( float )mtxTemp.Get( 0, 3 ); ( *currAnim )->m_mtxGlobalTransform._42 = ( float )mtxTemp.Get( 1, 3 );
				( *currAnim )->m_mtxGlobalTransform._43 = ( float )mtxTemp.Get( 2, 3 ); ( *currAnim )->m_mtxGlobalTransform._44 = ( float )mtxTemp.Get( 3, 3 );

				XMMATRIX mtxCompute = XMLoadFloat4x4( &( *currAnim )->m_mtxGlobalTransform );
				mtxCompute *= XMLoadFloat4x4( &m_vecJoint[ currJointIndex ].GetGlobalBindPoseInverse() );
				XMStoreFloat4x4( &( *currAnim )->m_mtxGlobalTransform, mtxCompute );

				currAnim = &( ( *currAnim )->m_pNext );
			}
		}
	}

	// Some of the control points only have less than 4 joints
	// affecting them.
	// For a normal renderer, there are usually 4 joints
	// I am adding more dummy joints if there isn't enough
	BLEND currBlendingIndexWeightPair;
	currBlendingIndexWeightPair.dwIndex = 0;
	currBlendingIndexWeightPair.dWeight = 0;

	for( size_t i = 0; i < m_vecCtrlPoint.size(); ++i )
	{
		for( size_t j = m_vecCtrlPoint[ i ].vecBlendInfo.size(); j < 4; ++j )
		{
			m_vecCtrlPoint[ i ].vecBlendInfo.push_back( currBlendingIndexWeightPair );
		}
	}
}

void CFbxContext::ProcessMesh( FbxNode* pNode )
{
	FbxMesh* currMesh = pNode->GetMesh();

	int iCtrlPointCnt = currMesh->GetControlPointsCount();
	int iPolygonCnt = currMesh->GetPolygonCount();
	int iVertexCnt{ 0 };

	for( int i = 0; i < iPolygonCnt; ++i )
	{
		for( int j = 0; j < currMesh->GetPolygonSize( i ); ++j )
		{
			int iCtrlPointIndex = currMesh->GetPolygonVertex( i, j );
			/* Normal & Tangent */
			ReadNormal( currMesh, iCtrlPointIndex, iVertexCnt++, m_vecCtrlPoint[ iCtrlPointIndex ].tVertex.vNormal, m_vecCtrlPoint[ iCtrlPointIndex ].tVertex.vTangent );
		}
	}

	XMVECTOR	vNormalizedNormal, vNormalizedTangent, vPos;

	for( int i = 0; i < iCtrlPointCnt; ++i )
	{
		vNormalizedNormal = XMLoadFloat3( &m_vecCtrlPoint[ i ].tVertex.vNormal );
		vNormalizedTangent = XMLoadFloat3( &m_vecCtrlPoint[ i ].tVertex.vTangent );
		vPos = XMLoadFloat3( &m_vecCtrlPoint[ i ].tVertex.vPos );

		XMStoreFloat3( &m_vecCtrlPoint[ i ].tVertex.vPos, vPos );
		XMStoreFloat3( &m_vecCtrlPoint[ i ].tVertex.vNormal, XMVector3Normalize( vNormalizedNormal ) );
		XMStoreFloat3( &m_vecCtrlPoint[ i ].tVertex.vTangent, XMVector3Normalize( vNormalizedTangent ) );
	}

	iVertexCnt = 0;
	for( int i = 0; i < iPolygonCnt; ++i )
	{
		for( int j = 0; j < 3; ++j )
		{
			int iCtrlPointIndex = currMesh->GetPolygonVertex( i, j );
			CTLPOINT vCurrCtrlPoint = m_vecCtrlPoint[ iCtrlPointIndex ];
			ReadUV( currMesh, iCtrlPointIndex, iVertexCnt, vCurrCtrlPoint.tVertex.vUV );

			vCurrCtrlPoint.sort();

			m_vecVertex.push_back( vCurrCtrlPoint );
		}
	}

	reverse( m_vecVertex.begin(), m_vecVertex.end() );

	m_vecCtrlPoint.clear();
}

void CFbxContext::Optimize()
{
	vector<CTLPOINT> vecOptimize;
	bool			 bExist{ false };

	size_t j = 0;
	for( size_t i = 0; i < m_vecVertex.size(); ++i )
	{
		for( ; j < vecOptimize.size(); ++j )
		{
			if( 0 == memcmp( &m_vecVertex[ i ].tVertex, &vecOptimize[ j ].tVertex, sizeof( VERTEX ) )
				&& false == m_vecVertex[ i ].vecBlendInfo.empty()
				&& 0 == memcmp( &m_vecVertex[ i ].vecBlendInfo[ 0 ], &vecOptimize[ j ].vecBlendInfo[ 0 ], sizeof( BLEND ) )
				&& 0 == memcmp( &m_vecVertex[ i ].vecBlendInfo[ 1 ], &vecOptimize[ j ].vecBlendInfo[ 1 ], sizeof( BLEND ) )
				&& 0 == memcmp( &m_vecVertex[ i ].vecBlendInfo[ 2 ], &vecOptimize[ j ].vecBlendInfo[ 2 ], sizeof( BLEND ) )
				&& 0 == memcmp( &m_vecVertex[ i ].vecBlendInfo[ 3 ], &vecOptimize[ j ].vecBlendInfo[ 3 ], sizeof( BLEND ) ) )
			{
				bExist = true;
				break;
			}
		}

		if( bExist )	m_vecIndex.push_back( ( int )j );

		else
		{
			m_vecIndex.push_back( ( int )j );
			vecOptimize.push_back( m_vecVertex[ i ] );
		}

		j = 0;
		bExist = false;
	}

	m_vecVertex.swap( vecOptimize );
}

void CFbxContext::Release( void )
{
	m_pScene = nullptr;
	m_pManager = nullptr;
	m_pImporter = nullptr;
	m_pRootNode = nullptr;

	m_vecCtrlPoint.erase( m_vecCtrlPoint.begin(), m_vecCtrlPoint.end() );
	m_vecCtrlPoint.swap( vector<CTLPOINT>() );

	m_vecIndex.erase( m_vecIndex.begin(), m_vecIndex.end() );
	m_vecIndex.swap( vector<int>() );

	DWORD dwJointCnt = ( DWORD )m_vecJoint.size();
	for( DWORD i = 0; i < dwJointCnt; ++i )
	{
		Keyframe* pAdvance = m_vecJoint[ i ].pAnimation;
		Keyframe* pKeyframe = m_vecJoint[ i ].pAnimation;

		while( nullptr != pAdvance )
		{
			pKeyframe = pAdvance;
			pAdvance = pKeyframe->m_pNext;

			delete pKeyframe;
			pKeyframe = nullptr;
		}
	}

	m_vecJoint.erase( m_vecJoint.begin(), m_vecJoint.end() );
	m_vecJoint.swap( vector<JOINT>() );

	m_vecVertex.clear();
	m_vecVertex.erase( m_vecVertex.begin(), m_vecVertex.end() );
	m_vecVertex.swap( vector< CTLPOINT >{} );
}

void CFbxContext::ReadNormal( FbxMesh* pMesh, int iCtrlPointIndex, int iVertexIndex, XMFLOAT3& vNormal, XMFLOAT3& vTangent )
{
	if( pMesh->GetElementNormalCount() < 1 )
	{
		throw std::exception( "Invalid Normal Number" );
	}

	FbxGeometryElementNormal* vertexNormal = pMesh->GetElementNormal( 0 );
	FbxGeometryElementTangent* vertexTangent = pMesh->GetElementTangent( 0 );
	switch( vertexNormal->GetMappingMode() )
	{
	case FbxGeometryElement::eByControlPoint:
		switch( vertexNormal->GetReferenceMode() )
		{
		case FbxGeometryElement::eDirect:
		{
			vNormal.x += static_cast< float >( vertexNormal->GetDirectArray().GetAt( iCtrlPointIndex ).mData[ 0 ] );
			vNormal.y += static_cast< float >( vertexNormal->GetDirectArray().GetAt( iCtrlPointIndex ).mData[ 1 ] );
			vNormal.z += static_cast< float >( vertexNormal->GetDirectArray().GetAt( iCtrlPointIndex ).mData[ 2 ] );

			vTangent.x += static_cast< float >( vertexTangent->GetDirectArray().GetAt( iCtrlPointIndex ).mData[ 0 ] );
			vTangent.y += static_cast< float >( vertexTangent->GetDirectArray().GetAt( iCtrlPointIndex ).mData[ 1 ] );
			vTangent.z += static_cast< float >( vertexTangent->GetDirectArray().GetAt( iCtrlPointIndex ).mData[ 2 ] );
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexNormal->GetIndexArray().GetAt( iCtrlPointIndex );
			vNormal.x += static_cast< float >( vertexNormal->GetDirectArray().GetAt( index ).mData[ 0 ] );
			vNormal.y += static_cast< float >( vertexNormal->GetDirectArray().GetAt( index ).mData[ 1 ] );
			vNormal.z += static_cast< float >( vertexNormal->GetDirectArray().GetAt( index ).mData[ 2 ] );

			vTangent.x += static_cast< float >( vertexTangent->GetDirectArray().GetAt( index ).mData[ 0 ] );
			vTangent.y += static_cast< float >( vertexTangent->GetDirectArray().GetAt( index ).mData[ 1 ] );
			vTangent.z += static_cast< float >( vertexTangent->GetDirectArray().GetAt( index ).mData[ 2 ] );
		}
		break;

		default:
			throw std::exception( "Invalid Reference" );
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch( vertexNormal->GetReferenceMode() )
		{
		case FbxGeometryElement::eDirect:
		{
			vNormal.x += static_cast< float >( vertexNormal->GetDirectArray().GetAt( iVertexIndex ).mData[ 0 ] );
			vNormal.y += static_cast< float >( vertexNormal->GetDirectArray().GetAt( iVertexIndex ).mData[ 1 ] );
			vNormal.z += static_cast< float >( vertexNormal->GetDirectArray().GetAt( iVertexIndex ).mData[ 2 ] );

			vTangent.x += static_cast< float >( vertexTangent->GetDirectArray().GetAt( iVertexIndex ).mData[ 0 ] );
			vTangent.y += static_cast< float >( vertexTangent->GetDirectArray().GetAt( iVertexIndex ).mData[ 1 ] );
			vTangent.z += static_cast< float >( vertexTangent->GetDirectArray().GetAt( iVertexIndex ).mData[ 2 ] );
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexNormal->GetIndexArray().GetAt( iVertexIndex );
			vNormal.x += static_cast< float >( vertexNormal->GetDirectArray().GetAt( index ).mData[ 0 ] );
			vNormal.y += static_cast< float >( vertexNormal->GetDirectArray().GetAt( index ).mData[ 1 ] );
			vNormal.z += static_cast< float >( vertexNormal->GetDirectArray().GetAt( index ).mData[ 2 ] );

			vTangent.x += static_cast< float >( vertexTangent->GetDirectArray().GetAt( index ).mData[ 0 ] );
			vTangent.y += static_cast< float >( vertexTangent->GetDirectArray().GetAt( index ).mData[ 1 ] );
			vTangent.z += static_cast< float >( vertexTangent->GetDirectArray().GetAt( index ).mData[ 2 ] );
		}
		break;
		}
		break;
	default:
		throw std::exception( "Invalid Reference" );
		break;
	}
}

void CFbxContext::ReadUV( FbxMesh* pMesh, int iCtrlPointIndex, int iVertexIndex, XMFLOAT2& vUV )
{
	if( pMesh->GetElementUVCount() < 1 )
	{
		throw std::exception( "Invalid Normal Number" );
	}

	FbxStringList lUVSetNameList;
	pMesh->GetUVSetNames( lUVSetNameList );

	const char* lUVSetName = lUVSetNameList.GetStringAt( 0 );

	FbxGeometryElementUV* vertexUV = pMesh->GetElementUV( lUVSetName );
	const int lIndexCount = vertexUV->GetIndexArray().GetCount();
	const int lUVCount = vertexUV->GetDirectArray().GetCount();

	if( iVertexIndex > lIndexCount ) return;

	switch( vertexUV->GetMappingMode() )
	{
	case FbxGeometryElement::eByControlPoint:
		switch( vertexUV->GetReferenceMode() )
		{
		case FbxGeometryElement::eDirect:
		{
			vUV.x = static_cast< float >( vertexUV->GetDirectArray().GetAt( iCtrlPointIndex ).mData[ 0 ] );
			vUV.y = static_cast< float >( 1.0 - vertexUV->GetDirectArray().GetAt( iCtrlPointIndex ).mData[ 1 ] );
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexUV->GetIndexArray().GetAt( iCtrlPointIndex );
			vUV.x = static_cast< float >( vertexUV->GetDirectArray().GetAt( index ).mData[ 0 ] );
			vUV.y = static_cast< float >( 1.0 - vertexUV->GetDirectArray().GetAt( index ).mData[ 1 ] );
		}
		break;

		default:
			throw std::exception( "Invalid Reference" );
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch( vertexUV->GetReferenceMode() )
		{
		case FbxGeometryElement::eDirect:
		{
			vUV.x = static_cast< float >( vertexUV->GetDirectArray().GetAt( iVertexIndex ).mData[ 0 ] );
			vUV.y = static_cast< float >( 1.0 - vertexUV->GetDirectArray().GetAt( iVertexIndex ).mData[ 1 ] );
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexUV->GetIndexArray().GetAt( iVertexIndex );
			FbxVector2 vUVTemp = vertexUV->GetDirectArray().GetAt( index );
			double dTestX = vUVTemp.mData[ 0 ];
			double dTestY = vUVTemp.mData[ 1 ];
			vUV.x = static_cast< float >( dTestX );
			vUV.y = static_cast< float >( 1.0 - dTestY );
		}
		break;
		}
		break;
	default:
		throw std::exception( "Invalid Reference" );
		break;
	}
}

DWORD CFbxContext::FindJointIndexUsingName( const string& strJointName )
{
	for( unsigned int i = 0; i < m_vecJoint.size(); ++i )
	{
		if( m_vecJoint[ i ].strName == strJointName )
			return i;
	}

	throw std::exception( "Skeleton information in FBX file is corrupted." );
}

void CFbxContext::ProcessControlPoints( FbxNode * pNode )
{
	strcpy_s( m_pMeshName, pNode->GetName() );
	FbxMesh* pMesh = pNode->GetMesh();
	int iCtrlPointCnt = pMesh->GetControlPointsCount();

	// 제어점의 수만큼 제어점 정보를 벡터 컨테이너에 삽입
	for( int i = 0; i < iCtrlPointCnt; ++i )
	{
		CTLPOINT vCtrlPoint;

		vCtrlPoint.tVertex.vPos.x = static_cast< float >( pMesh->GetControlPointAt( i ).mData[ 0 ] );

		if( m_bIncludeAnimation )
		{
			vCtrlPoint.tVertex.vPos.y = static_cast< float >( pMesh->GetControlPointAt( i ).mData[ 1 ] );
			vCtrlPoint.tVertex.vPos.z = static_cast< float >( pMesh->GetControlPointAt( i ).mData[ 2 ] );
		}

		else
		{
			vCtrlPoint.tVertex.vPos.y = static_cast< float >( pMesh->GetControlPointAt( i ).mData[ 2 ] );
			vCtrlPoint.tVertex.vPos.z = static_cast< float >( pMesh->GetControlPointAt( i ).mData[ 1 ] );
		}

		m_vecCtrlPoint.push_back( vCtrlPoint );
	}
}

// Unload the cache and release the memory fro this scene and release the textures in GPU
void CFbxContext::UnloadCacheRecursive( FbxScene * pScene )
{
	const int lTextureCount = pScene->GetTextureCount();
	for( int lTextureIndex = 0; lTextureIndex < lTextureCount; ++lTextureIndex )
	{
		FbxTexture * lTexture = pScene->GetTexture( lTextureIndex );
		FbxFileTexture * lFileTexture = FbxCast<FbxFileTexture>( lTexture );
	}

	UnloadCacheRecursive( pScene->GetRootNode() );
}

// Unload the cache and release the memory under this node recursively.
void CFbxContext::UnloadCacheRecursive( FbxNode * pNode )
{
	// Unload the material cache
	const int lMaterialCount = pNode->GetMaterialCount();
	for( int lMaterialIndex = 0; lMaterialIndex < lMaterialCount; ++lMaterialIndex )
	{
		FbxSurfaceMaterial * lMaterial = pNode->GetMaterial( lMaterialIndex );
		if( lMaterial && lMaterial->GetUserDataPtr() )
			lMaterial->SetUserDataPtr( NULL );
	}

	FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();
	if( lNodeAttribute )
	{
		// Unload the mesh cache
		if( lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh )
		{
			FbxMesh * lMesh = pNode->GetMesh();
			if( lMesh && lMesh->GetUserDataPtr() )
				lMesh->SetUserDataPtr( NULL );
		}
		// Unload the light cache
		else if( lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eLight )
		{
			FbxLight * lLight = pNode->GetLight();
			if( lLight && lLight->GetUserDataPtr() )
				lLight->SetUserDataPtr( NULL );
		}
	}

	const int lChildCount = pNode->GetChildCount();
	for( int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex )
	{
		UnloadCacheRecursive( pNode->GetChild( lChildIndex ) );
	}
}

void CFbxContext::ProcessSkeletonHierarchy( FbxNode* pRootNode )
{
	for( int i = 0; i < pRootNode->GetChildCount(); ++i )
	{
		FbxNode* currNode = pRootNode->GetChild( i );
		ProcessSkeletonHierarchyRecursively( currNode, 0, -1 );
	}
}
void CFbxContext::ProcessSkeletonHierarchyRecursively( FbxNode* pNode, int iIndex, int iParentIndex )
{
	if( pNode->GetNodeAttribute() && pNode->GetNodeAttribute()->GetAttributeType()
		&& pNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton )
	{
		FbxNodeAttribute* pTest = pNode->GetNodeAttribute();
		JOINT currJoint( iParentIndex, pNode->GetName() );
		m_vecJoint.push_back( currJoint );
	}

	for( int i = 0; i < pNode->GetChildCount(); i++ )
		ProcessSkeletonHierarchyRecursively( pNode->GetChild( i ), ( signed )m_vecJoint.size(), iIndex );
}