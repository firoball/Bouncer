#include "gdknewton.h"

//Callback functions - do not call directly!
void* NewtonAlloc_Callback(int sizeInBytes)
{
	return malloc (sizeInBytes);
}

void NewtonFree_Callback(void *ptr, int sizeInBytes)
{
	free (ptr);
}

void NewtonCleanUp_Callback()
{
	NewtonDestroy (env.nWorld);
}

void NewtonUpdateObject_Callback(const NewtonBody* body, const dFloat* matrix)
{
	int ctr = (int) NewtonBodyGetUserData (body); 
	dMatrix& mat = *((dMatrix*)matrix);

	dVector ang;
	NewtonGetEulerAngle(&mat[0][0],&ang.m_x);

	dbPositionObject (ctr, mat.m_posit.m_x, mat.m_posit.m_y, mat.m_posit.m_z);
	dbRotateObject (ctr, ang.m_x*180/3.1416, ang.m_y*180/3.1416, ang.m_z*180/3.1416);
}

void NewtonApplyGravity_Callback(const NewtonBody* body)
{
	dFloat mass;
	dFloat Ixx;
	dFloat Iyy;
	dFloat Izz;

	NewtonBodyGetMassMatrix (body, &mass, &Ixx, &Iyy, &Izz);
	dVector force (mass * env.rgravity.m_x, mass * env.rgravity.m_y, mass * env.rgravity.m_z);
	NewtonBodySetForce (body, &force.m_x);
}

//public functions
void NewtonInit()
{
	env.nWorld = NewtonCreate (NewtonAlloc_Callback, NewtonFree_Callback);
	atexit(NewtonCleanUp_Callback);
	dVector min (-800, -800, -800);
	dVector max (800, 800, 800);
	NewtonSetWorldSize(env.nWorld, &min.m_x, &max.m_x);
	env.gravity.m_x = 0.0f;
	env.gravity.m_y = GRAVITY_FAC * -9.81f;
	env.gravity.m_z = 0.0f;
	env.rgravity = env.gravity;

	//now speed up Newton
	NewtonSetMinimumFrameRate(env.nWorld, env.syncRate);
	NewtonSetFrictionModel(env.nWorld, 1);
	NewtonSetSolverModel(env.nWorld, 1);
}

void NewtonSetMass(int ctr, float mass)
{
	dFloat oldMass;
	dFloat Ixx;
	dFloat Iyy;
	dFloat Izz;

	NewtonBodyGetMassMatrix (env.body[ctr - 1], &oldMass, &Ixx, &Iyy, &Izz);
	NewtonBodySetMassMatrix (env.body[ctr - 1], mass, Ixx, Iyy, Izz);
}

void NewtonSetGravity(float fX, float fY, float fZ)
{
	dMatrix matrix (dgPitchMatrix(fX * 3.1416f / 180.0f));
	matrix = matrix * dgRollMatrix(fZ * 3.1416f / 180.0f);
	matrix = matrix * dgYawMatrix(fY * 3.1416f / 180.0f);
	env.rgravity = matrix.RotateVector(env.gravity);
}

void NewtonSetPlayer(int ctr)
{
	//player should be unfreezed to avoid lock states
	NewtonBodySetAutoFreeze (env.body[ctr - 1], 0);
	NewtonWorldUnfreezeBody (env.nWorld, env.body[ctr - 1]);
}

//db functions extended for Newton
void dbNewtonSync()
{
	NewtonUpdate (env.nWorld, env.timeStep);
}

void dbMakeNewtonObjectSphere(int ctr, float size, float mass)
{
//removed - unlike db object, physics object needs to be recreated each level start
//	dbMakeObjectSphere(ctr, size);

	size /= 2;
	NewtonCollision* collision = NewtonCreateSphere (env.nWorld, size, size, size, NULL); 
	env.body[ctr - 1] = NewtonCreateBody(env.nWorld, collision); 		
	NewtonReleaseCollision (env.nWorld, collision);

	NewtonBodySetUserData (env.body[ctr - 1], (void*)ctr);
	NewtonBodySetTransformCallback (env.body[ctr - 1], NewtonUpdateObject_Callback);
	NewtonBodySetForceAndTorqueCallback (env.body[ctr - 1], NewtonApplyGravity_Callback); 

	dFloat I;
	//I for sphere : I = 2/5*R²*m
	I = 2 * size * size * mass / 5;
	NewtonBodySetMassMatrix (env.body[ctr - 1], mass, I, I, I);

}

void dbDeleteNewtonObject(int ctr)
{
	NewtonDestroyBody(env.nWorld, env.body[ctr - 1]);
}

