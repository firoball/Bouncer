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
#include "tutorial.h"
#include "Materials.h"
#include "CustomUpVector.h"
#include "CharaterControl.h"


extern bool animateUpVector;

#define JUMP_TIMER	4
//#define USE_TRANFORM_MODIFIER

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CharacterController::CharacterController (dSceneNode* parent, NewtonWorld* nWorld, const dVector& position, const dVector& size)
	:SpherePrimitive (parent, GetIdentityMatrix(), size.m_x * 0.5f, size.m_y * 0.5f, size.m_z * 0.5f), 
	m_size (size.Scale (0.5f)),	m_forceVector (0.0f, 0.0f, 0.0f)
{							    
	NewtonCollision* sphere;


	// cache the world
	m_myWorld = nWorld;

	// assume the character is on the air
	m_isAirBorne = true;
	m_isStopped = true;
	m_jumpTimer = 0;

	// set animation mode false
	m_yawAngle = 0.0f;
	

	m_stepContact = dVector (0.0f, -m_size.m_y, 0.0f);   
	m_maxStepHigh = -m_size.m_y * 0.5f;

	// calculate the character ellipse radius
	dVector radius (size.Scale (0.5f));

	// scale the Max translation before considering tunneling prevention 
	m_maxTranslation = size.m_x * 0.25f;

	// create 100 tack of 10 boxes each
	dMatrix location (GetIdentityMatrix());

	location.m_posit.m_x = position.m_x;
	location.m_posit.m_z = position.m_z;
	location.m_posit.m_y = FindFloor (nWorld, position, 100) + radius.m_y;

	// place a sphere at the center
	sphere = NewtonCreateSphere (nWorld, radius.m_x, radius.m_y, radius.m_z, NULL); 

#ifdef USE_TRANFORM_MODIFIER
	// wrap the container collision under a transform, modifier for tunneling trough walls avoidance
	m_myCollision = NewtonCreateConvexHullModifier (nWorld, sphere);
	NewtonReleaseCollision (nWorld, sphere);		
#else
	m_myCollision = sphere;
#endif

	//create the rigid body
	m_myBody = NewtonCreateBody (nWorld, m_myCollision);

	// disable auto freeze management for the player
	NewtonBodySetAutoFreeze (m_myBody, 0);

	// keep the player always active 
	NewtonWorldUnfreezeBody (nWorld, m_myBody);

	dFloat damp[3];
	damp[0] = 0.0f;
	damp[1] = 0.0f;
	damp[2] = 0.0f;
	// set the viscous damping the the minimum
	NewtonBodySetLinearDamping (m_myBody, 0.0f);
	NewtonBodySetAngularDamping (m_myBody, damp);

	// Set Material Id for this object
	NewtonBodySetMaterialGroupID (m_myBody, characterID);

	// save the pointer to the graphic object with the body.
	NewtonBodySetUserData (m_myBody, this);

	// set a destructor for this rigid body
	NewtonBodySetDestructorCallback (m_myBody, PhysicsBodyDestructor);

	// set the transform call back function
	NewtonBodySetTransformCallback (m_myBody, PhysicsSetTransform);

	// set the force and torque call back function
	NewtonBodySetForceAndTorqueCallback (m_myBody, OnApplyForceAndTorque);

	// set the mass matrix
	NewtonBodySetMassMatrix (m_myBody, 10.0f, 10.0f, 10.0f, 10.0f);

	// set the matrix for both the rigid body and the graphic body
	NewtonBodySetMatrix (m_myBody, &location[0][0]);
	PhysicsSetTransform (m_myBody, &location[0][0]);

  	// add and up vector constraint to help in keeping the body upright
	m_upVector = new CustomUpVector (dVector (0.0f, 1.0f, 0.0f), m_myBody); 


	// release the collision geometry when not need it
	NewtonReleaseCollision (nWorld, m_myCollision);
}


CharacterController::~CharacterController ()
{
}

void CharacterController::Render() const
{
	SpherePrimitive::Render();
}



struct CharacterRayCastData
{
	NewtonBody* m_me;
	dFloat m_parameter;

	CharacterRayCastData (NewtonBody* me)
	{
		m_me = me;
		m_parameter = 1.2f;
	}
};

// callback to  find dFloor for character placement
dFloat  CharacterController::OnRayCastPlacement (const NewtonBody* body, const dFloat* normal, int collisionID, void* userData, dFloat intersetParam)
{
	dFloat paramPtr;
	CharacterRayCastData& data = *((CharacterRayCastData*) userData);

	// get the graphic object from the rigid body

	paramPtr = 1.2f;
	// any body can be a dFloor
	if (data.m_me != body) {
		if (intersetParam < data.m_parameter) {
			data.m_parameter = intersetParam;
			paramPtr = intersetParam;
		}
	}
	// else continue the search
	return paramPtr;
}


