//********************************************************************
// Newton Game dynamics 
// copyright 2000-2004
// By Julio Jerez
// VC: 6.0
// simple demo list vector class with iterators
//********************************************************************

#include <stdafx.h>
#include "tutorial.h"
#include "MousePick.h"
#include "HiResTimer.h"
#include "Materials.h"
#include "JointsTutorial.h"
#include "LevelPrimitive.h"
#include "RenderPrimitive.h"
#include "CharaterControl.h"
#include "CustomDryRollingFriction.h"


CHiResTimer timer;
#define GRAVITY	-10.0f

static NewtonWorld* nWorld;
static dSceneNode* nGrapphicWorld;
static CharacterController* player;

dVector cameraDir (1.0f, 0.0f, 0.0f);
dVector cameraEyepoint (0.0f, 5.0f, 0.0f);

//static bool forceMode;
//static dVector cameraTorqueVector (0.0f, 0.0f, 0.0f); 

bool hideObjects = false;
//bool hideObjects = true;

bool debugLinesMode = false;
//bool debugLinesMode = true;

bool animateUpVector = false;




static void  CleanUp ();
static void  Keyboard();
static void* PhysicsAlloc (int sizeInBytes);
static void  PhysicsFree (void *ptr, int sizeInBytes);


static void PopulateLevel ();
static void AddBallsWithRollingFriction ();


int debugCount = 0;
dVector debugFace[1024][2];

static void DebugShowCollision ();
static void DebugShowBodyCollision (const NewtonBody* body);
static void DebugShowGeometryCollision (const NewtonBody* body, int vertexCount, const dFloat* faceVertec, int id);

// set this to 0 for more realistic game (the stack will never go to sleep mode)
//#define AUTO_SLEEP_MODE 1
//static bool sleepMode = AUTO_SLEEP_MODE;


//********************************************************************
// 
//	Newton Tutorial 10 custom joints
//					   a
//********************************************************************
int main(int argc, char **argv)
{
	// initialize Opengl	
	InitOpenGl (argc, argv, "Newton Tutotial 10: Custom joints", DrawScene);

	// create the physics scene
	InitScene();
	
	// Execute Opengl main loop for ever
	glutMainLoop();   

	return 0;
} 

// memory allocation for Newton
void* PhysicsAlloc (int sizeInBytes)
{
	return malloc (sizeInBytes);
}

// memory de-allocation for Newton
void PhysicsFree (void *ptr, int sizeInBytes)
{
	free (ptr);
}

// destroy the world and every thing in it
void CleanUp ()
{
	// clear all resources use with materials
	CleanUpMaterials (nWorld);

	// destroy the Newton world
	NewtonDestroy (nWorld);
}

// rigid body destructor
void PhysicsBodyDestructor (const NewtonBody* body)
{
	RenderPrimitive* primitive;

	// get the graphic object form the rigid body
	primitive = (RenderPrimitive*) NewtonBodyGetUserData (body);

	// destroy the graphic object
	delete primitive;
}

// set the transformation of a rigid body
void PhysicsApplyGravityForce (const NewtonBody* body)
{
	dFloat mass;
	dFloat Ixx;
	dFloat Iyy;
	dFloat Izz;

	// to apply external force we need to get the object mass
	NewtonBodyGetMassMatrix (body, &mass, &Ixx, &Iyy, &Izz);
	dVector force (0.0f, mass * GRAVITY, 0.0f);

	// apply the force
	NewtonBodySetForce (body, &force.m_x);
}


// add force and torque to rigid body
void PhysicsSetTransform (const NewtonBody* body, const dFloat* matrix)
{
	RenderPrimitive* primitive;

	// get the graphic object form the rigid body
	primitive = (RenderPrimitive*) NewtonBodyGetUserData (body);

	// set the transformation matrix for this rigid body
	dMatrix& mat = *((dMatrix*)matrix);
	primitive->SetMatrix (mat);
}




