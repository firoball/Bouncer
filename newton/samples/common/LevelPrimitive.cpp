//********************************************************************
// Newton Game dynamics 
// copyright 2000-2004
// By Julio Jerez
// VC: 6.0
// simple demo list vector class with iterators
//********************************************************************

// LevelPrimitive.cpp: implementation of the LevelPrimitive class.
//
//////////////////////////////////////////////////////////////////////
#include <stdafx.h>
#include "OpenGlUtil.h"
#include "LevelPrimitive.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//#define SERIALIZE_MESH

#ifdef SERIALIZE_MESH
void SerializeFile (void* serializeHandle, const void* buffer, size_t size)
{
	fwrite (buffer, size, 1, (FILE*) serializeHandle);
}

void DeSerializeFile (void* serializeHandle, void* buffer, size_t size)
{
	fread (buffer, size, 1, (FILE*) serializeHandle);
}

#endif


LevelPrimitive::LevelPrimitive(
	dSceneNode* parent,
    const char* name, 
    NewtonWorld* nWorld, 
	NewtonTreeCollisionCallback levelCallback)
	:dSceneNode(parent)
{
	NewtonCollision* collision;
	
	// open the level data
	char fullPathName[256];
	GetWorkingFileName (name, fullPathName);

	dLoaderContext context;
	LoadFromFile(fullPathName, context);
  
	// create the collision tree geometry
	collision = NewtonCreateTreeCollision(nWorld, levelCallback);
  
	// prepare to create collision geometry
	NewtonTreeCollisionBeginBuild(collision);

	// iterate the entire geometry an build the collision
	dMatrix mat(GetIdentityMatrix());
	ForEachNode (mat, (NodeCallback)&LevelPrimitive::BuildCollision, collision);

	// finalize the collision tree build
	NewtonTreeCollisionEndBuild(collision, 1);
//	NewtonTreeCollisionEndBuild(collision, 0);

#ifdef SERIALIZE_MESH
	FILE* file;
	strtok (fullPathName, ".");
	strcat (fullPathName, ".bin");

	// save the collision file
	file = fopen (fullPathName, "wb");
	NewtonTreeCollisionSerialize (collision, SerializeFile, file);
	fclose (file);

	// load the collision file
	NewtonReleaseCollision (nWorld, collision);

	file = fopen (fullPathName, "rb");
	collision = NewtonCreateTreeCollisionFromSerialization (nWorld, levelCallback, DeSerializeFile, file);
	fclose (file);

#endif

	// create the level rigid body
	m_level = NewtonCreateBody(nWorld, collision);

	// release the collision tree (this way the application does not have to do book keeping of Newton objects
	NewtonReleaseCollision (nWorld, collision);

	// set the global position of this body
	NewtonBodySetMatrix (m_level, &m_matrix[0][0]); 


	// save the pointer to the graphic object with the body.
	NewtonBodySetUserData (m_level, this);


	// set a destructor for this rigid body
	NewtonBodySetDestructorCallback (m_level, Destructor);

	
	dVector boxP0; 
	dVector boxP1; 
	// get the position of the aabb of this geometry
	NewtonCollisionCalculateAABB (collision, &m_matrix[0][0], &boxP0.m_x, &boxP1.m_x); 

	// add some extra padding the world size
	boxP0.m_x -= 50.0f;
	boxP0.m_y -= 50.0f;
	boxP0.m_z -= 50.0f;
	boxP1.m_x += 50.0f;
	boxP1.m_y += 50.0f;
	boxP1.m_z += 50.0f;

	// set the world size
	NewtonSetWorldSize (nWorld, &boxP0.m_x, &boxP1.m_x); 



#if 0
	int count;
	NewtonCollision* collisionB;
	collisionB = NewtonCreateBox (nWorld, 3.5f, 3.5f, 3.5f, NULL);

	dMatrix matrixA (GetIdentityMatrix());
	dMatrix matrixB (GetIdentityMatrix());
	matrixA.m_posit.m_y += 10.0f;
	matrixB.m_posit.m_z += 0.0f;

	dVector contacts[20];
	dVector normals[20];
	dFloat depths[20];
	count = NewtonCollisionCollide (nWorld, 10, 
									collision, &matrixA[0][0], 
									collisionB, &matrixB[0][0],
									&contacts[0][0], &normals[0][0], &depths[0]);
#endif
}

LevelPrimitive::~LevelPrimitive()
{
}

// rigid body destructor
void LevelPrimitive::Destructor (const NewtonBody* body)
{
	LevelPrimitive* level;

	// get the graphic object form the rigid body
	level = (LevelPrimitive*) NewtonBodyGetUserData (body);

	// destroy the graphic object
	delete level;
}


void LevelPrimitive::BuildCollision (const dMatrix& globalMatrix,  NewtonCollision* collision) const
{
	dInt32 i;
	dInt32 index;
	dList<dGeometrySegment>::dListNode *nodes;

	if (m_geometry) {
		for (nodes = m_geometry->GetFirst(); nodes; nodes = nodes->GetNext()) {
			dGeometrySegment& segment = nodes->GetInfo();
			for (i = 0; i < segment.m_indexCount; i += 3) {
				dVector face[3];

				index = segment.m_indexes[i + 0] * 3;
				face[0] = dVector (segment.m_vertex[index + 0], segment.m_vertex[index + 1], segment.m_vertex[index + 2]);

				index = segment.m_indexes[i + 1] * 3;
				face[1] = dVector (segment.m_vertex[index + 0], segment.m_vertex[index + 1], segment.m_vertex[index + 2]);

				index = segment.m_indexes[i + 2] * 3;
				face[2] = dVector (segment.m_vertex[index + 0], segment.m_vertex[index + 1], segment.m_vertex[index + 2]);

				globalMatrix.TransformTriplex (face, sizeof (dVector), face, sizeof (dVector), 3);
				NewtonTreeCollisionAddFace(collision, 3, &face[0].m_x, sizeof (dVector), 1);
			}
		}
	}
}

