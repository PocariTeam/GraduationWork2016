#pragma once
#include "Tool_Header.h"
#include "../Common/Common.h"

class CFbxContext
{
public:
	CFbxContext();
	~CFbxContext();

public:
	HRESULT Initialize( void );
	HRESULT Load_Scene( const char* pFileName );
	HRESULT	Recursive_Hierarchy();
	void	ReleaseExcludeJoint();
private:
	/* 관절 정보 추출 */
	void	ProcessSkeletonHierarchy( FbxNode* pNode );
	void	ProcessSkeletonHierarchyRecursively( FbxNode* pNode, int iIndex, int iParentIndex );
	/* 기하 정보 추출 */
	void	ProcessGeometry( FbxNode* pNode );
	void	ProcessJointsAndAnimations( FbxNode* pNode );
	void	ProcessMesh( FbxNode* pNode );
	/* 인덱스 버퍼 생성 */
	void	Optimize();
private:
	void	Release( void );
private:
	/* 지울 때 */
	void UnloadCacheRecursive( FbxScene* pScene );
	void UnloadCacheRecursive( FbxNode * pNode );
private:
	void ReadNormal( FbxMesh* pMesh, int iCtrlPointIndex, int iVertexIndex, XMFLOAT3& vNormal, XMFLOAT3& vTangent );
	void ReadUV( FbxMesh* pMesh, int iCtrlPointIndex, int iVertexIndex, XMFLOAT2& vUV );
	DWORD FindJointIndexUsingName( const string& strJointName );
public:
	// 메쉬의 모든 정점의 위치를 얻는다.
	void ProcessControlPoints( FbxNode * pNode );
public:
	char m_pFileName[ MAX_PATH ];
	char m_pMeshName[ MAX_PATH ];

	FbxManager* m_pManager;
	FbxScene*	m_pScene;
	FbxImporter* m_pImporter;
	FbxNode*	m_pRootNode;
	bool		m_bIncludeAnimation;
	string		m_strAnimationName;
	FbxLongLong m_llAnimationLength;

	FbxTime m_Cache_Start{ FBXSDK_TIME_INFINITE };
	FbxTime m_Cache_Stop{ FBXSDK_TIME_MINUS_INFINITE };

	/*      메쉬       */
	// 모델의 정점 정보를 저장
	vector<CTLPOINT> m_vecVertex;
	// 모델의 인덱스 정보를 저장
	vector<int>		 m_vecIndex;
	// 모델의 컨트롤 포인트 정보를 저장
	vector<CTLPOINT>	 m_vecCtrlPoint;

	/* 애니메이션 */
	/* 관절 정보 */
	vector<JOINT>	 m_vecJoint;
};