// create physics scene
void InitScene()
{
	LevelPrimitive *level;

	// /////////////////////////////////////////////////////////////////////
	//
	// create the newton world
	nWorld = NewtonCreate (PhysicsAlloc, PhysicsFree);

	// Set the termination function
	atexit(CleanUp); 


	// /////////////////////////////////////////////////////////////////////
	//
	// create the sky box,
	nGrapphicWorld = new SkyBoxPrimitive (NULL);


	// /////////////////////////////////////////////////////////////////////
	//
	// set up all material and  material interactions
	SetupMaterials (nWorld);

	// /////////////////////////////////////////////////////////////////////
	//
	// add some joint configurations
	AddRope (nGrapphicWorld, nWorld);
	AddVise (nGrapphicWorld, nWorld);
	AddGears (nGrapphicWorld, nWorld);
	AddRollingBeats (nGrapphicWorld, nWorld);
	AddDoubleSwingDoors (nGrapphicWorld, nWorld);
	
	// add some object to the background
	PopulateLevel ();
	AddBallsWithRollingFriction ();

	// /////////////////////////////////////////////////////////////////////
	//
	// Load level geometry and add it to the display list 
	level = new LevelPrimitive (nGrapphicWorld, "level1.dg", nWorld, NULL);

	// assign the concrete id to the level geometry
	NewtonBodySetMaterialGroupID (level->m_level, levelID);


	// /////////////////////////////////////////////////////////////////////
	//
	// Create a character controller
//	dVector spawnSize (0.75f, 2.0f, 0.75f);
	dVector spawnSize (1.125f, 4.0f, 1.125f);
	dVector spawnPosit (0.0f, 10.0f, 0.0f);
	player = new CharacterController (nGrapphicWorld, nWorld, spawnPosit, spawnSize);

}


// show collision geometry in debug mode
void DebugShowGeometryCollision (const NewtonBody* body, int vertexCount, const dFloat* faceVertec, int id)
{
	int i;

	i = vertexCount - 1;
	dVector p0 (faceVertec[i * 3 + 0], faceVertec[i * 3 + 1], faceVertec[i * 3 + 2]);
	for (i = 0; i < vertexCount; i ++) {
		dVector p1 (faceVertec[i * 3 + 0], faceVertec[i * 3 + 1], faceVertec[i * 3 + 2]);
		glVertex3f (p0.m_x, p0.m_y, p0.m_z);
		glVertex3f (p1.m_x, p1.m_y, p1.m_z);
 		p0 = p1;
	}
}

// show rigid body collision geometry
void  DebugShowBodyCollision (const NewtonBody* body)
{
	dFloat Ixx;
	dFloat Iyy;
	dFloat Izz;
	dFloat mass;

	// only show collision for dynamics objects
	NewtonBodyGetMassMatrix (body, &mass, &Ixx, &Iyy, &Izz);
//	if (mass > 0.0f) {
		NewtonBodyForEachPolygonDo (body, DebugShowGeometryCollision);
//	}
}


// show all collision geometry is debug mode
void  DebugShowCollision ()
{
	int i;

	glDisable (GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	
	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 0.0f);
	NewtonWorldForEachBodyDo (nWorld, DebugShowBodyCollision);

	glColor3f(0.0f, 0.0f, 1.0f);
	for (i = 0; i < debugCount; i ++) {
		glVertex3f (debugFace[i][0].m_x, debugFace[i][0].m_y, debugFace[i][0].m_z);
		glVertex3f (debugFace[i][1].m_x, debugFace[i][1].m_y, debugFace[i][1].m_z);
	}
	glEnd();
}


