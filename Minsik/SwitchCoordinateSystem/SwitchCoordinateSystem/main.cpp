#pragma comment( lib, "NxCharacter64.lib" )
#pragma comment( lib, "PhysXCooking64.lib" )
#pragma comment( lib, "PhysXCore64.lib" )
#pragma comment( lib, "PhysXLoader64.lib" )

#include <stdio.h>
#include "NxPhysics.h"
#include "NXU_helper.h"
#include "Utilities.h"
#include "NxCooking.h"
#include "Stream.h"

NxPhysicsSDK*	gPhysicsSDK;
NxScene*		gScene;

bool Initialize()
{
	// Initialize PhysicsSDK
	NxPhysicsSDKDesc sdkDesc;
	sdkDesc.flags ^= NX_SDKF_NO_HARDWARE;
	NxSDKCreateError errorCode = NXCE_NO_ERROR;
	gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, NULL, NULL, sdkDesc, &errorCode);
	if (gPhysicsSDK == NULL)
	{
		printf("\nSDK create error (%d - %s).\nUnable to initialize the PhysX SDK, exiting the program.\n\n", errorCode, getNxSDKCreateError(errorCode));
		return false;
	}

	return true;
}

void SwitchCoordinateSystem(NxScene* scene, NxMat34 mat)
{
	NxU32 nbActors = scene->getNbActors();
	NxActor** actors = scene->getActors();

	NxMat33 roll2;
	roll2.rotX(-NxPi*0.5f);

	NxMat34 invMat;
	mat.getInverse(invMat);

	bool bStaticActor;
	// Adjust actors' positions and orientations
	for (NxU32 i = 0; i < nbActors; i++)
	{
		NxActor* actor = actors[i];

		NxVec3 pos = actor->getGlobalPosition();
		pos = mat*pos;

		bStaticActor = false;
		if (!actor->isDynamic())
		{
			bStaticActor = true;
		}
		if (!bStaticActor)
			actor->setGlobalPosition(pos);

		// Adjust shape's positions and orientations within actors
		NxU32 nbShapes = actor->getNbShapes();
		NxShape*const* shape = actor->getShapes();

		NxActorDesc newActorDesc;

		if (bStaticActor)
		{
			actor->saveToDesc(newActorDesc);
		}
		for (NxU32 j = 0; j < nbShapes; j++)
		{
			// Transform the local position of each shape
			NxVec3 pos = shape[j]->getLocalPosition();
			pos = mat*pos;
			shape[j]->setLocalPosition(pos);
			if (NxPlaneShape* planeShape = shape[j]->isPlane())
			{
				NxPlaneShapeDesc planeDesc;
				planeShape->saveToDesc(planeDesc);
				NxReal y = planeDesc.normal.y;
				planeDesc.normal.y = planeDesc.normal.z;
				planeDesc.normal.z = y;

				if (bStaticActor)
				{
					newActorDesc.shapes.push_back(&planeDesc);
				}
				planeShape->setPlane(planeDesc.normal, planeDesc.d);
			}
			if (NxBoxShape* boxShape = shape[j]->isBox())
			{
				NxBoxShapeDesc boxDesc;
				boxShape->saveToDesc(boxDesc);
				NxReal y = boxDesc.dimensions.y;
				boxDesc.dimensions.y = boxDesc.dimensions.z;
				boxDesc.dimensions.z = y;

				boxShape->setDimensions(boxDesc.dimensions);

				if (bStaticActor)
				{
					newActorDesc.shapes.push_back(&boxDesc);
				}

			}
			if (NxCapsuleShape* capsuleShape = shape[j]->isCapsule())
			{
				NxMat33 mat = shape[j]->getGlobalOrientation();
				mat = roll2*mat;
				shape[j]->setGlobalOrientation(mat);
			}
			if (NxConvexShape* convexShape = shape[j]->isConvexMesh())
			{
				// TRANSFORM AND RE-COOK THE CONVEX MESH DATA
				NxConvexShapeDesc convexShapeDesc;
				convexShape->saveToDesc(convexShapeDesc);

				// Transform the mesh data to the new coordinate system
				NxConvexMeshDesc convexDesc;
				convexShape->getConvexMesh().saveToDesc(convexDesc);

				// TRANSFORM POINTS HERE
				NxVec3* verts = (NxVec3*)convexDesc.points;
				for (NxU32 i = 0; i < convexDesc.numVertices; i++)
				{
					NxVec3 pos = verts[i];
					pos = mat*pos;
					verts[i] = pos;
				}

				// Cook the new mesh data
				NxInitCooking();
				if (0)
				{
					// Cooking from file
#ifdef WIN32
					bool status = NxCookConvexMesh(convexDesc, UserStream("c:\\tmp.bin", false));
					convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(UserStream("c:\\tmp.bin", true));
#endif
				}
				else
				{
					// Cooking from memory
					MemoryWriteBuffer buf;
					bool status = NxCookConvexMesh(convexDesc, buf);
					convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(MemoryReadBuffer(buf.data));
				}

				if (bStaticActor)
				{
					newActorDesc.shapes.push_back(&convexShapeDesc);
				}
				else
				{
					actor->createShape(convexShapeDesc);

					NxActorDesc actorDesc;
					actor->saveToDesc(actorDesc);
					actorDesc.shapes.pushBack(&convexShapeDesc);

					shape[j]->userData = (void*)-1;  // Schedule for deletion
				}

			}
			if (NxTriangleMeshShape* meshShape = shape[j]->isTriangleMesh())
			{
				// TRANSFORM AND RE-COOK THE TRIANGLE MESH DATA
				NxTriangleMeshShapeDesc meshShapeDesc;
				meshShape->saveToDesc(meshShapeDesc);

				// Transform the mesh data to the new coordinate system
				NxTriangleMeshDesc meshDesc;
				meshShape->getTriangleMesh().saveToDesc(meshDesc);

				// TRANSFORM POINTS HERE
				NxVec3* verts = (NxVec3*)meshDesc.points;
				for (NxU32 i = 0; i < meshDesc.numVertices; i++)
				{
					NxVec3 pos = verts[i];
					pos = mat*pos;
					verts[i] = pos;
				}

				// Cook the new mesh data
				NxInitCooking();
				if (0)
				{
					// Cooking from file
#ifdef WIN32
					bool status = NxCookTriangleMesh(meshDesc, UserStream("c:\\tmp.bin", false));
					meshShapeDesc.meshData = gPhysicsSDK->createTriangleMesh(UserStream("c:\\tmp.bin", true));
#endif
				}
				else
				{
					// Cooking from memory
					MemoryWriteBuffer buf;
					bool status = NxCookTriangleMesh(meshDesc, buf);
					meshShapeDesc.meshData = gPhysicsSDK->createTriangleMesh(MemoryReadBuffer(buf.data));
				}

				if (bStaticActor)
				{
					newActorDesc.shapes.push_back(&meshShapeDesc);
				}
				else
				{
					actor->createShape(meshShapeDesc);

				}

				NxActorDesc actorDesc;
				actor->saveToDesc(actorDesc);
				actorDesc.shapes.pushBack(&meshShapeDesc);

				shape[j]->userData = (void*)-1;  // Schedule for deletion
			}
		}

		if (bStaticActor)
		{
			newActorDesc.globalPose.t = pos;
			scene->createActor(newActorDesc);
			scene->releaseActor(*actor);
		}


		//		actor->updateMassFromShapes(0,actor->getMass());
	}

	nbActors = scene->getNbActors();
	actors = scene->getActors();

	// Remove convex shapes scheduled for deletion
	// Update actor masses
	for (NxU32 i = 0; i < nbActors; i++)
	{
		NxActor* actor = actors[i];

		// Release convex shapes scheduled for deletion
		NxU32 nbShapes = actor->getNbShapes();
		NxShape*const* shape = actor->getShapes();
		for (NxU32 j = 0; j < nbShapes; j++)
		{
			if (shape[j]->userData == (void*)-1)
			{
				if (actor->isDynamic())
					actor->releaseShape(*shape[j]);
				shape[j]->userData = 0;
			}
		}

		if (actor->isDynamic())  actor->updateMassFromShapes(0, actor->getMass());
	}
}

