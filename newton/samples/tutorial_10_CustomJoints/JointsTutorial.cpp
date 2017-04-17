//********************************************************************
// Newton Game dynamics 
// copyright 2000-2004
// By Julio Jerez
// VC: 6.0
// simple demo list vector class with iterators
//********************************************************************

// JointsTutorial.cpp: implementation of the JointsTutorial class.
//
//////////////////////////////////////////////////////////////////////
#include <stdafx.h>
#include "tutorial.h"
#include "Materials.h"
#include "JointsTutorial.h"
#include "CustomGear.h"
#include "CustomPulley.h"
#include "CustomWormGear.h"
#include "CustomHinge.h"
#include "CustomSlider.h"
#include "CustomUniversal.h"
#include "CustomCorkScrew.h"
#include "CustomBallAndSocket.h"
#include "CustomConeLimitedBallAndSocket.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// create a rope of boxes
void AddRope (dSceneNode* parent, NewtonWorld* nWorld)
{
	int i;
	dFloat mass;
	dFloat Ixx;
	dFloat Iyy;
	dFloat Izz;
	NewtonBody* link0;
	NewtonBody* link1;
	NewtonCustomJoint* joint;
	
	NewtonCollision* collision;
	RenderPrimitive* visualObject;


	dVector size (2.0f, 0.25f, 0.25f);


	//dMatrix location (GetIdentityMatrix());
	dMatrix location (dgRollMatrix(3.1426f * 0.5f));
	location.m_posit.m_y =  9.0f; 
	location.m_posit.m_z = -5.0f; 

	// create a collision primitive to be shared by all links
	collision = NewtonCreateCapsule (nWorld, size.m_y, size.m_x, NULL);


	// calculate a accurate moment of inertia
	dVector origin;
	dVector inertia;
	NewtonConvexCollisionCalculateInertialMatrix (collision, &inertia[0], &origin[0]);	

	mass = 2.0f;
	Ixx = mass * inertia[0];
	Iyy = mass * inertia[1];
	Izz = mass * inertia[2];


	link0 = NULL;
	// create a long vertical rope with limits
	for (i = 0; i < 7; i ++) {
		// create the a graphic character (use a visualObject as our body
		visualObject = new CapsulePrimitive (parent, location, size.m_y, size.m_x);

		//create the rigid body
		link1 = NewtonCreateBody (nWorld, collision);


		// add some damping to each link
		NewtonBodySetLinearDamping (link1, 0.2f);
		dVector angularDamp (0.2f, 0.2f, 0.2f);
		NewtonBodySetAngularDamping (link1, &angularDamp.m_x);

		// Set Material Id for this object
		NewtonBodySetMaterialGroupID (link1, woodID);

		// save the pointer to the graphic object with the body.
		NewtonBodySetUserData (link1, visualObject);

		// set a destructor for this rigid body
		NewtonBodySetDestructorCallback (link1, PhysicsBodyDestructor);

		// set the transform call back function
		NewtonBodySetTransformCallback (link1, PhysicsSetTransform);

		// set the force and torque call back function
		NewtonBodySetForceAndTorqueCallback (link1,PhysicsApplyGravityForce);

		// set the mass matrix
		NewtonBodySetMassMatrix (link1, mass, Ixx, Iyy, Izz);

		// set the matrix for both the rigid body and the graphic body
		NewtonBodySetMatrix (link1, &location[0][0]);
		PhysicsSetTransform (link1, &location[0][0]);

		dVector pivot (location.m_posit);
		pivot.m_y += (size.m_x - size.m_y) * 0.5f;

		dFloat coneAngle = 15.0 * 3.1416f / 180.0f;
		dFloat twistAngle = 5.0 * 3.1416f / 180.0f;
		dVector pin (location.m_front.Scale (-1.0f));

		joint = new CustomConeLimitedBallAndSocket(twistAngle, coneAngle, pin, pivot, link1, link0);

		link0 = link1;
		location.m_posit.m_y -= (size.m_x - size.m_y);
	}

	// release the collision geometry when not need it
	NewtonReleaseCollision (nWorld, collision);
}