// DrawScene()
void DrawScene ()
{
	dFloat timeStep;


	// get the time step
	timeStep = timer.GetElapsedSeconds();

	// update the Newton physics world
	NewtonUpdate (nWorld, timeStep);


	// move the camera
	dVector target (cameraEyepoint + cameraDir);
	SetCamera (cameraEyepoint, target);

	// render the scene
	if (!hideObjects) {
		glEnable (GL_LIGHTING);
		glPushMatrix();	
		nGrapphicWorld->Render ();
		glPopMatrix();
	}

	// read the keyboard
	Keyboard ();

	// if debug display is on, scan all rigid bodies and display the collision geometry in wire frame
	if (debugLinesMode) {
		DebugShowCollision ();
	}
	debugCount = 0;

	dVector color (1.0, 1.0f, 0.0f);
	Print (color, 4, 4, "fps %f", 1.0f / (timeStep + 1.0e-6f));
	Print (color, 4, 34, "f1 - show collition mesh");
	Print (color, 4, 64, "f2 - hide model");
	Print (color, 4, 94, "f3 - toogle animation mode for upvector");
} 


//	Keyboard handler. 
void Keyboard()
{
	// check for termination
	if (dGetKeyState (VK_ESCAPE) & 0x8000) {
		exit(0);
	}

	// show debug information when press F1
	static unsigned prevF1Key = ~(dGetKeyState (VK_F1) & 0x8000);
	unsigned f1Key = dGetKeyState (VK_F1) & 0x8000;
	if (prevF1Key & f1Key) {
		// clear debugLines counter
		debugLinesMode = ! debugLinesMode;
	}
	prevF1Key = ~f1Key;


	static unsigned prevF2Key = ~(dGetKeyState (VK_F2) & 0x8000);
	unsigned f2Key = dGetKeyState (VK_F2) & 0x8000;
	if (prevF2Key & f2Key) {
		// toggle debug line mode 
		hideObjects = ! hideObjects;
	}
	prevF2Key = ~f2Key;

	static unsigned prevF3Key = ~(dGetKeyState (VK_F3) & 0x8000);
	unsigned f3Key = dGetKeyState (VK_F3) & 0x8000;
	if (prevF3Key & f3Key) {
		// toggle debug line mode 
		animateUpVector = ! animateUpVector;
	}
	prevF3Key = ~f3Key;

	// read the mouse position and set the camera direction
	static dFloat yawAngle = 0.0f * 3.1416 / 180.0f;
	static dFloat rollAngle = 0.0f;

	MOUSE_POINT mouse1;
	dInt32 mouseLeftKey;
	static MOUSE_POINT mouse0;
	
	GetCursorPos(mouse1);
	mouseLeftKey = dGetKeyState (VK_LBUTTON) & 0x8000;

	// this section control the camera object picking
	mouseLeftKey = dGetKeyState (VK_LBUTTON) & 0x8000;
	if (!MousePick (nWorld, mouse1, mouseLeftKey, 0.125f, 1.0f)) {
		// we are not in mouse pick mode, then we are in camera tracking mode
		if (mouseLeftKey) {
			// when click left mouse button the first time, we reset the camera
			// convert the mouse x position to delta yaw angle
			if (mouse1.x > (mouse0.x + 1)) {
				yawAngle += 1.0f * 3.1416 / 180.0f;
				if (yawAngle > (360.0f * 3.1416 / 180.0f)) {
					yawAngle -= (360.0f * 3.1416 / 180.0f);
				}
			} else if (mouse1.x < (mouse0.x - 1)) {
				yawAngle -= 1.0f * 3.1416 / 180.0f;
				if (yawAngle < 0.0f) {
					yawAngle += (360.0f * 3.1416 / 180.0f);
				}
			}

			if (mouse1.y > (mouse0.y + 1)) {
				rollAngle += 1.0f * 3.1416 / 180.0f;
				if (rollAngle > (80.0f * 3.1416 / 180.0f)) {
					rollAngle = 80.0f * 3.1416 / 180.0f;
				}
			} else if (mouse1.y < (mouse0.y - 1)) {
				rollAngle -= 1.0f * 3.1416 / 180.0f;
				if (rollAngle < -(80.0f * 3.1416 / 180.0f)) {
					rollAngle = -80.0f * 3.1416 / 180.0f;
				}
			}
			dMatrix cameraDirMat (dgRollMatrix(rollAngle) * dgYawMatrix(yawAngle));
			cameraDir = cameraDirMat.m_front;
		}
	}

	// save mouse position and left mouse key state for next frame
	mouse0 = mouse1;


	// apply a simple player input control
	if (player) {
		dVector force (0.0f, 0.0f, 0.0f); 

		// camera control
		if (dGetKeyState ('W') & 0x8000) {
			force.m_x = 1.0f;
		} else if (dGetKeyState ('S') & 0x8000) {
			force.m_x = -1.0f;
		}

		if (dGetKeyState ('D') & 0x8000) {
			force.m_z = 1.0f;
		} else if (dGetKeyState ('A') & 0x8000) {
			force.m_z = -1.0f;
		}

		// only one jump per space key hit
		static unsigned prevSpace = ~(dGetKeyState (' ') & 0x8000);
		unsigned space = dGetKeyState (' ') & 0x8000;
		if (prevSpace & space) {
			force.m_y = 1.0f;
		}
		prevSpace = ~space;

		player->SetForce (force);

		cameraEyepoint = player->GetMatrix().m_posit;
		cameraEyepoint.m_y += 1.0f;

		cameraEyepoint -= cameraDir.Scale (6.0f);
	}
} 


