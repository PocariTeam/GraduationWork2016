/*----------------------------------------------------------------------------*\
|
|						    NVIDIA PhysX Technology
|
|							    www.nvidia.com
|
\*----------------------------------------------------------------------------*/

//Test Cloth Implementation

#if defined WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX		//suppress windows' global min,max macros.
#include <windows.h>	//needed for gl.h
#endif

#include "stdafx.h"
#include <stdio.h>
#include "MyCloth.h"
#include "Stream.h"
#include "wavefront.h"
#include "BmpLoader.h"
#include "NxCooking.h"
#include "cooking.h"
#include "MeshMgr.h"
#include "ClothMesh.h"

#define TEAR_MEMORY_FACTOR 3

//---------------------------------------------------
MyCloth::MyCloth( NxScene *scene, ID3D11Device* pDevice, NxClothDesc &desc, NxReal w, NxReal h, NxReal d, const char *texFilename, bool tearLines )
	: mVertexRenderBuffer( NULL ), mScene( NULL ), mCloth( NULL ), mClothMesh( NULL ), m_pDevice( pDevice ), 
	mIndexRenderBuffer( NULL ), mTempTexCoords( NULL ), mNumTempTexCoords( 0 ), mTexId( 0 ), mTeared( false )
{
	mInitDone = false;
	NxClothMeshDesc meshDesc;
	generateRegularMeshDesc( meshDesc, w, h, d, texFilename != NULL, tearLines );
	init( scene, desc, meshDesc );
	if( texFilename )
		createTexture( texFilename );
}

// -----------------------------------------------------------------------
void MyCloth::init( NxScene *scene, NxClothDesc &desc, NxClothMeshDesc &meshDesc )
{
	mScene = scene;

	// if we want tearing we must tell the cooker
	// this way it will generate some space for particles that will be generated during tearing
	if( desc.flags & NX_CLF_TEARABLE )
		meshDesc.flags |= NX_CLOTH_MESH_TEARABLE;

	// Enable Debug rendering for this cloth
	desc.flags |= NX_CLF_VISUALIZATION;

	cookMesh( meshDesc );
	releaseMeshDescBuffers( meshDesc );

	allocateReceiveBuffers( meshDesc.numVertices, meshDesc.numTriangles );

	desc.clothMesh = mClothMesh;
	desc.meshData = mReceiveBuffers;
	mCloth = scene->createCloth( desc );
	mInitDone = true;
}

// -----------------------------------------------------------------------
MyCloth::~MyCloth()
{
	if( mInitDone ) {
		if( mCloth ) mScene->releaseCloth( *mCloth );
		if( mClothMesh ) mScene->getPhysicsSDK().releaseClothMesh( *mClothMesh );
		releaseReceiveBuffers();

		// temp texcoords are only used temporary and should be deallocated way earlier
		assert( mTempTexCoords == NULL );

		if( mTexId ) {
			//glDeleteTextures(1, &mTexId);
			mTexId = 0;
		}

		// delete the rendering buffers
		free( mVertexRenderBuffer );
		free( mIndexRenderBuffer );
	}

	if( m_pMesh ) {
		m_pMesh->Release();
		m_pMesh = nullptr;
	}
		
}

int MyCloth::Update( const float& fTimeDelta )
{
	return 0;
}