void AddRollingBeats (dSceneNode* parent, NewtonWorld* nWorld)
{
	dFloat mass;
	dFloat Ixx;
	dFloat Iyy;
	dFloat Izz;
	NewtonBody* bar;
	NewtonCollision* collision;

	dMatrix location (GetIdentityMatrix());
	location.m_posit.m_x = -20.0f; 
	location.m_posit.m_y = -1.0f; 
	location.m_posit.m_z = -10.0f; 
	dVector size (10.0f, 0.25f, 0.25f);

	bar = NULL;
	// /////////////////////////////////////////////////////////////////////////////////////             
	//
	// create a bar and attach it to the world with a hinge with limits
	//
	// ////////////////////////////////////////////////////////////////////////////////////
	{
		CustomHinge* joint;
		RenderPrimitive* visualObject;

		// create the a graphic character (use a visualObject as our body
		visualObject = new CylinderPrimitive (parent, location, size.m_y, size.m_x);

		// create a collision primitive to be shared by all links
		collision = NewtonCreateCylinder (nWorld, size.m_y, size.m_x, NULL); 

		// create the bar body
		bar = NewtonCreateBody(nWorld, collision);
		NewtonReleaseCollision (nWorld, collision);

		// attach graphic object to the rigid body
		NewtonBodySetUserData(bar, visualObject);

		// set a destructor function
		NewtonBodySetDestructorCallback (bar, PhysicsBodyDestructor);

		// set the transform call back function
		NewtonBodySetTransformCallback (bar, PhysicsSetTransform);

		// set the force and torque call back function
		NewtonBodySetForceAndTorqueCallback (bar,PhysicsApplyGravityForce);

		// calculate a accurate moment of inertia
		dVector origin;
		dVector inertia;
		NewtonConvexCollisionCalculateInertialMatrix (collision, &inertia[0], &origin[0]);	

		mass = 5.0f;
		Ixx = mass * inertia[0];
		Iyy = mass * inertia[1];
		Izz = mass * inertia[2];

		// set the mass matrix
		NewtonBodySetMassMatrix (bar, mass, Ixx, Iyy, Izz);

		// set the matrix for both the rigid body and the graphic body
		NewtonBodySetMatrix (bar, &location[0][0]);
		PhysicsSetTransform (bar, &location[0][0]);

		dVector pin (0.0f, 1.0f, 0.0f);
		dVector pivot (location.m_posit);
		pivot.m_x -= size.m_x * 0.5f;
  
		// connect these two bodies by a ball and socket joint
		//joint = NewtonConstraintCreateHinge (nWorld, &pivot.m_x, &pin.m_x, link0, link1);
		joint = new CustomHinge (pivot, pin, bar, NULL);
	}


	{
		// ////////////////////////////////////////////////////////////////////////////////////
		//
		// add a corkscrew visualObject with limits
		//
		// ////////////////////////////////////////////////////////////////////////////////////
		NewtonBody* beat;
		CustomCorkScrew* joint;
		RenderPrimitive* visualObject;
		dMatrix beatLocation (location);
		dVector beatSize (0.5f, 1.25f, 1.25f);

		beatLocation.m_posit.m_x -= size.m_x * 0.25f;

		// create the a graphic character (use a visualObject as our body
		//visualObject = new BoxPrimitive (beatLocation, beatSize);
		visualObject = new ChamferCylinderPrimitive (parent, beatLocation, beatSize.m_y, beatSize.m_x);

		// create a collision primitive to be shared by all links
		collision = NewtonCreateChamferCylinder (nWorld, beatSize.m_y, beatSize.m_x, NULL); 

		beat = NewtonCreateBody(nWorld, collision);
		NewtonReleaseCollision (nWorld, collision);

			// attach graphic object to the rigid body
		NewtonBodySetUserData(beat, visualObject);

		// set a destructor function
		NewtonBodySetDestructorCallback (beat, PhysicsBodyDestructor);

		// set the transform call back function
		NewtonBodySetTransformCallback (beat, PhysicsSetTransform);

		// set the force and torque call back function
		NewtonBodySetForceAndTorqueCallback (beat,PhysicsApplyGravityForce);

		// calculate a accurate moment of inertia
		dVector origin;
		dVector inertia;
		NewtonConvexCollisionCalculateInertialMatrix (collision, &inertia[0], &origin[0]);	

		mass = 5.0f;
		Ixx = mass * inertia[0];
		Iyy = mass * inertia[1];
		Izz = mass * inertia[2];


		// set the mass matrix
		NewtonBodySetMassMatrix (beat, mass, Ixx, Iyy, Izz);

		// set the matrix for both the rigid body and the graphic body
		NewtonBodySetMatrix (beat, &beatLocation[0][0]);
		PhysicsSetTransform (beat, &beatLocation[0][0]);

		// set the pivot relative for the first bar
		dVector pivot (beatLocation.m_posit); 
		dVector pin (beatLocation.m_front);
		joint = new CustomCorkScrew (pivot, pin, beat, bar);

		// calculate the minimum and maximum limit for this joints
		dFloat minLimits = ((location.m_posit.m_x - beatLocation.m_posit.m_x) - size.m_x * 0.5f);
		dFloat maxLimits = ((location.m_posit.m_x - beatLocation.m_posit.m_x) + size.m_x * 0.5f);

		joint->EnableLimits(true);
		joint->SetLimis (minLimits, maxLimits); 
	}


	{
		// ////////////////////////////////////////////////////////////////////////////////////
		//
		// add a universal joint visualObject with limits
		//
		// ////////////////////////////////////////////////////////////////////////////////////
		NewtonBody* beat;
		CustomUniversal* joint;
		RenderPrimitive* visualObject;
		dMatrix beatLocation (location);
		dVector beatSize (0.5f, 1.25f, 1.25f);

		beatLocation.m_posit.m_x -= size.m_x * 0.5f;

		// create the a graphic character (use a visualObject as our body
		//visualObject = new BoxPrimitive (beatLocation, beatSize);
		visualObject = new ChamferCylinderPrimitive (parent, beatLocation, beatSize.m_y, beatSize.m_x);

		// create a collision primitive to be shared by all links
		collision = NewtonCreateChamferCylinder (nWorld, beatSize.m_y, beatSize.m_x, NULL); 

		beat = NewtonCreateBody(nWorld, collision);
		NewtonReleaseCollision (nWorld, collision);

			// attach graphic object to the rigid body
		NewtonBodySetUserData(beat, visualObject);

		// set a destructor function
		NewtonBodySetDestructorCallback (beat, PhysicsBodyDestructor);

		// set the transform call back function
		NewtonBodySetTransformCallback (beat, PhysicsSetTransform);

		// set the force and torque call back function
		NewtonBodySetForceAndTorqueCallback (beat,PhysicsApplyGravityForce);

		// calculate a accurate moment of inertia
		dVector origin;
		dVector inertia;
		NewtonConvexCollisionCalculateInertialMatrix (collision, &inertia[0], &origin[0]);	

		mass = 5.0f;
		Ixx = mass * inertia[0];
		Iyy = mass * inertia[1];
		Izz = mass * inertia[2];

		// set the mass matrix
		NewtonBodySetMassMatrix (beat, mass, Ixx, Iyy, Izz);

		// set the matrix for both the rigid body and the graphic body
		NewtonBodySetMatrix (beat, &beatLocation[0][0]);
		PhysicsSetTransform (beat, &beatLocation[0][0]);

		// set the pivot relative for the first bar
		dVector pivot (beatLocation.m_posit); 
		dVector pin0 (beatLocation.m_front);
		dVector pin1 (beatLocation.m_up);
		// tell this joint to destroy its local private data when destroyed
		joint = new CustomUniversal (pivot, pin0, pin1, beat, bar);

		joint->EnableLimit_1 (false);
		joint->EnableMotor_1 (true);

	}


	{
		// ////////////////////////////////////////////////////////////////////////////////////
		//
		// add a universal joint visualObject with limits
		//
		// ////////////////////////////////////////////////////////////////////////////////////
		NewtonBody* beat;
		CustomUniversal* joint;
		RenderPrimitive* visualObject;
		dMatrix beatLocation (location);
		dVector beatSize (0.5f, 1.25f, 1.25f);

		beatLocation.m_posit.m_x += size.m_x * 0.5f;

		// create the a graphic character (use a visualObject as our body
		//visualObject = new BoxPrimitive (beatLocation, beatSize);
		visualObject = new ChamferCylinderPrimitive (parent, beatLocation, beatSize.m_y, beatSize.m_x);

		// create a collision primitive to be shared by all links
		collision = NewtonCreateChamferCylinder (nWorld, beatSize.m_y, beatSize.m_x, NULL); 

		beat = NewtonCreateBody(nWorld, collision);
		NewtonReleaseCollision (nWorld, collision);

		// attach graphic object to the rigid body
		NewtonBodySetUserData(beat, visualObject);

		// set a destructor function
		NewtonBodySetDestructorCallback (beat, PhysicsBodyDestructor);

		// set the transform call back function
		NewtonBodySetTransformCallback (beat, PhysicsSetTransform);

		// set the force and torque call back function
		NewtonBodySetForceAndTorqueCallback (beat,PhysicsApplyGravityForce);

		// calculate a accurate moment of inertia
		dVector origin;
		dVector inertia;
		NewtonConvexCollisionCalculateInertialMatrix (collision, &inertia[0], &origin[0]);	

		mass = 5.0f;
		Ixx = mass * inertia[0];
		Iyy = mass * inertia[1];
		Izz = mass * inertia[2];


		// set the mass matrix
		NewtonBodySetMassMatrix (beat, mass, Ixx, Iyy, Izz);

		// set the matrix for both the rigid body and the graphic body
		NewtonBodySetMatrix (beat, &beatLocation[0][0]);
		PhysicsSetTransform (beat, &beatLocation[0][0]);

		// set the pivot relative for the first bar
		dVector pivot (beatLocation.m_posit); 
		dVector pin0 (beatLocation.m_front.Scale(-1.0f));
		dVector pin1 (beatLocation.m_up);
		// tell this joint to destroy its local private data when destroyed
		joint = new CustomUniversal (pivot, pin0, pin1, beat, bar);

		joint->EnableLimit_0 (false);
		joint->EnableMotor_0 (true);
	}


	{
		// ////////////////////////////////////////////////////////////////////////////////////
		//
		// add a sliding visualObject with limits
		//
		NewtonBody* beat;
		CustomSlider* joint;
		RenderPrimitive* visualObject;
		dMatrix beatLocation (location);
		dVector beatSize (0.5f, 2.0f, 2.0f);

		beatLocation.m_posit.m_x += size.m_x * 0.25f;

		// create the a graphic character (use a visualObject as our body)
		visualObject = new BoxPrimitive (parent, beatLocation, beatSize);

		// create a collision primitive to be shared by all links
		collision = NewtonCreateBox (nWorld, beatSize.m_x, beatSize.m_y, beatSize.m_z, NULL); 

		beat = NewtonCreateBody(nWorld, collision);
		NewtonReleaseCollision (nWorld, collision);

			// attach graphic object to the rigid body
		NewtonBodySetUserData(beat, visualObject);

		// set a destructor function
		NewtonBodySetDestructorCallback (beat, PhysicsBodyDestructor);

		// set the transform call back function
		NewtonBodySetTransformCallback (beat, PhysicsSetTransform);

		// set the force and torque call back function
		NewtonBodySetForceAndTorqueCallback (beat,PhysicsApplyGravityForce);

		// calculate a accurate moment of inertia
		dVector origin;
		dVector inertia;
		NewtonConvexCollisionCalculateInertialMatrix (collision, &inertia[0], &origin[0]);	

		mass = 5.0f;
		Ixx = mass * inertia[0];
		Iyy = mass * inertia[1];
		Izz = mass * inertia[2];


		// set the mass matrix
		NewtonBodySetMassMatrix (beat, mass, Ixx, Iyy, Izz);

		// set the matrix for both the rigid body and the graphic body
		NewtonBodySetMatrix (beat, &beatLocation[0][0]);
		PhysicsSetTransform (beat, &beatLocation[0][0]);

		// set the pivot relative for the first bar
		dVector pivot (beatLocation.m_posit); 
		dVector pin (beatLocation.m_front);
		joint = new CustomSlider (pivot, pin, beat, bar);

		// calculate the minimum and maximum limit for this joints
		dFloat minLimits = ((location.m_posit.m_x - beatLocation.m_posit.m_x) - size.m_x * 0.5f);
		dFloat maxLimits = ((location.m_posit.m_x - beatLocation.m_posit.m_x) + size.m_x * 0.5f);

		joint->EnableLimits(true);
		joint->SetLimis (minLimits, maxLimits); 
	}

}