// find dFloor for character placement
dFloat CharacterController::FindFloor (NewtonWorld* world, const dVector& p0, dFloat maxDist)
{
	CharacterRayCastData data (m_myBody);
	
	// shot a vertical ray from a high altitude and collect the intersection parameter.
	dVector p1 (p0); 
	p1.m_y -= maxDist;

	NewtonWorldRayCast (world, &p0[0], &p1[0], OnRayCastPlacement, &data, NULL);
//	_ASSERTE (data.m_parameter < 1.0f);

	// the intersection is the interpolated value
	return p0.m_y - maxDist * data.m_parameter;
}

// Apply force callback
void CharacterController::OnApplyForceAndTorque (const NewtonBody* body)
{
	CharacterController* player;

	// get the graphic object form the rigid body
	player = (CharacterController*) NewtonBodyGetUserData (body);

	player->OnApplyForceAndTorque ();
}

void CharacterController::SetForce (const dVector& force)
{
	m_forceVector = force;
	m_isStopped = (m_forceVector.m_x == 0.0f) && (m_forceVector.m_z == 0.0f);
	if (m_forceVector.m_y != 0.0f) {
		m_jumpTimer = 4;
	}
}


// this function will analyze the contacts and determine what action is to be taken
void CharacterController::OnCollisionWithLevel(const NewtonMaterial* material, const NewtonContact* contact)
{
	dVector point;
	dVector normal;
	dVector velocity;

	// Get the collision and normal
	NewtonMaterialGetContactPositionAndNormal (material, &point.m_x, &normal.m_x);

	dVector localPoint (m_matrix.UntransformVector (point));

	// if a contact is below the max need consider the character is on the ground
	if (localPoint.m_y < m_maxStepHigh) {
		m_isAirBorne = false;
			
		NewtonBodyGetVelocity(m_myBody, &velocity.m_x);

		// calculate ball velocity perpendicular to the contact normal
		dVector tangentVelocity (velocity - normal.Scale (normal % velocity));

		// align the tangent at the contact point with the tangent velocity vector of the ball
		NewtonMaterialContactRotateTangentDirections (material, &tangentVelocity.m_x);

		// we do do want bound back we hitting the dFloor
		NewtonMaterialSetContactElasticity (material, 0.3f);
	
		// save the elevation of the highest step to take
		if (localPoint.m_y > m_stepContact.m_y) {
			if (dAbs (normal.m_y) < 0.8f) {
				m_stepContact = localPoint;   
			}
		}

		// if the player want to move set disable friction else set high ground friction so it can stop on slopes
		if (m_isStopped) {
			NewtonMaterialSetContactStaticFrictionCoef (material, 2.0f, 0);
			NewtonMaterialSetContactKineticFrictionCoef (material, 2.0f, 0);
			NewtonMaterialSetContactStaticFrictionCoef (material, 2.0f, 1);
			NewtonMaterialSetContactKineticFrictionCoef (material, 2.0f, 1);

		} else {
			NewtonMaterialSetContactFrictionState (material, 0, 0);
			NewtonMaterialSetContactFrictionState (material, 0, 1);
		}

	} else {
		//set contact above the max step to be friction less
		// disable fiction calculation for sphere collision
		NewtonMaterialSetContactFrictionState (material, 0, 0);
		NewtonMaterialSetContactFrictionState (material, 0, 1);
	}
}


