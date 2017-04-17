#ifndef GDKNEWTON_H
#define GDKNEWTON_H

#include <DarkGDK.h>
#include <newton.h>
#include <OpenGlUtil.h>

#include "environment.h"

typedef struct
{
	float x, y, z;
	float nx, ny, nz;
	DWORD diffuse;
	float u,v;
}_VERTEX_;

#define GRAVITY_FAC	5

// Prototypes - Callbacks
void* NewtonAlloc_Callback(int sizeInBytes);
void NewtonFree_Callback(void *ptr, int sizeInBytes);
void NewtonCleanUp_Callback();
void NewtonUpdateObject_Callback(const NewtonBody* body, const dFloat* matrix);
void NewtonApplyGravity_Callback(const NewtonBody* body);

// Prototypes - Public
void NewtonInit();
void NewtonSetMass(int ctr, float mass);
void NewtonSetGravity(float fX, float fY, float fZ);
void NewtonSetPlayer(int ctr);

void dbNewtonSync();
void dbMakeNewtonObjectSphere(int ctr, float size, float mass = 0.0f);
void dbDeleteNewtonObject(int ctr);
void dbLoadNewtonMeshObject(char* filename, int ctr, float scale = 1.0f);
void dbRotateNewtonMeshObject (int ctr, float fX, float fY, float fZ);
void dbPositionNewtonObject(int ctr, float fX, float fY, float fZ);

#endif