void AddDoubleSwingDoors (dSceneNode* parent, NewtonWorld* nWorld)
{
	dFloat mass;
	dFloat Ixx;
	dFloat Iyy;
	dFloat Izz;
	NewtonBody* link0;
	NewtonBody* link1;
	CustomHinge* joint;
	BoxPrimitive* visualObject;
	NewtonCollision* collision;

	dVector size (2.0f, 5.0f, 0.5f);

	// create 100 tack of 10 boxes each
	dMatrix location (GetIdentityMatrix());
	location.m_posit.m_x = -4.0f; 
	location.m_posit.m_y =  0.0f; 
	location.m_posit.m_z = -2.0f; 

	// create a collision primitive to be shared by all links
	collision = NewtonCreateBox (nWorld, size.m_x, size.m_y, size.m_z, NULL); 

	// calculate a accurate moment of inertia
	dVector origin;
	dVector inertia;
	NewtonConvexCollisionCalculateInertialMatrix (collision, &inertia[0], &origin[0]);	

	mass = 5.0f;
	Ixx = mass * inertia[0];
	Iyy = mass * inertia[1];
	Izz = mass * inertia[2];

	// make first wing
	{
		// create the a graphic character (use a visualObject as our body
		visualObject = new BoxPrimitive (parent, location, size);

		//create the rigid body
		link1 = NewtonCreateBody (nWorld, collision);

		// Set Material Id for this object
		NewtonBodySetMaterialGroupID (link1, woodID);

		// save the pointer to the graphic object with the body.
		NewtonBodySetUserData (link1, visualObject);

		// set a destructor for this rigid body
		NewtonBodySetDestructorCallback (link1, PhysicsBodyDestructor);

		// set the transform call back function
		NewtonBodySetTransformCallback (link1, PhysicsSetTransform);

		// set the force and torque call back function
		NewtonBodySetForceAndTorqueCallback (link1,PhysicsApplyGravityForce);

		// set the mass matrix
		NewtonBodySetMassMatrix (link1, mass, Ixx, Iyy, Izz);

		// set the matrix for both the rigid body and the graphic body
		NewtonBodySetMatrix (link1, &location[0][0]);
		PhysicsSetTransform (link1, &location[0][0]);

		dVector pivot (location.m_posit);
		dVector pin (location.m_up);
		pivot.m_x += size.m_x * 0.5f;

		// connect these two bodies by a ball and socket joint
		joint = new CustomHinge (pivot, pin, link1, NULL);

		joint->EnableLimits (true);
		joint->SetLimis (-30.0f * 3.1416f/180.0f, 30.0f * 3.1416f/180.0f); 
	}


	// make second wing
	{

		location.m_posit.m_x -= size.m_x;

		// create the a graphic character (use a visualObject as our body
		visualObject = new BoxPrimitive (parent, location, size);

		//create the rigid body
		link0 = NewtonCreateBody (nWorld, collision);

		// Set Material Id for this object
		NewtonBodySetMaterialGroupID (link0, woodID);

		// save the pointer to the graphic object with the body.
		NewtonBodySetUserData (link0, visualObject);

		// set a destructor for this rigid body
		NewtonBodySetDestructorCallback (link0, PhysicsBodyDestructor);

		// set the transform call back function
		NewtonBodySetTransformCallback (link0, PhysicsSetTransform);

		// set the force and torque call back function
		NewtonBodySetForceAndTorqueCallback (link0,PhysicsApplyGravityForce);

		// set the mass matrix
		NewtonBodySetMassMatrix (link0, mass, Ixx, Iyy, Izz);

		// set the matrix for both the rigid body and the graphic body
		NewtonBodySetMatrix (link0, &location[0][0]);
		PhysicsSetTransform (link0, &location[0][0]);

		dVector pivot (location.m_posit);
		dVector pin (location.m_up);
		pivot.m_x += size.m_x * 0.5f;

		// connect these two bodies by a ball and socket joint
		//joint = NewtonConstraintCreateHinge (nWorld, &pivot.m_x, &pin.m_x, link0, link1);
		joint = new CustomHinge (pivot, pin, link0, link1);

		joint->EnableLimits (true);
		joint->SetLimis (-30.0f * 3.1416f/180.0f, 30.0f * 3.1416f/180.0f); 
	}

	// release the collision geometry when not need it
	NewtonReleaseCollision (nWorld, collision);
}