void PopulateLevel ()
{
	NewtonCollision* collision;

	// set the initial size
	dVector size (1.0f, 1.0f, 1.0f);

	// create the collision 
	collision = NewtonCreateBox (nWorld, size.m_x, size.m_y, size.m_z, NULL); 
		
	// populate the world with some boxes 
	dMatrix location (GetIdentityMatrix());
	location.m_posit.m_x = -10.0f; 
//location.m_posit.m_x = 2.0f; 

	dFloat damp[3];
	damp[0] = 0.0f;
	damp[1] = 0.0f;
	damp[2] = 0.0f;

	for (int k = 0; k < 10; k ++) { 
		location.m_posit.m_z = 0.0f; 
		for (int j = 0; j < 10; j ++) { 

			location.m_posit.m_y = 10.0f; 
			for (int i = 0; i < 1; i ++) {
				BoxPrimitive* box;
				NewtonBody* boxBody;

				// create a graphic box
				box = new BoxPrimitive (nGrapphicWorld, location, size);

				//create the rigid body
				boxBody = NewtonCreateBody (nWorld, collision);

				// set zero linear and angular drag
				NewtonBodySetLinearDamping (boxBody, 0.0f);
				NewtonBodySetAngularDamping (boxBody, damp);

				//dVector omega(5, 0, 0);
				//NewtonBodySetOmega (boxBody, &omega[0]);

				// Set Material Id for this object
				if (dRand() & 1) {
					NewtonBodySetMaterialGroupID (boxBody, metalID);
				} else {
					NewtonBodySetMaterialGroupID (boxBody, woodID);
				}

				// save the pointer to the graphic object with the body.
				NewtonBodySetUserData (boxBody, box);

				// set a destructor for this rigid body
				NewtonBodySetDestructorCallback (boxBody, PhysicsBodyDestructor);

				// set the transform call back function
				NewtonBodySetTransformCallback (boxBody, PhysicsSetTransform);

				// set the force and torque call back function
				NewtonBodySetForceAndTorqueCallback (boxBody, PhysicsApplyGravityForce);


				// calculate a accurate moment of inertia
				dVector origin;
				dVector inertia;
				NewtonConvexCollisionCalculateInertialMatrix (collision, &inertia[0], &origin[0]);	

				dFloat mass = 1.0f;
				dFloat Ixx = mass * inertia[0];
				dFloat Iyy = mass * inertia[1];
				dFloat Izz = mass * inertia[2];

				// set the mass matrix
				NewtonBodySetMassMatrix (boxBody, mass, Ixx, Iyy, Izz);

				// set the matrix for both the rigid body and the graphic body
				NewtonBodySetMatrix (boxBody, &location[0][0]);
				PhysicsSetTransform (boxBody, &location[0][0]);

				location.m_posit.m_y += size.m_y * 2.0f;
			}
			location.m_posit.m_z -= size.m_z * 4.0f; 	
		}
		location.m_posit.m_x += size.m_x * 4.0f; 
	}

	// release the collision geometry when not need it
	NewtonReleaseCollision (nWorld, collision);
}