bool LoadScene(const char *pFilename, NXU::NXU_FileType type)
{
	bool success = false;

	char scratch[512];
	switch (type) {
	case NXU::FT_XML:
		sprintf(scratch, "%s.xml", pFilename);
		break;
	case NXU::FT_BINARY:
		sprintf(scratch, "%s.nxb", pFilename);
		break;
	}

	if (gPhysicsSDK)
	{
		
		NXU::NxuPhysicsCollection *c = NXU::loadCollection(scratch, type);

		if (c)
		{
			if (gScene)
			{
				gPhysicsSDK->releaseScene(*gScene);
				gScene = nullptr;
			}

			success = NXU::instantiateCollection(c, *gPhysicsSDK, 0, 0, 0);
			gScene = gPhysicsSDK->getScene(0);
			
			NXU::releaseCollection(c);

			if (gScene)
			{

				NxMat34 mat;
				NxMat33 orient;
				orient.setColumn(0, NxVec3(1, 0, 0));
				orient.setColumn(1, NxVec3(0, 0, 1));
				orient.setColumn(2, NxVec3(0, 1, 0));
				mat.M = orient;

				SwitchCoordinateSystem(gScene, mat);

				NxU32 nbActors = gScene->getNbActors();
				NxActor **alist = gScene->getActors();

				for (NxU32 i = 0; i < nbActors; i++)
				{
					NxActor *a = alist[i];
					NxVec3 pos = a->getGlobalPosition();

					a->clearBodyFlag(NX_BF_VISUALIZATION);
					printf(" - 액터 '%s': (%f, %f, %f) \n", a->getName(), pos.x, pos.y, pos.z);

					NxU32 nbShapes = a->getNbShapes();
					if (nbShapes)
					{
						NxShape ** slist = (NxShape **)a->getShapes();
						for (NxU32 j = 0; j < nbShapes; j++)
						{
							NxShape *s = slist[j];
							s->setFlag(NX_SF_VISUALIZATION, false);
							NxVec3 spos = s->getLocalPosition();
							printf("   [%d] '%s'local:(%f, %f, %f) \n", j, s->getName(), spos.x, spos.y, spos.z);

						}
					}
				}
			}
			else {
				printf("m_pScene is NULL! \n");
				success = false;
			}

		}
		else
		{
			printf("NxuPhysicsCollection is NULL! \n");
		}
	}


	if (success)
		printf("Scene loaded from '%s'.\n", scratch);

	return success;
}