void AddGears (dSceneNode* parent, NewtonWorld* nWorld)
{
	dFloat Ixx;
	dFloat Iyy;
	dFloat Izz;
	dFloat mass;
	dFloat witdh;
	dFloat radius;
	dFloat smallRadius;
	NewtonBody* maryPopy;
	NewtonBody* centerCicle;
	NewtonCollision* collision;
	
	witdh = 0.25f;
	radius = 4.0f;
	smallRadius = radius * 0.5f; 
	dMatrix location (dgRollMatrix(3.1416f * 0.5f));
	location.m_posit = dVector (6.0f, -2.0f, 10.0f);
	{
		// ////////////////////////////////////////////////////////////////////////////////////
		//
		// add a big cylinder fixed the wold by a hinge
		//
		// ////////////////////////////////////////////////////////////////////////////////////
		
		CustomHinge* joint;
		RenderPrimitive* visualObject;
		dMatrix beatLocation (location);

		// create the a graphic character (use a visualObject as our body
		//visualObject = new BoxPrimitive (beatLocation, beatSize);
		visualObject = new ChamferCylinderPrimitive (parent, beatLocation, radius, witdh);

		// create a collision primitive to be shared by all links
		collision = NewtonCreateChamferCylinder (nWorld, radius, witdh, NULL); 

		maryPopy = NewtonCreateBody(nWorld, collision);
		NewtonReleaseCollision (nWorld, collision);

		// attach graphic object to the rigid body
		NewtonBodySetUserData(maryPopy, visualObject);

		// set a destructor function
		NewtonBodySetDestructorCallback (maryPopy, PhysicsBodyDestructor);

		// set the transform call back function
		NewtonBodySetTransformCallback (maryPopy, PhysicsSetTransform);

		// set the force and torque call back function
		NewtonBodySetForceAndTorqueCallback (maryPopy,PhysicsApplyGravityForce);

		// calculate a accurate moment of inertia
		dVector origin;
		dVector inertia;
		NewtonConvexCollisionCalculateInertialMatrix (collision, &inertia[0], &origin[0]);	

		mass = 5.0f;
		Ixx = mass * inertia[0];
		Iyy = mass * inertia[1];
		Izz = mass * inertia[2];


		// set the mass matrix
		NewtonBodySetMassMatrix (maryPopy, mass, Ixx, Iyy, Izz);

		// set the matrix for both the rigid body and the graphic body
		NewtonBodySetMatrix (maryPopy, &beatLocation[0][0]);
		PhysicsSetTransform (maryPopy, &beatLocation[0][0]);


		// set the pivot relative for the first bar
		dVector pivot (beatLocation.m_posit); 
		dVector pin (beatLocation.m_front);
		joint = new CustomHinge (pivot, pin, maryPopy, NULL);
	}


	{
		// ////////////////////////////////////////////////////////////////////////////////////
		//
		// add handle satellite 
		//
		// ////////////////////////////////////////////////////////////////////////////////////
		
		NewtonCustomJoint* joint;
		RenderPrimitive* visualObject;
		dMatrix beatLocation (GetIdentityMatrix());
		beatLocation.m_posit = location.m_posit;
		beatLocation.m_posit.m_z += 8.0f;
		beatLocation.m_posit.m_y += 1.5f;

		// create the a graphic character (use a visualObject as our body
		//visualObject = new BoxPrimitive (beatLocation, beatSize);
		visualObject = new ChamferCylinderPrimitive (parent, beatLocation, smallRadius, witdh);

		// create a collision primitive to be shared by all links
		collision = NewtonCreateChamferCylinder (nWorld, smallRadius, witdh, NULL); 

		centerCicle = NewtonCreateBody(nWorld, collision);
		NewtonReleaseCollision (nWorld, collision);

		// attach graphic object to the rigid body
		NewtonBodySetUserData(centerCicle, visualObject);

		// set a destructor function
		NewtonBodySetDestructorCallback (centerCicle, PhysicsBodyDestructor);

		// set the transform call back function
		NewtonBodySetTransformCallback (centerCicle, PhysicsSetTransform);

		// set the force and torque call back function
		NewtonBodySetForceAndTorqueCallback (centerCicle,PhysicsApplyGravityForce);

		// calculate a accurate moment of inertia
		dVector origin;
		dVector inertia;
		NewtonConvexCollisionCalculateInertialMatrix (collision, &inertia[0], &origin[0]);	

		mass = 5.0f * 0.5f;
		Ixx = mass * inertia[0];
		Iyy = mass * inertia[1];
		Izz = mass * inertia[2];
	
		// set the mass matrix
		NewtonBodySetMassMatrix (centerCicle, mass, Ixx, Iyy, Izz);

		// set the matrix for both the rigid body and the graphic body
		NewtonBodySetMatrix (centerCicle, &beatLocation[0][0]);
		PhysicsSetTransform (centerCicle, &beatLocation[0][0]);


		// set the pivot relative for the first bar
		dVector pivot (beatLocation.m_posit); 
		dVector pin (beatLocation.m_front);
		joint = new CustomHinge (pivot, pin, centerCicle, NULL);

		// connect the popy and the control gear
		joint = new CustomGear (2.0f, pin, location.m_front, centerCicle, maryPopy);
  	}


	{
		// ////////////////////////////////////////////////////////////////////////////////////
		//
		// add a the center cylinder
		//
		// ////////////////////////////////////////////////////////////////////////////////////
		
		NewtonCustomJoint* joint;
		RenderPrimitive* visualObject;
		dMatrix beatLocation (location);
		beatLocation.m_posit.m_y += witdh;

		// create the a graphic character (use a visualObject as our body
		//visualObject = new BoxPrimitive (beatLocation, beatSize);
		visualObject = new ChamferCylinderPrimitive (parent, beatLocation, smallRadius, witdh);

		// create a collision primitive to be shared by all links
		collision = NewtonCreateChamferCylinder (nWorld, smallRadius, witdh, NULL); 

		centerCicle = NewtonCreateBody(nWorld, collision);
		NewtonReleaseCollision (nWorld, collision);

		// attach graphic object to the rigid body
		NewtonBodySetUserData(centerCicle, visualObject);

		// set a destructor function
		NewtonBodySetDestructorCallback (centerCicle, PhysicsBodyDestructor);

		// set the transform call back function
		NewtonBodySetTransformCallback (centerCicle, PhysicsSetTransform);

		// set the force and torque call back function
		NewtonBodySetForceAndTorqueCallback (centerCicle,PhysicsApplyGravityForce);

		// calculate a accurate moment of inertia
		dVector origin;
		dVector inertia;
		NewtonConvexCollisionCalculateInertialMatrix (collision, &inertia[0], &origin[0]);	

		mass = 5.0f * 0.5f;
		Ixx = mass * inertia[0];
		Iyy = mass * inertia[1];
		Izz = mass * inertia[2];
	
		// set the mass matrix
		NewtonBodySetMassMatrix (centerCicle, mass, Ixx, Iyy, Izz);

		// set the matrix for both the rigid body and the graphic body
		NewtonBodySetMatrix (centerCicle, &beatLocation[0][0]);
		PhysicsSetTransform (centerCicle, &beatLocation[0][0]);


		// set the pivot relative for the first bar
		dVector pivot (beatLocation.m_posit); 
		dVector pin (beatLocation.m_front);
		joint = new CustomHinge (pivot, pin, centerCicle, maryPopy);


		// connect the marypopy and the center circle with a gear joint
		joint = new CustomGear (-2.0f * radius / smallRadius, pin, location.m_front, centerCicle, maryPopy);
  	}


	const int satelliteCount = 5;
	for (int i = 0; i < satelliteCount; i ++)
	{
		// ////////////////////////////////////////////////////////////////////////////////////
		//
		// add a satellite disk
		//
		// ////////////////////////////////////////////////////////////////////////////////////
		
		NewtonBody* sattelite;
		NewtonCustomJoint* joint;
		RenderPrimitive* visualObject;
		dMatrix beatLocation (location);

		dMatrix rot (dgYawMatrix((dFloat(i) / satelliteCount) * (2.0f * 3.1416f)));
		beatLocation.m_posit += rot.RotateVector(dVector (smallRadius * 2.0f, 0.0f, 0.0f));
		beatLocation.m_posit.m_y += witdh;

		// create the a graphic character (use a visualObject as our body
		//visualObject = new BoxPrimitive (beatLocation, beatSize);
		visualObject = new ChamferCylinderPrimitive (parent, beatLocation, smallRadius, witdh);

		// create a collision primitive to be shared by all links
		collision = NewtonCreateChamferCylinder (nWorld, smallRadius, witdh, NULL); 

		sattelite = NewtonCreateBody(nWorld, collision);
		NewtonReleaseCollision (nWorld, collision);

		// attach graphic object to the rigid body
		NewtonBodySetUserData(sattelite, visualObject);

		// set a destructor function
		NewtonBodySetDestructorCallback (sattelite, PhysicsBodyDestructor);

		// set the transform call back function
		NewtonBodySetTransformCallback (sattelite, PhysicsSetTransform);

		// set the force and torque call back function
		NewtonBodySetForceAndTorqueCallback (sattelite, PhysicsApplyGravityForce);

		// calculate a accurate moment of inertia
		dVector origin;
		dVector inertia;
		NewtonConvexCollisionCalculateInertialMatrix (collision, &inertia[0], &origin[0]);	

		mass = 5.0f * 0.5f;
		Ixx = mass * inertia[0];
		Iyy = mass * inertia[1];
		Izz = mass * inertia[2];


		// set the mass matrix
		NewtonBodySetMassMatrix (sattelite, mass, Ixx, Iyy, Izz);

		// set the matrix for both the rigid body and the graphic body
		NewtonBodySetMatrix (sattelite, &beatLocation[0][0]);
		PhysicsSetTransform (sattelite, &beatLocation[0][0]);


		// set the pivot relative for the first bar
		dVector pivot (beatLocation.m_posit); 
		dVector pin (beatLocation.m_front);
		joint = new CustomHinge (pivot, pin, sattelite, maryPopy);

		// connect the satellite and the center circle with a gear joint
		joint = new CustomGear (smallRadius / smallRadius, pin, location.m_front, sattelite, centerCicle);
  	}
 }



