//********************************************************************
// Newton Game dynamics 
// copyright 2000-2004
// By Julio Jerez
// VC: 6.0
// 
//********************************************************************

#include <stdafx.h>
#include "tutorial.h"
#include "HiResTimer.h"
#include "Materials.h"
#include "RenderPrimitive.h"

CHiResTimer timer;
  
static NewtonWorld* nWorld;
static dSceneNode* nGrapphicWorld;
static NewtonCollision* collisionA;
static NewtonCollision* collisionB;
static NewtonCollision* collisionC;


#define CAMERA_SPEED 4.0f

static dVector cameraDir (1.0f, 0.0f, 0.0f);
static dVector cameraEyepoint (0.0f, 5.0f, 0.0f);



static void CleanUp ();
static void Keyboard();


enum PrimitiveType
{
	_BOX_PRIMITIVE,
	_CONE_PRIMITIVE,
	_SPHERE_PRIMITIVE,
	_CYLINDER_PRIMITIVE,
	_CAPSULE_PRIMITIVE,
	_CHAMFER_CYLINDER_PRIMITIVE,
	_RANDOM_COMVEX_HULL_PRIMITIVE,
};




NewtonCollision* AddBlockObject (const dMatrix& matrix, const dVector& size, PrimitiveType type);


//********************************************************************
// 
//	Newton Tutorial 12 Using ray cast and other utility functions
//
//********************************************************************
int main(int argc, char **argv)
{
	// initialize opengl	
	InitOpenGl (argc, argv, "Newton Tutotial 12: Distance To a Point", DrawScene);

	// create the physics scene
	InitScene();
	
	// Execute opengl main loop for ever
	glutMainLoop();   

	return 0;
} 


// destroy the world and every thing in it
void CleanUp ()
{
	NewtonReleaseCollision (nWorld, collisionA);
	NewtonReleaseCollision (nWorld, collisionB);
	NewtonReleaseCollision (nWorld, collisionC);

	// destroy the Newton world
	NewtonDestroy (nWorld);

	// destroy the world and all objects;
	delete nGrapphicWorld;
}

static dFloat RandomVariable(dFloat amp)
{
	return amp * (dFloat (dRand() + dRand()) / dRAND_MAX - 1.0f) * 0.5f;
}