void MyCloth::Render( ID3D11DeviceContext* pContext )
{
	static NxU32 numVertices = mNumVertices;
	NxU32 numElements = mNumIndices;
	numVertices = mNumVertices;

	// Disable pressure if tearing occurs
	if( mTeared && ( mCloth->getFlags() & NX_CLF_PRESSURE ) )
	{
		// Disable Pressure
		mCloth->setFlags( mCloth->getFlags() & ~NX_CLF_PRESSURE );
		mCloth->setPressure( 0 );

		// Reduce tearing factor
		NxReal oldTearing = mCloth->getTearFactor();
		oldTearing = ( oldTearing - 1 ) / 3 + 1;
		mCloth->setTearFactor( oldTearing );

		// Reduce bending stiffness
		if( mCloth->getBendingStiffness() > 0.9f )
			mCloth->setBendingStiffness( 0.2f );

		// Apply explosion in the middle of the cloth
		NxBounds3 bounds;
		mCloth->getWorldBounds( bounds );
		NxVec3 center;
		bounds.getCenter( center );
		NxReal radius = bounds.min.distance( bounds.max );
		mCloth->addForceAtPos( center, 7 * NxMath::pow( radius, 3 ), radius, NX_IMPULSE );
		printf( "Pressure disabled\n" );
	}

	if( mTexId > 0 )
	{
		updateTextureCoordinates();
	}

//	( ( CClothMesh* )m_pMesh )->UpdateGeometryInformation( pContext, mVertexRenderBuffer, mIndexRenderBuffer );
	m_pMesh->Render( pContext );

	/*
	"성민이를 위한 설명~"

	버텍스 개수: numVertices 2640개, 인덱스 개수: numElements 15228개
	버텍스 배열: mVertexRenderBuffer (포지션, 노말, 텍스쳐좌표)

	인덱스 개수: mNumIndices
	인덱스 배열: mIndexRenderBuffer

	glDrawElements() 인덱스배열을 이용하여 임의의 원소들을 임의의 순서대로 사용하여 기본도형을 그린다.
	glDrawRangeElements() 인덱스배열의 특정 범위만 사용하여 기본도형을 그린다.

	*/


	/*
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glVertexPointer(3, GL_FLOAT, sizeof(RenderBufferVertexElement), numVertices, &(mVertexRenderBuffer[0].position.x));
	glNormalPointer(GL_FLOAT, sizeof(RenderBufferVertexElement), numVertices, &(mVertexRenderBuffer[0].normal.x));

	if (mTexId) {
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glTexCoordPointer(2, GL_FLOAT, sizeof(RenderBufferVertexElement), numVertices, &(mVertexRenderBuffer[0].texCoord[0]));

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mTexId);
	glColor4f(1.0f, 1.0f, 1.0f,1.0f);
	}

	#ifdef __CELLOS_LV2__
	glDrawRangeElements(GL_TRIANGLES, 0, numVertices-1, numElements, GL_UNSIGNED_INT, mIndexRenderBuffer);
	#else
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_INT, mIndexRenderBuffer);
	#endif

	if (mTexId) {
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);
	}

	if (shadows) {
	const static float ShadowMat[]={ 1,0,0,0, 0,0,0,0, 0,0,1,0, 0,0,0,1 };
	glPushMatrix();
	glMultMatrixf(ShadowMat);
	glDisable(GL_LIGHTING);
	glColor4f(0.05f, 0.1f, 0.15f,1.0f);

	#ifdef __CELLOS_LV2__
	glDrawRangeElements(GL_TRIANGLES, 0, numVertices-1, numElements, GL_UNSIGNED_INT, mIndexRenderBuffer);
	#else
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_INT, mIndexRenderBuffer);
	#endif

	glColor4f(1.0f, 1.0f, 1.0f,1.0f);
	glEnable(GL_LIGHTING);
	glPopMatrix();
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	*/
}

DWORD MyCloth::Release( void )
{
	return 0;
}

// -----------------------------------------------------------------------
void MyCloth::generateRegularMeshDesc( NxClothMeshDesc &desc, NxReal w, NxReal h, NxReal d, bool texCoords, bool tearLines )
{
	int numX = ( int )( w / d ) + 1;
	int numY = ( int )( h / d ) + 1;

	desc.numVertices = ( numX + 1 ) * ( numY + 1 );
	desc.numTriangles = numX*numY * 2;
	desc.pointStrideBytes = sizeof( NxVec3 );
	desc.triangleStrideBytes = 3 * sizeof( NxU32 );
	desc.vertexMassStrideBytes = sizeof( NxReal );
	desc.vertexFlagStrideBytes = sizeof( NxU32 );
	desc.points = ( NxVec3* )malloc( sizeof( NxVec3 )*desc.numVertices );
	desc.triangles = ( NxU32* )malloc( sizeof( NxU32 )*desc.numTriangles * 3 );
	desc.vertexMasses = 0;
	desc.vertexFlags = 0;
	desc.flags = 0;

	mMaxVertices = TEAR_MEMORY_FACTOR * desc.numVertices;
	mMaxIndices = 3 * desc.numTriangles;

	int i, j;
	NxVec3 *p = ( NxVec3* )desc.points;
	for( i = 0; i <= numY; i++ ) {
		for( j = 0; j <= numX; j++ ) {
			p->set( d*j, 0.0f, d*i );
			p++;
		}
	}

	if( texCoords ) {
		mTempTexCoords = ( float * )malloc( sizeof( float ) * 2 * TEAR_MEMORY_FACTOR*desc.numVertices );
		float *f = mTempTexCoords;
		float dx = 1.0f; if( numX > 0 ) dx /= numX;
		float dy = 1.0f; if( numY > 0 ) dy /= numY;
		for( i = 0; i <= numY; i++ ) {
			for( j = 0; j <= numX; j++ ) {
				*f++ = j*dx;
				*f++ = i*dy;
			}
		}
		mNumTempTexCoords = desc.numVertices;
	}
	else
	{
		mNumTempTexCoords = 0;
		mTempTexCoords = NULL;
	}

	NxU32 *id = ( NxU32* )desc.triangles;
	for( i = 0; i < numY; i++ ) {
		for( j = 0; j < numX; j++ ) {
			NxU32 i0 = i * ( numX + 1 ) + j;
			NxU32 i1 = i0 + 1;
			NxU32 i2 = i0 + ( numX + 1 );
			NxU32 i3 = i2 + 1;
			if( ( j + i ) % 2 ) {
				*id++ = i0; *id++ = i2; *id++ = i1;
				*id++ = i1; *id++ = i2; *id++ = i3;
			}
			else {
				*id++ = i0; *id++ = i2; *id++ = i3;
				*id++ = i0; *id++ = i3; *id++ = i1;
			}
		}
	}

	// generate tear lines if necessary
	if( tearLines )
		generateTearLines( desc, numX + 1, numY + 1 );
}