void AddVise (dSceneNode* parent, NewtonWorld* nWorld)
{
	dFloat Ixx;
	dFloat Iyy;
	dFloat Izz;
	dFloat mass;
	NewtonBody* viseBody;
	NewtonBody* viseHandle;
	NewtonBody* viserRightClaw;
	NewtonBody* viserLeftClaw;
	NewtonCollision* collision;
	
	dVector size (2, 3, 8);
	dMatrix location (GetIdentityMatrix());
	location.m_posit = dVector (5.0f, -1.0f, -2.0f);
	{
		// ////////////////////////////////////////////////////////////////////////////////////
		//
		// add a big block for the base of the vise
		//
		// ////////////////////////////////////////////////////////////////////////////////////
		
		CustomHinge* joint;
		RenderPrimitive* visualObject;
		dMatrix beatLocation (location);

		// create the a graphic character (use a visualObject as our body
		visualObject = new BoxPrimitive (parent, beatLocation, size);

		// create a collision primitive to be shared by all links
		collision = NewtonCreateBox (nWorld, size.m_x, size.m_y, size.m_z, NULL); 

		viseBody = NewtonCreateBody(nWorld, collision);
		NewtonReleaseCollision (nWorld, collision);

		// attach graphic object to the rigid body
		NewtonBodySetUserData(viseBody, visualObject);

		// set a destructor function
		NewtonBodySetDestructorCallback (viseBody, PhysicsBodyDestructor);

		// set the transform call back function
		NewtonBodySetTransformCallback (viseBody, PhysicsSetTransform);

		// set the force and torque call back function
		NewtonBodySetForceAndTorqueCallback (viseBody,PhysicsApplyGravityForce);

		// calculate a accurate moment of inertia
		dVector origin;
		dVector inertia;
		NewtonConvexCollisionCalculateInertialMatrix (collision, &inertia[0], &origin[0]);	

		mass = 5.0f;
		Ixx = mass * inertia[0];
		Iyy = mass * inertia[1];
		Izz = mass * inertia[2];


		// set the mass matrix
		NewtonBodySetMassMatrix (viseBody, mass, Ixx, Iyy, Izz);

		// set the matrix for both the rigid body and the graphic body
		NewtonBodySetMatrix (viseBody, &beatLocation[0][0]);
		PhysicsSetTransform (viseBody, &beatLocation[0][0]);

		// set the pivot relative for the first bar
		dVector pivot (beatLocation.m_posit); 
		dVector pin (beatLocation.m_up);
		joint = new CustomHinge (pivot, pin, viseBody, NULL);

	}


	{
		// ////////////////////////////////////////////////////////////////////////////////////
		//
		// add vise handle  
		//
		// ////////////////////////////////////////////////////////////////////////////////////
		NewtonCustomJoint* joint;
		RenderPrimitive* visualObject;
		dMatrix beatLocation (GetIdentityMatrix());

		dFloat radius = size.m_y * 0.5f;
		dFloat width = size.m_x * 0.5f;

		beatLocation.m_posit = location.m_posit;
		beatLocation.m_posit.m_x -= ((size.m_x * 0.5f) + (width * 0.6f));


		// create the a graphic character (use a visualObject as our body
		//visualObject = new BoxPrimitive (beatLocation, beatSize);
		visualObject = new ChamferCylinderPrimitive (parent, beatLocation, radius, width);

		// create a collision primitive to be shared by all links
		collision = NewtonCreateChamferCylinder (nWorld, radius, width, NULL); 

		viseHandle = NewtonCreateBody(nWorld, collision);
		NewtonReleaseCollision (nWorld, collision);

		// attach graphic object to the rigid body
		NewtonBodySetUserData(viseHandle, visualObject);

		// set a destructor function
		NewtonBodySetDestructorCallback (viseHandle, PhysicsBodyDestructor);

		// set the transform call back function
		NewtonBodySetTransformCallback (viseHandle, PhysicsSetTransform);

		// set the force and torque call back function
		NewtonBodySetForceAndTorqueCallback (viseHandle,PhysicsApplyGravityForce);

		// calculate a accurate moment of inertia
		dVector origin;
		dVector inertia;
		NewtonConvexCollisionCalculateInertialMatrix (collision, &inertia[0], &origin[0]);	

		mass = 5.0f * 0.5f;
		Ixx = mass * inertia[0];
		Iyy = mass * inertia[1];
		Izz = mass * inertia[2];
	
		// set the mass matrix
		NewtonBodySetMassMatrix (viseHandle, mass, Ixx, Iyy, Izz);

		// set the matrix for both the rigid body and the graphic body
		NewtonBodySetMatrix (viseHandle, &beatLocation[0][0]);
		PhysicsSetTransform (viseHandle, &beatLocation[0][0]);


		// set the pivot relative for the first bar
		dVector pivot (beatLocation.m_posit); 
		dVector pin (beatLocation.m_front);
		joint = new CustomHinge (pivot, pin, viseHandle, viseBody);
  	}

	{
		// ////////////////////////////////////////////////////////////////////////////////////
		//
		// add right vise side
		//
		// ////////////////////////////////////////////////////////////////////////////////////
		NewtonCustomJoint* joint;
		RenderPrimitive* visualObject;
		dMatrix beatLocation (location);
		dVector viseSize (4.0f, 2.0f, 0.5f);
		beatLocation.m_posit.m_y += (size.m_y * 0.5f + viseSize.m_y * 0.5f);
		beatLocation.m_posit.m_z += (size.m_z * 0.5f - viseSize.m_z * 0.5f); 

		// create the a graphic character (use a visualObject as our body
		visualObject = new BoxPrimitive (parent, beatLocation, viseSize);

		// create a collision primitive to be shared by all links
		collision = NewtonCreateBox (nWorld, viseSize.m_x, viseSize.m_y, viseSize.m_z, NULL); 

		viserRightClaw = NewtonCreateBody(nWorld, collision);
		NewtonReleaseCollision (nWorld, collision);

		// attach graphic object to the rigid body
		NewtonBodySetUserData(viserRightClaw, visualObject);

		// set a destructor function
		NewtonBodySetDestructorCallback (viserRightClaw, PhysicsBodyDestructor);

		// set the transform call back function
		NewtonBodySetTransformCallback (viserRightClaw, PhysicsSetTransform);

		// set the force and torque call back function
		NewtonBodySetForceAndTorqueCallback (viserRightClaw,PhysicsApplyGravityForce);

		// calculate a accurate moment of inertia
		dVector origin;
		dVector inertia;
		NewtonConvexCollisionCalculateInertialMatrix (collision, &inertia[0], &origin[0]);	

		mass = 5.0f * 0.5f;
		Ixx = mass * inertia[0];
		Iyy = mass * inertia[1];
		Izz = mass * inertia[2];
	
		// set the mass matrix
		NewtonBodySetMassMatrix (viserRightClaw, mass, Ixx, Iyy, Izz);

		// set the matrix for both the rigid body and the graphic body
		NewtonBodySetMatrix (viserRightClaw, &beatLocation[0][0]);
		PhysicsSetTransform (viserRightClaw, &beatLocation[0][0]);


		// set the pivot relative for the first bar
		dVector pivot (beatLocation.m_posit); 
		dVector pin (beatLocation.m_right);

		CustomSlider* slider;
		slider = new CustomSlider (pivot, pin, viserRightClaw, viseBody);
		slider->EnableLimits(true);
		slider->SetLimis(-(size.m_z - viseSize.m_z * 2.5f) * 0.5f, 0.0f);

  		// connect the vise handle with right claw
		dMatrix handleMatrix;
		NewtonBodyGetMatrix(viseHandle, &handleMatrix[0][0]);
		joint = new CustomWormGear (4.0f, handleMatrix.m_front, location.m_right, viseHandle, viserRightClaw);
  	}


	{
		// ////////////////////////////////////////////////////////////////////////////////////
		//
		// add left vise side
		//
		// ////////////////////////////////////////////////////////////////////////////////////
		NewtonCustomJoint* joint;
		RenderPrimitive* visualObject;
		dMatrix beatLocation (location);
		dVector viseSize (4.0f, 2.0f, 0.5f);
		beatLocation.m_posit.m_y += (size.m_y * 0.5f + viseSize.m_y * 0.5f);
		beatLocation.m_posit.m_z -= (size.m_z * 0.5f - viseSize.m_z * 0.5f); 

		// create the a graphic character (use a visualObject as our body
		visualObject = new BoxPrimitive (parent, beatLocation, viseSize);

		// create a collision primitive to be shared by all links
		collision = NewtonCreateBox (nWorld, viseSize.m_x, viseSize.m_y, viseSize.m_z, NULL); 

		viserLeftClaw = NewtonCreateBody(nWorld, collision);
		NewtonReleaseCollision (nWorld, collision);

		// attach graphic object to the rigid body
		NewtonBodySetUserData(viserLeftClaw, visualObject);

		// set a destructor function
		NewtonBodySetDestructorCallback (viserLeftClaw, PhysicsBodyDestructor);

		// set the transform call back function
		NewtonBodySetTransformCallback (viserLeftClaw, PhysicsSetTransform);

		// set the force and torque call back function
		NewtonBodySetForceAndTorqueCallback (viserLeftClaw,PhysicsApplyGravityForce);

		// calculate a accurate moment of inertia
		dVector origin;
		dVector inertia;
		NewtonConvexCollisionCalculateInertialMatrix (collision, &inertia[0], &origin[0]);	

		mass = 5.0f * 0.5f;
		Ixx = mass * inertia[0];
		Iyy = mass * inertia[1];
		Izz = mass * inertia[2];
	
		// set the mass matrix
		NewtonBodySetMassMatrix (viserLeftClaw, mass, Ixx, Iyy, Izz);

		// set the matrix for both the rigid body and the graphic body
		NewtonBodySetMatrix (viserLeftClaw, &beatLocation[0][0]);
		PhysicsSetTransform (viserLeftClaw, &beatLocation[0][0]);


		// set the pivot relative for the first bar
		dVector pivot (beatLocation.m_posit); 
		dVector pin (beatLocation.m_right);

		CustomSlider* slider;
		slider = new CustomSlider (pivot, pin, viserLeftClaw, viseBody);
		slider->EnableLimits(true);
		slider->SetLimis(0.0f, size.m_z * 0.5f);

  		// connect the tow arm claw with a pulley gear
		dMatrix viserRightClawMatrix;
		NewtonBodyGetMatrix(viserRightClaw, &viserRightClawMatrix[0][0]);
		joint = new CustomPulley (1.0f, viserRightClawMatrix.m_right, location.m_right, viserRightClaw, viserLeftClaw);
  	}
 }