NewtonCollision* AddBlockObject (const dMatrix& srcMatrix, const dVector& size, PrimitiveType type)
{
//	NewtonBody* blockBoxBody = NULL;
	RenderPrimitive* blockBox = NULL;
	NewtonCollision* collision = NULL;

	// set realistic mass and inertia matrix for each block

	dMatrix matrix (srcMatrix);
	switch (type) 
	{
		case _SPHERE_PRIMITIVE:
		{

			dFloat r = size.m_x * 0.5f;
			
			// create the collision 
			collision = NewtonCreateSphere (nWorld, r, r, r, NULL); 

			// create a graphic box
			blockBox = new SpherePrimitive (nGrapphicWorld, matrix, r, r, r);
			
			break;
		}

		case _BOX_PRIMITIVE:
		{

			// create the collision 
			collision = NewtonCreateBox (nWorld, size.m_x, size.m_y, size.m_z, NULL); 

			// create a graphic box
			blockBox = new BoxPrimitive (nGrapphicWorld, matrix, size);
			break;
		}

		case _CONE_PRIMITIVE:
		{
			dFloat h = size.m_x;
			dFloat r = size.m_y;

			
			// create the collision 
			collision = NewtonCreateCone (nWorld, r, h, NULL); 

			// create a graphic box
			blockBox = new ConePrimitive (nGrapphicWorld, matrix, r, h);
			break;
		}

		case _CYLINDER_PRIMITIVE:
		{
			// create the collision 
			collision = NewtonCreateCylinder (nWorld, size.m_y, size.m_x, NULL); 

			// create a graphic box
			blockBox = new CylinderPrimitive (nGrapphicWorld, matrix, size.m_y, size.m_x);
			break;
		}

		case _CAPSULE_PRIMITIVE:
		{
			// create the collision 
			collision = NewtonCreateCapsule (nWorld, size.m_y, size.m_x, NULL); 

			// create a graphic box
			blockBox = new CapsulePrimitive (nGrapphicWorld, matrix, size.m_y, size.m_x);
			break;
		}

		case _CHAMFER_CYLINDER_PRIMITIVE:
		{
			// create the collision 
			matrix = dgRollMatrix (3.1416f * 0.5f) * matrix;
			collision = NewtonCreateChamferCylinder (nWorld, size.m_x * 0.5f, size.m_y, NULL); 

			// create a graphic box
			blockBox = new ChamferCylinderPrimitive (nGrapphicWorld, matrix, size.m_x * 0.5f, size.m_y);
			break;
		}


		case _RANDOM_COMVEX_HULL_PRIMITIVE:
		{
			#define SAMPLE_COUNT 500
			// Create a clouds of random point around the origin
			dVector cloud [SAMPLE_COUNT];
			int i;
			// make sure that at least the top and bottom are present
			cloud [0] = dVector ( size.m_x * 0.5f, -0.3f,  0.0f);
			cloud [1] = dVector ( size.m_x * 0.5f,  0.3f,  0.3f);
			cloud [2] = dVector ( size.m_x * 0.5f,  0.3f, -0.3f);
			cloud [3] = dVector (-size.m_x * 0.5f, -0.3f,  0.0f);
			cloud [4] = dVector (-size.m_x * 0.5f,  0.3f,  0.3f);
			cloud [5] = dVector (-size.m_x * 0.5f,  0.3f, -0.3f);

			// populate the cloud with pseudo Gaussian random points
			for (i = 6; i < SAMPLE_COUNT; i ++) {
				cloud [i].m_x = RandomVariable(size.m_x);
				cloud [i].m_y = RandomVariable(size.m_y * 2.0f);
				cloud [i].m_z = RandomVariable(size.m_z * 2.0f);
			}
			
			collision = NewtonCreateConvexHull (nWorld, SAMPLE_COUNT, &cloud[0].m_x, sizeof (dVector), NULL); 
			
			// create a graphic box
			blockBox = new ConvexHullPrimitive (nGrapphicWorld, matrix, nWorld, collision);
			break;
		}
	}

	return collision;
}


NewtonCollision* CompoundCollisionStruture (dMatrix& baseMatrix)
{
	int i;
	int j;
	int count;
	int segments;
	int hullCount;
	dFloat A;
	dFloat C;
	dFloat alpha;
	dFloat beta0;
	dFloat beta1;
	dVector array[200];
	NewtonCollision* collision;
	NewtonCollision* conhexHullArray[100];
  
	A = 2.0f;
	C = 4.0f;

	segments = 16;
	hullCount = 0;
	// create the upper half of a change
	for (i = 0; i < segments; i ++) {
		beta0 = 2.0f * dFloat (i) * 3.1416 / segments; 
		beta1 = 2.0f * dFloat (i + 1) * 3.1416 / segments; 
		count = 0;
		for (j = 0; j < segments; j ++) {
			alpha = 2.0f * dFloat (j) * 3.1416 / segments; 
			array[count].m_x = A * dSin (alpha);
			array[count].m_y = (C + A * dCos(alpha)) * dCos (beta0);
			array[count].m_z = (C + A * dCos(alpha)) * dSin (beta0);
			count ++;

			array[count].m_x = A * dSin (alpha);
			array[count].m_y = (C + A * dCos(alpha)) * dCos (beta1);
			array[count].m_z = (C + A * dCos(alpha)) * dSin (beta1);
			count ++;
		}
		conhexHullArray[hullCount] = NewtonCreateConvexHull (nWorld, count, &array[0].m_x, sizeof (dVector), NULL);
		hullCount ++;
	}

	RenderPrimitive* blockBox;
	baseMatrix = dgYawMatrix (0.5f * 3.1416) * baseMatrix;

	dMatrix matrix (baseMatrix);
	blockBox = new TorusPrimitive (nGrapphicWorld, matrix, A, C);

	// create the compound collision
	collision = NewtonCreateCompoundCollision (nWorld, hullCount, conhexHullArray);

	// release all collision parts
	for (i = 0; i < hullCount; i ++) {
		NewtonReleaseCollision (nWorld, conhexHullArray[i]);
	}

	return collision;
}


