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
	/* ���� ���� ���� */
	void	ProcessSkeletonHierarchy( FbxNode* pNode );
	void	ProcessSkeletonHierarchyRecursively( FbxNode* pNode, int iIndex, int iParentIndex );
	/* ���� ���� ���� */
	void	ProcessGeometry( FbxNode* pNode );
	void	ProcessJointsAndAnimations( FbxNode* pNode );
	void	ProcessMesh( FbxNode* pNode );
	/* �ε��� ���� ���� */
	void	Optimize();
private:
	void	Release( void );
private:
	/* ���� �� */
	void UnloadCacheRecursive( FbxScene* pScene );
	void UnloadCacheRecursive( FbxNode * pNode );
private:
	void ReadNormal( FbxMesh* pMesh, int iCtrlPointIndex, int iVertexIndex, XMFLOAT3& vNormal, XMFLOAT3& vTangent );
	void ReadUV( FbxMesh* pMesh, int iCtrlPointIndex, int iVertexIndex, XMFLOAT2& vUV );
	DWORD FindJointIndexUsingName( const string& strJointName );
public:
	// �޽��� ��� ������ ��ġ�� ��´�.
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

	/*      �޽�       */
	// ���� ���� ������ ����
	vector<CTLPOINT> m_vecVertex;
	// ���� �ε��� ������ ����
	vector<int>		 m_vecIndex;
	// ���� ��Ʈ�� ����Ʈ ������ ����
	vector<CTLPOINT>	 m_vecCtrlPoint;

	/* �ִϸ��̼� */
	/* ���� ���� */
	vector<JOINT>	 m_vecJoint;
};