// -----------------------------------------------------------------------
void MyCloth::generateTearLines( NxClothMeshDesc& desc, NxU32 w, NxU32 h )
{
	// allocate flag buffer
	if( desc.vertexFlags == 0 )
		desc.vertexFlags = malloc( sizeof( NxU32 )*desc.numVertices );

	// create tear lines
	NxU32* flags = ( NxU32* )desc.vertexFlags;
	NxU32 y;
	for( y = 0; y < h; y++ )
	{
		NxU32 x;
		for( x = 0; x < w; x++ )
		{
			if( ( ( x + y ) % 16 == 0 ) || ( ( x - y + 16 ) % 16 == 0 ) )
				flags[ y * w + x ] = NX_CLOTH_VERTEX_TEARABLE;
			else
				flags[ y * w + x ] = 0;
		}
	}
}

// -----------------------------------------------------------------------
void MyCloth::releaseMeshDescBuffers( const NxClothMeshDesc& desc )
{
	NxVec3* p = ( NxVec3* )desc.points;
	NxU32* t = ( NxU32* )desc.triangles;
	NxReal* m = ( NxReal* )desc.vertexMasses;
	NxU32* f = ( NxU32* )desc.vertexFlags;
	free( p );
	free( t );
	free( m );
	free( f );
}

// -----------------------------------------------------------------------
bool MyCloth::cookMesh( NxClothMeshDesc& desc )
{
	// Store correct number to detect tearing mesh in time
	mLastNumVertices = desc.numVertices;

	// we cook the mesh on the fly through a memory stream
	// we could also use a file stream and pre-cook the mesh
	MemoryWriteBuffer wb;
	assert( desc.isValid() );
	bool success = CookClothMesh( desc, wb );

	if( !success )
		return false;

	MemoryReadBuffer rb( wb.data );
	mClothMesh = mScene->getPhysicsSDK().createClothMesh( rb );
	return true;
}