// create physics scene
void InitScene()
{
	// /////////////////////////////////////////////////////////////////////
	//
	// create the newton world
	nWorld = NewtonCreate (NULL, NULL);

	// Set the adaptive solve for compromise trade off speed/stability combination
	NewtonSetSolverModel (nWorld, 1);

	// used the adaptive friction model
	NewtonSetFrictionModel (nWorld, 1);

	// Set the termination function
	atexit(CleanUp); 


	// /////////////////////////////////////////////////////////////////////
	//
	// create the sky box,
	nGrapphicWorld = new dSceneNode(NULL);


	dMatrix baseMatrix (GetIdentityMatrix());

	// get the floor position in from o the camera
	baseMatrix.m_posit.m_x = cameraEyepoint.m_x + 30.0f;
	baseMatrix.m_posit.m_z = 0.0f;
	baseMatrix.m_posit.m_y = 0.0f; 

	dVector size(8.0f, 4.0f, 6.0f);

	collisionA = AddBlockObject (baseMatrix, size, _BOX_PRIMITIVE);
//	collisionA = AddBlockObject (baseMatrix, size, _RANDOM_COMVEX_HULL_PRIMITIVE);

	baseMatrix.m_posit.m_z -= 20.0f;
	collisionB = AddBlockObject (baseMatrix, size, _CONE_PRIMITIVE);
//	collisionB = CompoundCollisionStruture (baseMatrix);

	baseMatrix.m_posit.m_z = 0.0f;
	baseMatrix.m_posit.m_y += 20.0f;
//	collisionC = AddBlockObject (baseMatrix, size, _CONE_PRIMITIVE);
	collisionC = CompoundCollisionStruture (baseMatrix);
}



//	Keyboard handler. 
void Keyboard()
{
	// check for termination
	if (dGetKeyState (VK_ESCAPE) & 0x8000) {
		exit(0);
	}

	// show debug information when press F2
//	static unsigned prevF2Key = ~(dGetKeyState (VK_F2) & 0x8000);
//	unsigned f2Key = dGetKeyState (VK_F2) & 0x8000;
//	if (prevF2Key & f2Key) {
//		// toggle debug line mode 
//		debugLinesMode = ! debugLinesMode;
//	}
//	prevF2Key = ~f2Key;


	// read the mouse position and set the camera direction
//	static MOUSE_POINT mouse0;
//	static dFloat yawAngle = 0.0f;
//	static dFloat rollAngle = 0.0f;
//	static bool mousePickMode = false;
//	static int mouseLeftKey0 = dGetKeyState (VK_LBUTTON) & 0x8000;

	// camera control
	if (dGetKeyState ('W') & 0x8000) {
		cameraEyepoint += cameraDir.Scale (CAMERA_SPEED / 60.0f);
	} else if (dGetKeyState ('S') & 0x8000) {
		cameraEyepoint -= cameraDir.Scale (CAMERA_SPEED / 60.0f);
	}

	if (dGetKeyState ('D') & 0x8000) {
		dVector up (0.0f, 1.0f, 0.0f);
		dVector right (cameraDir * up);
		cameraEyepoint += right.Scale (CAMERA_SPEED / 60.0f);
	} else if (dGetKeyState ('A') & 0x8000) {
		dVector up (0.0f, 1.0f, 0.0f);
		dVector right (cameraDir * up);
		cameraEyepoint -= right.Scale (CAMERA_SPEED / 60.0f);
	}
} 