// //////////////////////////////////////////////////////////////////////////////////////// 
// 
// this is the main character controller function.
// This is the strategy
// - the charter moment is constraint to an up vector joint
// - horizontal linear motion will be controlled by forces.
// - vertical movement is control kinematicas when the body is no in free fall
// - turning is controlled kinematics at all time
//
// - To prevent tunneling trough walls the collision geometry will be scaled by the translation distance when 
//   the character is moving a hight speed
void CharacterController::OnApplyForceAndTorque ()
{

	dFloat mass;
	dFloat Ixx;
	dFloat Iyy;
	dFloat Izz;
	dFloat dist;
	dFloat dFloor;
	dFloat accelY;
	dFloat deltaHeight;
	dFloat steerAngle;
	dFloat timestep;
	dFloat timestepInv;
	dVector force;
	dVector omega;
	dVector alpha;
	dVector heading;
	dVector velocity;
	dMatrix matrix;
	

	// Get the current world timestep
	timestep = NewtonGetTimeStep(m_myWorld);
	timestepInv = 1.0f / timestep;

	// get the character mass
	NewtonBodyGetMassMatrix (m_myBody, &mass, &Ixx, &Iyy, &Izz);

	// apply the gravity force, cheat a little with the character gravity
	force = dVector (0.0f, -mass * 20.0f, 0.0f);

	// Get the velocity vector
	NewtonBodyGetVelocity(m_myBody, &velocity.m_x);

	// determine if the character have to be snap to the found
	NewtonBodyGetMatrix(m_myBody, &matrix[0][0]);


	// if the dFloor is with in reach then the character must be snap to the ground
	// the max allow distance for snapping i 0.25 of a meter
	if (m_isAirBorne && !m_jumpTimer) { 
		dFloor = FindFloor (m_myWorld, matrix.m_posit, m_size.m_y + 0.25f);
		deltaHeight = (matrix.m_posit.m_y - m_size.m_y) - dFloor;
		if ((deltaHeight < (0.25f - 0.001f)) && (deltaHeight > 0.01f)) {
			// snap to dFloor only if the dFloor is lower than the character feet		
			accelY = - (deltaHeight * timestepInv + velocity.m_y) * timestepInv;
			force.m_y = mass * accelY;
		}
	} else if (m_jumpTimer == JUMP_TIMER) {
	   dVector veloc (0.0f, 7.5f, 0.0f);
	   NewtonAddBodyImpulse (m_myBody, &veloc[0], &matrix.m_posit[0]);
	}

	m_jumpTimer = m_jumpTimer ? m_jumpTimer - 1 : 0;
	
	// rotate the force direction to align with the camera
	heading = m_matrix.RotateVector (m_forceVector);
	heading = heading.Scale (1.0f / dSqrt ((heading % heading) + 1.0e-6f));

	force += (heading.Scale (mass * 30.0f) - heading.Scale (2.0f * (velocity % heading))); 
	NewtonBodySetForce (m_myBody, &force.m_x);

	// estimate the final horizontal translation for to next force and velocity
	dVector step ((force.Scale (timestep  / mass) + velocity).Scale (timestep));

	step = m_matrix.UnrotateVector (step);
	dMatrix collisionPaddingMatrix (GetIdentityMatrix());

	step.m_y = 0.0f;

	dist = step % step;
	if (dist > m_maxTranslation * m_maxTranslation) {
		// when the velocity is high enough that can miss collision we will enlarge the collision 
		// long the vector velocity
		dist = dSqrt (dist);
		step = step.Scale (1.0f / dist);

		//make a rotation matrix that will align the velocity vector with the front vector
		collisionPaddingMatrix[0][0] =  step[0];
		collisionPaddingMatrix[0][2] = -step[2];
		collisionPaddingMatrix[2][0] =  step[2];
		collisionPaddingMatrix[2][2] =  step[0];

		// get the transpose of the matrix
		dMatrix transp (collisionPaddingMatrix.Transpose());
		transp[0] = transp[0].Scale (dist/m_maxTranslation);

		// calculate a oblique scale matrix by using a similar transformation matrix of the for, R'* S * R
		collisionPaddingMatrix = collisionPaddingMatrix * transp;
	}

	// set the collision modifierMatrix;
	NewtonConvexHullModifierSetMatrix (m_myCollision, &collisionPaddingMatrix[0][0]);

	// calculate the torque vector
	steerAngle = min (max ((m_matrix.m_front * cameraDir).m_y, -1.0f), 1.0f);
	steerAngle = dAsin (steerAngle); 
	NewtonBodyGetOmega(m_myBody, &omega.m_x);

	dVector torque (0.0f, 0.5f * Iyy * (steerAngle * timestepInv - omega.m_y) * timestepInv, 0.0f);
//	torque = dVector (0.0f, Iyy * (20.0f * steerAngle - 5.0f * omega.m_y), 0.0f);  
	NewtonBodySetTorque (m_myBody, &torque.m_x);


	// assume the character is on the air. this variable will be set to false if the contact detect 
	//the character is landed 
	m_isAirBorne = true;
	m_stepContact = dVector (0.0f, -m_size.m_y, 0.0f);   

	dVector pin (0.0f, 1.0f, 0.0f);
	if (animateUpVector) {
		// animate the pin vector
		m_yawAngle = dMod (m_yawAngle + 3.0f * 3.1416f / 180.0f, 2.0f * 3.1416f);
//		m_yawAngle = 0.0f * 3.1416f / 180.0f;
		dMatrix animMatrix (dgYawMatrix(m_yawAngle));
		dVector dir (dSin (20.0f * 3.1416f / 180.0f), dCos (20.0f * 3.1416f / 180.0f), 0.0f);
//		dVector dir (dSin (45.0f * 3.1416f / 180.0f), dCos (45.0f * 3.1416f / 180.0f), 0.0f);
		pin = animMatrix.RotateVector (dir);
	}
	m_upVector->SetPinDir(pin);

}


