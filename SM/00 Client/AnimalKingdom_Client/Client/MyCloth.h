// ===============================================================================
//						  NVIDIA PhysX SDK Sample ProgramS
//					        TEST CLOTH IMPLEMENTATION
//
//		            Written by Matthias M?ler-Fischer 03-08-05
// ===============================================================================

#ifndef MYCLOTH_H
#define MYCLOTH_H

#include "NxPhysics.h"
#include "Struct.h"

#include "VertexWelder.h"
class CMesh;
class MyCloth {
public:
	/* create mesh from obj file */
	MyCloth(NxScene *scene, ID3D11Device* pDevice, NxClothDesc &desc, const char *objFileName, NxReal scale, NxVec3* offset = NULL, const char *texFilename = NULL);
	/* create regular mesh */
	MyCloth(NxScene *scene, ID3D11Device* pDevice, NxClothDesc &desc, NxReal w, NxReal h, NxReal d, const char *texFilename = NULL, bool tearLines = false);

	~MyCloth();

	void draw(bool shadows);
	NxCloth *getNxCloth() { return mCloth; }

private:
	void init(NxScene *scene, NxClothDesc &desc, NxClothMeshDesc &meshDesc);
	bool generateObjMeshDesc(NxClothMeshDesc &desc, const char *filename, NxReal scale, NxVec3* offset, bool textured);
	void generateRegularMeshDesc(NxClothMeshDesc &desc, NxReal w, NxReal h, NxReal d, bool texCoords, bool tearLines = false);
	void generateTearLines(NxClothMeshDesc& desc, NxU32 w, NxU32 h);
	void releaseMeshDescBuffers(const NxClothMeshDesc& desc);

	bool cookMesh(NxClothMeshDesc& desc);

	void allocateReceiveBuffers(int numVertices, int numTriangles);
	void releaseReceiveBuffers();

	bool createTexture(const char *filename);
	void updateTextureCoordinates();

	bool mInitDone;

	NxMeshData mReceiveBuffers;
	NxScene *mScene;
	NxCloth *mCloth;
	NxClothMesh *mClothMesh;

	unsigned int mTexId;

	// The rendering buffers
	ID3D11Device*	m_pDevice;
	VERTEX_PNT* mVertexRenderBuffer;
	DWORD*		mIndexRenderBuffer;
	CMesh*		m_pMesh;

	NxU32 mMaxVertices;
	NxU32 mMaxIndices;
	NxU32 mNumIndices;
	NxU32 mNumParentIndices;
	NxU32 mNumVertices;
	NxU32 mLastNumVertices;

	NxU32 mMeshDirtyFlags;
	bool mTeared;

	VertexWelder* mVertexWelder;

	// Temporary storage for tex coords
	float *mTempTexCoords;
	NxU32   mNumTempTexCoords;

};

#endif