void AddBallsWithRollingFriction ()
{
	dFloat r;
	dFloat Ixx;
	dFloat Iyy;
	dFloat Izz;
	dFloat mass;

	NewtonCollision* collision;

	// set the initial size
	dVector size (1.0f, 1.0f, 1.0f);

		
	// populate the world with some boxes 
	dMatrix location (GetIdentityMatrix());
//	location.m_posit.m_x = -12.0f; 
	location.m_posit.m_z = -2.0f; 

	dFloat damp[3];
	damp[0] = 0.0f;
	damp[1] = 0.0f;
	damp[2] = 0.0f;

	// it is very important that the mass and the inertia are calculated correctly
	mass = 5.0f;
	r = size.m_x * 0.5f;
//	Ixx = 0.4f * mass * r * r;
//	Iyy = Ixx;
//	Izz = Ixx;

	// create the collision 
	collision = NewtonCreateSphere (nWorld, r, r, r, NULL); 

	// calculate a accurate moment of inertia
	dVector origin;
	dVector inertia;
	NewtonConvexCollisionCalculateInertialMatrix (collision, &inertia[0], &origin[0]);	
	Ixx = mass * inertia[0];
	Iyy = mass * inertia[1];
	Izz = mass * inertia[2];
			
	for (int k = 0; k < 5; k ++) { 
//	for (int k = 0; k < 1; k ++) { 
		location.m_posit.m_z = 0.0f; 
		for (int j = 0; j < 5; j ++) { 
//		for (int j = 0; j < 1; j ++) { 
			location.m_posit.m_y = 10.0f; 
			for (int i = 0; i < 1; i ++) {
				RenderPrimitive* box;
				NewtonBody* boxBody;

				// create a graphic box
				box = new SpherePrimitive (nGrapphicWorld, location, r, r, r);

				//create the rigid body
				boxBody = NewtonCreateBody (nWorld, collision);

				// set zero linear and angular drag
				NewtonBodySetLinearDamping (boxBody, 0.0f);
				NewtonBodySetAngularDamping (boxBody, damp);

				//dVector omega(5, 0, 0);
				//NewtonBodySetOmega (boxBody, &omega[0]);

				// Set Material Id for this object
				if (dRand() & 1) {
					NewtonBodySetMaterialGroupID (boxBody, metalID);
				} else {
					NewtonBodySetMaterialGroupID (boxBody, woodID);
				}

				// save the pointer to the graphic object with the body.
				NewtonBodySetUserData (boxBody, box);

				// set a destructor for this rigid body
				NewtonBodySetDestructorCallback (boxBody, PhysicsBodyDestructor);

				// set the transform call back function
				NewtonBodySetTransformCallback (boxBody, PhysicsSetTransform);

				// set the force and torque call back function
				NewtonBodySetForceAndTorqueCallback (boxBody, PhysicsApplyGravityForce);

				// set the mass matrix
				NewtonBodySetMassMatrix (boxBody, mass, Ixx, Iyy, Izz);

				// set the matrix for both the rigid body and the graphic body
				NewtonBodySetMatrix (boxBody, &location[0][0]);
				PhysicsSetTransform (boxBody, &location[0][0]);

				location.m_posit.m_y += size.m_y * 2.0f;

				// Add a dry rolling friction joint to the ball
				new CustomDryRollingFriction (boxBody, r, 5.2f);

			}
			location.m_posit.m_z -= size.m_z * 4.0f; 	
		}
		location.m_posit.m_x += size.m_x * 4.0f; 
	}

	// release the collision geometry when not need it
	NewtonReleaseCollision (nWorld, collision);
}