void SaveScene(const char *pFilename)
{

	// 중력과 filter값은 프로그램에서 바꾸어줘야 한다.
	gScene->setGravity(NxVec3(0, -9.81f, 0));
	gScene->setFilterBool(false);
	NXU::NxuPhysicsCollection *c = NXU::extractCollectionScene(gScene);

	if (c)
	{
		char scratch[512];
		sprintf(scratch, "%s_fix.xml", pFilename);
		printf("Saving NxuStream XML file to '%s'\r\n", scratch);
		NXU::saveCollection(c, scratch, NXU::FT_XML, false, false);

		//sprintf(scratch, "%s_fix.nxb", pFilename);
		//printf("Saving NxuStream BINARY file to '%s'\r\n", scratch);
		//NXU::saveCollection(c, scratch, NXU::FT_BINARY, true, true);

		//sprintf(scratch, "%s.dae", SaveFilename);
		//printf("Saving COLLADA Physics 1.4.1 file to '%s'\r\n", scratch);
		//NXU::saveCollection(c, scratch, NXU::FT_COLLADA, false, false);

		NXU::releaseCollection(c);

	}

	}

int main() {

	if (Initialize() != true)
	{
		return -1;
	}

	char fileName[256];
	int fileType;

	while (1)
	{

		printf("파일이름입력:");
		scanf("%s", fileName);
		printf("파일유형(1: XML, 2: BIN):");
		scanf("%d", &fileType);

		switch (fileType) {

		case 1:
			if (LoadScene(fileName, NXU::FT_XML)) {
				SaveScene(fileName);
			}
			break;
		case 2:
			if (LoadScene(fileName, NXU::FT_BINARY)) {
				SaveScene(fileName);
			}
			break;
		}

	}
}