void dbLoadNewtonMeshObject(char* filename, int ctr, float scale)
{
	//load object into Darkbasic
	dbLoadObject(filename, ctr);

	//get pointer to object mesh
	dbLoadMesh(filename, 1);
	dbMakeMemblockFromMesh(2, 1);
	DWORD* pMesh = (DWORD*)dbGetMemblockPtr(2);

	//setup mesh information
	int format = *pMesh;
	int vsize = *(pMesh+1);
	int numvertices = *(pMesh+2);
	int numfaces = numvertices / 3;
	_VERTEX_ *pvertices = (_VERTEX_*)malloc(sizeof(_VERTEX_) * numvertices);

	int i;
	
	//organize vertex data (FVF)
	DWORD* vaddress;	//start address for mesh vertexdata
	for (i = 0; i < numvertices; i++)
	{
		vaddress = pMesh+3 + i * vsize / sizeof(DWORD);

		pvertices[i].x = *(float*)vaddress;
		pvertices[i].y = *(float*)(vaddress+1);
		pvertices[i].z = *(float*)(vaddress+2);

		pvertices[i].nx = *(float*)(vaddress+3);
		pvertices[i].ny = *(float*)(vaddress+4);
		pvertices[i].nz = *(float*)(vaddress+5);

		if (format == 338)
		{
			pvertices[i].diffuse = (DWORD)*(vaddress+6);
			pvertices[i].u = *(float*)(vaddress+7);
			pvertices[i].v = *(float*)(vaddress+8);
		}
		else
		{
			//format 274 does not carry diffuse info
			pvertices[i].diffuse = 0;
			pvertices[i].u = *(float*)(vaddress+6);
			pvertices[i].v = *(float*)(vaddress+7);
		}

	}

	NewtonCollision* collision = NewtonCreateTreeCollision(env.nWorld, NULL);
	NewtonTreeCollisionBeginBuild(collision);

	// add triangles to Newton collision tree
	float scalefac = scale / 100;
	float v[9];
	for(i = 0; i < numfaces; i++)
	{	
		//Vertex 1
		v[0] = pvertices[(i*3)+2].x * scalefac;
		v[1] = pvertices[(i*3)+2].y * scalefac;
		v[2] = pvertices[(i*3)+2].z * scalefac;
		//Vertex 2
		v[3] = pvertices[(i*3)+0].x * scalefac;
		v[4] = pvertices[(i*3)+0].y * scalefac;
		v[5] = pvertices[(i*3)+0].z * scalefac;
		//Vertex 3
		v[6] = pvertices[(i*3)+1].x * scalefac;
		v[7] = pvertices[(i*3)+1].y * scalefac;
		v[8] = pvertices[(i*3)+1].z * scalefac;
		NewtonTreeCollisionAddFace(collision, 3, v, sizeof(DWORD)*3, 1);
	}
	NewtonTreeCollisionEndBuild(collision, 1);
	env.body[ctr - 1] = NewtonCreateBody(env.nWorld, collision); 		
	NewtonReleaseCollision (env.nWorld, collision);
	NewtonBodySetUserData (env.body[ctr - 1], (void*)ctr);

	dbScaleObject(ctr, scale, scale, scale);

	//cleanup
	free(pvertices);
	dbDeleteMemblock(2);
	dbDeleteMesh(1);
}

void dbRotateNewtonMeshObject (int ctr, float fX, float fY, float fZ)
{
	dMatrix matrix;
	NewtonBodyGetMatrix (env.body[ctr - 1], &matrix[0][0]);

	dMatrix rmatrix (dgPitchMatrix(fX * 3.1416f / 180.0f));
	rmatrix = rmatrix * dgRollMatrix(fZ * 3.1416f / 180.0f);
	rmatrix = rmatrix * dgYawMatrix(fY * 3.1416f / 180.0f);
	rmatrix.m_posit.m_x = matrix.m_posit.m_x;
	rmatrix.m_posit.m_y = matrix.m_posit.m_y;
	rmatrix.m_posit.m_z = matrix.m_posit.m_z;

	NewtonBodySetMatrix (env.body[ctr - 1], &rmatrix[0][0]);
	//directly update object by calling callback function
	NewtonUpdateObject_Callback(env.body[ctr - 1], &rmatrix[0][0]);
}

void dbPositionNewtonObject(int ctr, float fX, float fY, float fZ)
{
	dMatrix matrix;
	NewtonBodyGetMatrix (env.body[ctr - 1], &matrix[0][0]);
	matrix.m_posit.m_x = fX;
	matrix.m_posit.m_y = fY;
	matrix.m_posit.m_z = fZ;
	NewtonBodySetMatrix (env.body[ctr - 1], &matrix[0][0]);
	//directly update object by calling callback function
	NewtonUpdateObject_Callback(env.body[ctr - 1], &matrix[0][0]);

}