// -----------------------------------------------------------------------
void MyCloth::allocateReceiveBuffers( int numVertices, int numTriangles )
{
	// here we setup the buffers through which the SDK returns the dynamic cloth data
	// we reserve more memory for vertices than the initial mesh takes
	// because tearing creates new vertices
	// the SDK only tears cloth as long as there is room in these buffers

	//NxU32 maxVertices = TEAR_MEMORY_FACTOR * numVertices;
	//NxU32 maxIndices = 3*numTriangles;

	if( mVertexRenderBuffer == NULL )
	{
		// Allocate Render Buffer for Vertices if it hasn't been done before
		mVertexRenderBuffer = ( VERTEX_PNT* )malloc( sizeof( VERTEX_PNT ) * mMaxVertices );
		memset( mVertexRenderBuffer, 0, sizeof( VERTEX_PNT ) * mMaxVertices );
	}

	if( mIndexRenderBuffer == NULL )
	{
		// Allocate Render Buffer for Indices if it hasn't been done before
		mIndexRenderBuffer = ( DWORD* )malloc( sizeof( DWORD ) * mMaxIndices );
		memset( mIndexRenderBuffer, 0, sizeof( DWORD ) * mMaxIndices );
	}

	mReceiveBuffers.verticesPosBegin = &( mVertexRenderBuffer[ 0 ].m_vPos.x );
	mReceiveBuffers.verticesNormalBegin = &( mVertexRenderBuffer[ 0 ].m_vNormal.x );
	mReceiveBuffers.verticesPosByteStride = sizeof( XMFLOAT3 );
	mReceiveBuffers.verticesNormalByteStride = sizeof( XMFLOAT3 );
	mReceiveBuffers.maxVertices = mMaxVertices;
	mReceiveBuffers.numVerticesPtr = &mNumVertices;

	// the number of triangles is constant, even if the cloth is torn
	NxU32 maxIndices = 3 * numTriangles;
	mReceiveBuffers.indicesBegin = mIndexRenderBuffer;
	mReceiveBuffers.indicesByteStride = sizeof( NxU32 );
	mReceiveBuffers.maxIndices = maxIndices;
	mReceiveBuffers.numIndicesPtr = &mNumIndices;

	if( mNumTempTexCoords > 0 )
	{
		// Copy Tex Coords from temp buffers to graphics buffer
		assert( mNumTempTexCoords == numVertices );

		for( NxU32 i = 0; i < mNumTempTexCoords; i++ )
		{
			mVertexRenderBuffer[ i ].m_vUV.x = mTempTexCoords[ 2 * i + 0 ];
			mVertexRenderBuffer[ i ].m_vUV.y = mTempTexCoords[ 2 * i + 1 ];
		}

		// Get rid of temp buffer
		mNumTempTexCoords = 0;
		free( mTempTexCoords );
		mTempTexCoords = NULL;
	}

	// the parent index information would be needed if we used textured cloth
	mReceiveBuffers.parentIndicesBegin = ( NxU32* )malloc( sizeof( NxU32 )*mMaxVertices );
	mReceiveBuffers.parentIndicesByteStride = sizeof( NxU32 );
	mReceiveBuffers.maxParentIndices = mMaxVertices;
	mReceiveBuffers.numParentIndicesPtr = &mNumParentIndices;

	mReceiveBuffers.dirtyBufferFlagsPtr = &mMeshDirtyFlags;

	CMeshMgr::GetInstance()->Add( m_pDevice, "Mesh_Cloth", mVertexRenderBuffer, mMaxVertices, mIndexRenderBuffer, mMaxIndices );
	m_pMesh = CMeshMgr::GetInstance()->Clone( "Mesh_Cloth" );

	// init the buffers in case we want to draw the mesh 
	// before the SDK as filled in the correct values
	mMeshDirtyFlags = 0;
	mNumVertices = 0;
	mNumIndices = 0;
	mNumParentIndices = 0;
}

// -----------------------------------------------------------------------
void MyCloth::releaseReceiveBuffers()
{
	// Parent Indices is always allocated
	free( mReceiveBuffers.parentIndicesBegin );

	mReceiveBuffers.setToDefault();
}

// -----------------------------------------------------------------------
bool MyCloth::createTexture( const char *filename )
{
	/*BmpLoader bl;
	if (!bl.loadBmp(filename)) return false;

	glGenTextures(1, &mTexId);
	if (!mTexId) return false;
	glBindTexture(GL_TEXTURE_2D, mTexId);
#ifndef __PPCGEKKO__
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
#endif
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
		bl.mWidth, bl.mHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bl.mRGB);*/

	return true;
}

// -----------------------------------------------------------------------
void MyCloth::updateTextureCoordinates()
{
	NxU32 numVertices = *mReceiveBuffers.numVerticesPtr;
	NxU32 *parent = ( NxU32 * )mReceiveBuffers.parentIndicesBegin + mLastNumVertices;
	for( NxU32 i = mLastNumVertices; i < numVertices; i++, parent++ ) {
		mVertexRenderBuffer[ i ].m_vUV.x = mVertexRenderBuffer[ *parent ].m_vUV.x;
		mVertexRenderBuffer[ i ].m_vUV.y = mVertexRenderBuffer[ *parent ].m_vUV.y;
	}

	mTeared |= mLastNumVertices != numVertices;
	mLastNumVertices = numVertices;
}