// DrawScene()
void DrawScene ()
{
	dFloat timeStep;

	// get the time step
	timeStep = timer.GetElapsedSeconds();

	// read the keyboard
	Keyboard ();

	// move the camera
	dVector target (cameraEyepoint + cameraDir);
	SetCamera (cameraEyepoint, target);

	dSceneNode* node0;
	dSceneNode* node1;
	dSceneNode* node2;

	static dFloat angle0 = 0, angle1= 0, angle2 = 0.0f;

	angle0 = dMod (angle0 + 0.001f * 3.1416f, 2.0f * 3.1316f);
	angle1 = dMod (angle1 + 0.0005f * 3.1416f, 2.0f * 3.1316f);
	angle2 = dMod (angle2 + 0.0007f * 3.1416f, 2.0f * 3.1316f);

	dMatrix rotation (dgPitchMatrix(angle0) * dgYawMatrix(angle1) * dgRollMatrix(angle2));

	node0 = nGrapphicWorld->GetChild();
	dMatrix matrix0 (rotation);
	static dVector posit (node0->GetMatrix().m_posit);
	matrix0.m_posit = posit + matrix0.m_front.Scale (15.0f);
	node0->SetMatrix (matrix0);

	node1 = node0->GetSibling();
	dMatrix matrix1 (rotation);
	matrix1.m_posit = node1->GetMatrix().m_posit;
	node1->SetMatrix (matrix1);

	node2 = node1->GetSibling();;
	dMatrix matrix2 (rotation);
	matrix2.m_posit = node2->GetMatrix().m_posit;
	node2->SetMatrix (matrix2);

	// render the scene
	glEnable (GL_LIGHTING);
	glEnable (GL_BLEND);
	glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_DST_ALPHA);


	// find the closest point from and arbitrary point to geometryA
	dVector contact1, normal1;
	static dVector origin (matrix0.m_posit + dVector (0.0f, 0.0f, 15.0f));


	if (NewtonCollisionPointDistance(nWorld, &origin[0], collisionA, &matrix0[0][0], &contact1[0], &normal1[0])) {
		ShowMousePicking (origin, contact1, 0.5f);
	} 
	

	// find the closest point geometry A and B
	dVector contact2;
	dVector contact3;
	dVector normal23;
	if (NewtonCollisionClosestPoint(nWorld, collisionA, &matrix0[0][0], collisionB, &matrix1[0][0], &contact2[0], &contact3[0],  &normal23[0])) {
		ShowMousePicking (contact2, contact3, 0.5f);
	} else {
		// here is means this tow collision are intersecting
		// the first time it mean they touching
	}
  

	// find the closest point geometry A and C
	dVector contact4;
	dVector contact5;
	dVector normal45;
	if (NewtonCollisionClosestPoint(nWorld, collisionA, &matrix0[0][0], collisionC, &matrix2[0][0], &contact4[0], &contact5[0],  &normal45[0])) {
		ShowMousePicking (contact4, contact5, 0.5f);
	}

	else {
		_ASSERTE (0);
	}

 	dVector contact6, normal6;
	if (NewtonCollisionPointDistance(nWorld, &origin[0], collisionC, &matrix2[0][0], &contact6[0], &normal6[0])) {
		ShowMousePicking (origin, contact6, 0.5f);
	}

	else {
		_ASSERTE (0);
	}

	dVector contact7;
	dVector contact8;
	dVector normal78;
	if (NewtonCollisionClosestPoint(nWorld, collisionB, &matrix1[0][0], collisionC, &matrix2[0][0], &contact7[0], &contact8[0],  &normal78[0])) {
		ShowMousePicking (contact7, contact8, 0.5f);
	}

	else {
		_ASSERTE (0);
	}

	glPushMatrix();
	nGrapphicWorld->Render();
	glPopMatrix();

	dVector color (1.0, 1.0f, 1.0f);
	Print (color, 4,  4, "fps %f", 1.0f / (timeStep + 1.0e-6f));
//	Print (color, 4, 34, "f1 - auto sleep on/off      f2 - show collision boxes");
} 






