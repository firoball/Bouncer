// AstroChiken.cpp: implementation of the AstroChiken class.
//
//////////////////////////////////////////////////////////////////////
#include <stdafx.h>
#include "Materials.h"
#include "AstroChiken.h"
#include "CustomHinge.h"
#include "CustomUniversal.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define CHIKEN_LENGTH		2.5f
#define CHIKEN_HEIGHT		0.5f
#define CHIKEN_WIDTH		1.0f


#define MIN_JOINT_PIN_LENGTH 50.0f


class AstroChikenCalf: public CapsulePrimitive
{

	class ThighEngine: public CustomHinge
	{
		public:
		ThighEngine(AstroChikenCalf* me, const dVector& pivot, const dVector& pin, NewtonBody* child, NewtonBody* parent, dFloat side)
			:CustomHinge(pivot, pin, child, parent)
		{
			dFloat sinAngle;
			dFloat cosAngle;
			dMatrix matrix0;
			dMatrix matrix1;

			m_parent = me;
			// calculate the position of the pivot point and the Jacobian direction vectors, in global space. 
			CalculateGlobalMatrix (m_localMatrix0, m_localMatrix1, matrix0, matrix1);

			
			// the joint angle can be determine by getting the angle between any two non parallel vectors
			sinAngle = (matrix0.m_up * matrix1.m_up) % matrix0.m_front;
			cosAngle = matrix0.m_up % matrix1.m_up;
			m_baseAngle = dAtan2 (sinAngle, cosAngle) - side * 60.0f * 3.1416f / 180.0f;
		}

		void SubmitConstrainst ()
		{
			dFloat angle;
			dFloat sinAngle;
			dFloat cosAngle;
			dFloat relAngle;
			dMatrix matrix0;
			dMatrix matrix1;

			// calculate the position of the pivot point and the Jacobian direction vectors, in global space. 
			CalculateGlobalMatrix (m_localMatrix0, m_localMatrix1, matrix0, matrix1);

			// Restrict the movement on the pivot point along all tree orthonormal direction
			NewtonUserJointAddLinearRow (m_joint, &matrix0.m_posit[0], &matrix1.m_posit[0], &matrix0.m_front[0]);
			NewtonUserJointAddLinearRow (m_joint, &matrix0.m_posit[0], &matrix1.m_posit[0], &matrix0.m_up[0]);
			NewtonUserJointAddLinearRow (m_joint, &matrix0.m_posit[0], &matrix1.m_posit[0], &matrix0.m_right[0]);
			
			// get a point along the pin axis at some reasonable large distance from the pivot
			dVector q0 (matrix0.m_posit + matrix0.m_front.Scale(MIN_JOINT_PIN_LENGTH));
			dVector q1 (matrix1.m_posit + matrix1.m_front.Scale(MIN_JOINT_PIN_LENGTH));

			// two constraints row perpendicular to the pin vector
 			NewtonUserJointAddLinearRow (m_joint, &q0[0], &q1[0], &matrix0.m_up[0]);
			NewtonUserJointAddLinearRow (m_joint, &q0[0], &q1[0], &matrix0.m_right[0]);

				
			// the joint angle can be determine by getting the angle between any two non parallel vectors
			sinAngle = (matrix0.m_up * matrix1.m_up) % matrix0.m_front;
			cosAngle = matrix0.m_up % matrix1.m_up;
			angle = dAtan2 (sinAngle, cosAngle);

			relAngle = angle - m_baseAngle - m_parent->m_jointAngle;
	
			// tell joint error will minimize the exceeded angle error
			NewtonUserJointAddAngularRow (m_joint, relAngle, &matrix0.m_front[0]);
		}

		dFloat m_baseAngle;
		AstroChikenCalf *m_parent;
	};

	public:
	AstroChikenCalf (dSceneNode* parent, NewtonWorld* nWorld, const NewtonBody* parentBody, const dMatrix& matrix, dFloat radius, dFloat length, 
					 NewtonApplyForceAndTorque externaforce, dFloat side)
		:CapsulePrimitive (parent, matrix, radius, length)
	{
		// crate the physics for this vehicle
		NewtonBody* body;
		NewtonCollision* collision;

		dFloat mass = 10.0f;
		dVector size (radius * 2, radius * 2, length);
		dFloat Ixx = mass * (size.m_y * size.m_y + size.m_z * size.m_z) / 12.0f;
		dFloat Iyy = mass * (size.m_x * size.m_x + size.m_z * size.m_z) / 12.0f;
		dFloat Izz = mass * (size.m_x * size.m_x + size.m_y * size.m_y) / 12.0f;

		// ////////////////////////////////////////////////////////////////
		//
		//create the rigid body
		collision = NewtonCreateCapsule (nWorld, radius, length, NULL);
		body = NewtonCreateBody (nWorld, collision);

		// make sure body parts do not collide
		NewtonBodySetMaterialGroupID (body, metalID);


		// save the pointer to the graphic object with the body.
		NewtonBodySetUserData (body, this);

		// set the transform call back function
		NewtonBodySetTransformCallback (body, SetTransform);

		// set the force and torque call back function
		NewtonBodySetForceAndTorqueCallback (body, externaforce);

		// set the mass matrix
		NewtonBodySetMassMatrix (body, mass, Ixx, Iyy, Izz);

		// set the matrix for both the rigid body and the graphic body
		NewtonBodySetMatrix (body, &m_matrix[0][0]);

		// release the collision 
		NewtonReleaseCollision (nWorld, collision);	


		dVector pin (matrix.m_right);
		dVector pivot (matrix.m_posit - matrix.m_front.Scale(side * length * 0.5f));
		m_engine = new ThighEngine (this, pivot, pin, body, (NewtonBody*)parentBody, side);

	}

	~AstroChikenCalf()
	{
	}

	// Set the vehicle matrix and all tire matrices
	static void SetTransform (const NewtonBody* body, const dFloat* matrixPtr)
	{
		AstroChiken* vehicle;

		// get the graphic object form the rigid body
		vehicle = (AstroChiken*) NewtonBodyGetUserData (body);

		// set the transformation matrix for this rigid body
		dMatrix& matrix = *((dMatrix*)matrixPtr);
		vehicle->SetMatrix (matrix);
	}

	void SetAngle(dFloat angle)
	{
		m_jointAngle = angle;
	}

	dFloat m_jointAngle;
	ThighEngine* m_engine; 
};


class AstroChikenThight: public CapsulePrimitive
{

	class ThighEngine: public CustomHinge
	{
		public:
		ThighEngine(AstroChikenThight *me, const dVector& pivot, const dVector& pin, NewtonBody* child, NewtonBody* parent, dFloat side)
			:CustomHinge(pivot, pin, child, parent)
		{
			dFloat sinAngle;
			dFloat cosAngle;
			dMatrix matrix0;
			dMatrix matrix1;

			m_parent = me;
			// calculate the position of the pivot point and the Jacobian direction vectors, in global space. 
			CalculateGlobalMatrix (m_localMatrix0, m_localMatrix1, matrix0, matrix1);

			
			// the joint angle can be determine by getting the angle between any two non parallel vectors
			sinAngle = (matrix0.m_up * matrix1.m_up) % matrix0.m_front;
			cosAngle = matrix0.m_up % matrix1.m_up;
			m_baseAngle = dAtan2 (sinAngle, cosAngle) + side * 90.0f * 3.1416f / 180.0f;
		}

		void SubmitConstrainst ()
		{
			dFloat angle;
			dFloat sinAngle;
			dFloat cosAngle;
			dFloat relAngle;
			dMatrix matrix0;
			dMatrix matrix1;

			// calculate the position of the pivot point and the Jacobian direction vectors, in global space. 
			CalculateGlobalMatrix (m_localMatrix0, m_localMatrix1, matrix0, matrix1);

			// Restrict the movement on the pivot point along all tree orthonormal direction
			NewtonUserJointAddLinearRow (m_joint, &matrix0.m_posit[0], &matrix1.m_posit[0], &matrix0.m_front[0]);
			NewtonUserJointAddLinearRow (m_joint, &matrix0.m_posit[0], &matrix1.m_posit[0], &matrix0.m_up[0]);
			NewtonUserJointAddLinearRow (m_joint, &matrix0.m_posit[0], &matrix1.m_posit[0], &matrix0.m_right[0]);
			
			// get a point along the pin axis at some reasonable large distance from the pivot
			dVector q0 (matrix0.m_posit + matrix0.m_front.Scale(MIN_JOINT_PIN_LENGTH));
			dVector q1 (matrix1.m_posit + matrix1.m_front.Scale(MIN_JOINT_PIN_LENGTH));

			// two constraints row perpendicular to the pin vector
 			NewtonUserJointAddLinearRow (m_joint, &q0[0], &q1[0], &matrix0.m_up[0]);
			NewtonUserJointAddLinearRow (m_joint, &q0[0], &q1[0], &matrix0.m_right[0]);

				
			// the joint angle can be determine by getting the angle between any two non parallel vectors
			sinAngle = (matrix0.m_up * matrix1.m_up) % matrix0.m_front;
			cosAngle = matrix0.m_up % matrix1.m_up;
			angle = dAtan2 (sinAngle, cosAngle);

			//relAngle = angle - m_baseAngle;
			relAngle = angle + m_parent->m_jointAngle - m_baseAngle ;
	
			// tell joint error will minimize the exceeded angle error
			NewtonUserJointAddAngularRow (m_joint, relAngle, &matrix0.m_front[0]);
		}

		dFloat m_baseAngle;
		AstroChikenThight* m_parent;
	};


	public:
	AstroChikenThight (dSceneNode* parent, NewtonWorld* nWorld, const NewtonBody* parentBody, const dMatrix& matrix, dFloat radius, dFloat length, dFloat side, NewtonApplyForceAndTorque externaforce)
		:CapsulePrimitive (parent, matrix, radius, length)
	{
		// crate the physics for this vehicle
		NewtonBody* body;
		NewtonCollision* collision;

		m_side = side;
		m_jointAngle = 0.0f;
		dFloat mass = 10.0f;
		dVector size (radius * 2, radius * 2, length);
		dFloat Ixx = mass * (size.m_y * size.m_y + size.m_z * size.m_z) / 12.0f;
		dFloat Iyy = mass * (size.m_x * size.m_x + size.m_z * size.m_z) / 12.0f;
		dFloat Izz = mass * (size.m_x * size.m_x + size.m_y * size.m_y) / 12.0f;

		// ////////////////////////////////////////////////////////////////
		//
		//create the rigid body
		collision = NewtonCreateCapsule (nWorld, radius, length, NULL);
		body = NewtonCreateBody (nWorld, collision);

		// make sure body parts do not collide
		NewtonBodySetMaterialGroupID (body, metalID);

		// save the pointer to the graphic object with the body.
		NewtonBodySetUserData (body, this);

		// set the transform call back function
		NewtonBodySetTransformCallback (body, SetTransform);

		// set the force and torque call back function
		NewtonBodySetForceAndTorqueCallback (body, externaforce);

		// set the mass matrix
		NewtonBodySetMassMatrix (body, mass, Ixx, Iyy, Izz);

		// set the matrix for both the rigid body and the graphic body
		NewtonBodySetMatrix (body, &m_matrix[0][0]);

		// release the collision 
		NewtonReleaseCollision (nWorld, collision);	


		dVector pin (matrix.m_right);
		dVector pivot (matrix.m_posit - matrix.m_front.Scale(side * length * 0.5f));
		m_engine = new ThighEngine (this, pivot, pin, body, (NewtonBody*)parentBody, side);

		//
		dMatrix matrix1 (matrix);
		matrix1.m_posit += matrix.m_front.Scale (side * length * 0.5f);

		matrix1 = dgRollMatrix(-side * ((0.5f + 0.15) * 3.1416f)) * matrix1;

		length *= 1.0f;
	    matrix1.m_posit += matrix1.m_front.Scale (side * length * 0.5f);

		m_calf = new AstroChikenCalf (parent, nWorld, body, matrix1, radius, length, externaforce, side); 

	}

	~AstroChikenThight()
	{
	}

	void SetAngle(dFloat angle)
	{
		m_jointAngle = angle + 0.25 * 3.1416f * m_side;
		m_calf->SetAngle (angle * 0.5f + 0.25 * 3.1416f * m_side);
	}

	// Set the vehicle matrix and all tire matrices
	static void SetTransform (const NewtonBody* body, const dFloat* matrixPtr)
	{
		AstroChiken* vehicle;

		// get the graphic object form the rigid body
		vehicle = (AstroChiken*) NewtonBodyGetUserData (body);

		// set the transformation matrix for this rigid body
		dMatrix& matrix = *((dMatrix*)matrixPtr);
		vehicle->SetMatrix (matrix);
	}

	dFloat m_side;
	dFloat m_jointAngle;
	ThighEngine* m_engine; 
	AstroChikenCalf *m_calf;
	
};



class AstroChikenLeg: public CapsulePrimitive
{
	class LegEngine: public CustomUniversal
	{
		public:
		LegEngine(AstroChikenLeg* me, const dVector& pivot, const dVector& pin0, const dVector& pin1, NewtonBody* child, NewtonBody* parent)
			:CustomUniversal(pivot, pin0, pin1, child, parent)
		{
			m_lift = 0.0f;
			m_angle = 0.0f;
			m_parent = me;
		}

		void SubmitConstrainst ()
		{
			dFloat angle;
			dFloat relAngle;
			dFloat sinAngle;
			dFloat cosAngle;
			dMatrix matrix0;
			dMatrix matrix1;

			// calculate the position of the pivot point and the Jacobian direction vectors, in global space. 
			CalculateGlobalMatrix (m_localMatrix0, m_localMatrix1, matrix0, matrix1);

			// get the pin fixed to the first body
			const dVector& dir0 = matrix0.m_front;
			// get the pin fixed to the second body
			const dVector& dir1 = matrix1.m_up;

			// construct an orthogonal coordinate system with these two vectors
			dVector dir2 (dir0 * dir1);
			dVector dir3 (dir2 * dir0);
			dir3 = dir3.Scale (1.0f / dSqrt (dir3 % dir3));

			const dVector& p0 = matrix0.m_posit;
			const dVector& p1 = matrix1.m_posit;

			dVector q0 (p0 + dir3.Scale(MIN_JOINT_PIN_LENGTH));
			dVector q1 (p1 + dir1.Scale(MIN_JOINT_PIN_LENGTH));

			NewtonUserJointAddLinearRow (m_joint, &p0[0], &p1[0], &dir0[0]);
			NewtonUserJointSetRowStiffness (m_joint, 1.0f);

			NewtonUserJointAddLinearRow (m_joint, &p0[0], &p1[0], &dir1[0]);
			NewtonUserJointSetRowStiffness (m_joint, 1.0f);

			NewtonUserJointAddLinearRow (m_joint, &p0[0], &p1[0], &dir2[0]);
			NewtonUserJointSetRowStiffness (m_joint, 1.0f);

			NewtonUserJointAddLinearRow (m_joint, &q0[0], &q1[0], &dir0[0]);
			NewtonUserJointSetRowStiffness (m_joint, 1.0f);

			dFloat relOmega;
			dVector omega0 (0.0f, 0.0f, 0.0f);
			dVector omega1 (0.0f, 0.0f, 0.0f);
			// get relative angular velocity
			NewtonBodyGetOmega(m_body0, &omega0[0]);
			NewtonBodyGetOmega(m_body1, &omega1[0]);
			{
				// calculate the angle error
				sinAngle = (matrix0.m_front * matrix1.m_front) % matrix1.m_up;
				cosAngle = matrix0.m_front % matrix1.m_front;
				angle = dAtan2 (sinAngle, cosAngle);
				relAngle = angle - m_angle;
				// calculate the angle error derivative
				relOmega = (omega0 - omega1) % matrix1.m_up;
				NewtonUserJointAddAngularRow (m_joint, relAngle, &matrix1.m_up[0]);
			}

			{
				sinAngle = (matrix0.m_up * matrix1.m_up) % matrix0.m_front;
				cosAngle = matrix0.m_up % matrix1.m_up;
				angle = dAtan2 (sinAngle, cosAngle);

				// save the angle relative angle into the thigh
				m_parent->m_thight->SetAngle (angle);
				relAngle = angle - m_lift;
				// calculate the angle error derivative
				relOmega = (omega0 - omega1) % matrix0.m_front;
				NewtonUserJointAddAngularRow (m_joint, relAngle, &matrix0.m_front[0]);
			}
		 }


		 void SetTargetAngle (dFloat angle, dFloat lift)
		 {
			m_angle = angle;
			m_lift = lift;
		 }

		 dFloat m_lift;
		 dFloat m_angle;
		 AstroChikenLeg* m_parent;
	};


	public:
	AstroChikenLeg (dSceneNode* nodeParent, NewtonWorld* nWorld, AstroChiken* parent, const dMatrix& localMatrix, dFloat radius, dFloat length, dFloat side, NewtonApplyForceAndTorque externaforce)
		:CapsulePrimitive (nodeParent, GetIdentityMatrix(), radius, length)
	{
		// reposition the tire to  pivot around the tire local axis
		dMatrix matrix (dgYawMatrix (0.5f * 3.1416f) * localMatrix * parent->GetMatrix());
		SetMatrix (matrix);

		m_side = side;
		// crate the physics for this vehicle
		NewtonBody* body;
		NewtonCollision* collision;

		dFloat mass = 10.0f;
		dVector size (radius * 2, radius * 2, length);
		dFloat Ixx = mass * (size.m_y * size.m_y + size.m_z * size.m_z) / 12.0f;
		dFloat Iyy = mass * (size.m_x * size.m_x + size.m_z * size.m_z) / 12.0f;
		dFloat Izz = mass * (size.m_x * size.m_x + size.m_y * size.m_y) / 12.0f;

		// ////////////////////////////////////////////////////////////////
		//
		//create the rigid body
		collision = NewtonCreateCapsule (nWorld, radius, length, NULL);
		body = NewtonCreateBody (nWorld, collision);

		// set the material group id for vehicle
		NewtonBodySetMaterialGroupID (body, metalID);

		// save the pointer to the graphic object with the body.
		NewtonBodySetUserData (body, this);

		// set a destructor for this rigid body
//		NewtonBodySetDestructorCallback (body, DestroyVehicle);

		// set the transform call back function
		NewtonBodySetTransformCallback (body, SetTransform);

		// set the force and torque call back function
		NewtonBodySetForceAndTorqueCallback (body, externaforce);

		// set the mass matrix
		NewtonBodySetMassMatrix (body, mass, Ixx, Iyy, Izz);

		// set the matrix for both the rigid body and the graphic body
		NewtonBodySetMatrix (body, &m_matrix[0][0]);

		// release the collision 
		NewtonReleaseCollision (nWorld, collision);	

		dVector pin1 (matrix.m_up);
		dVector pin0 (matrix.m_right);
		dVector pivot (matrix.m_posit);

		m_engine = new LegEngine (this, pivot, pin0, pin1, body, parent->m_vehicleBody);


		matrix.m_posit += matrix.m_front.Scale (side * length * 0.5f);
		matrix = dgRollMatrix(side * 0.5f * 3.1416f) * matrix;
		length *= 2.0f;
	    matrix.m_posit += matrix.m_front.Scale (side * length * 0.5f);
 		m_thight = new AstroChikenThight (nodeParent, nWorld, body, matrix, radius, length, side, externaforce); 
	}

	~AstroChikenLeg()
	{
	}


	// Set the vehicle matrix and all tire matrices
	static void SetTransform (const NewtonBody* body, const dFloat* matrixPtr)
	{
		AstroChiken* vehicle;

		// get the graphic object form the rigid body
		vehicle = (AstroChiken*) NewtonBodyGetUserData (body);

		// set the transformation matrix for this rigid body
		dMatrix& matrix = *((dMatrix*)matrixPtr);
		vehicle->SetMatrix (matrix);
	}

	dFloat m_side;
	LegEngine* m_engine;
	AstroChikenThight *m_thight; 
};



AstroChiken::AstroChiken(dSceneNode* parent, NewtonWorld* nWorld, const dMatrix& matrix, NewtonApplyForceAndTorque externaforce)
	:RenderPrimitive(parent, matrix)
{
	static dFloat TankBody[][3] =
	{
		{  CHIKEN_LENGTH,  CHIKEN_HEIGHT, CHIKEN_WIDTH},
		{  CHIKEN_LENGTH,  CHIKEN_HEIGHT,-CHIKEN_WIDTH},
		{ -CHIKEN_LENGTH,  CHIKEN_HEIGHT,-CHIKEN_WIDTH},
		{ -CHIKEN_LENGTH,  CHIKEN_HEIGHT, CHIKEN_WIDTH},
					 							
		{  CHIKEN_LENGTH, -CHIKEN_HEIGHT, CHIKEN_WIDTH},
		{  CHIKEN_LENGTH, -CHIKEN_HEIGHT,-CHIKEN_WIDTH},
		{ -CHIKEN_LENGTH, -CHIKEN_HEIGHT,-CHIKEN_WIDTH},
		{ -CHIKEN_LENGTH, -CHIKEN_HEIGHT, CHIKEN_WIDTH},

		{ CHIKEN_LENGTH * 1.2f,  CHIKEN_HEIGHT * 0.8f,  CHIKEN_WIDTH * 0.8f},
		{ CHIKEN_LENGTH * 1.2f,  CHIKEN_HEIGHT * 0.8f, -CHIKEN_WIDTH * 0.8f},
		{-CHIKEN_LENGTH * 1.2f,  CHIKEN_HEIGHT * 0.8f, -CHIKEN_WIDTH * 0.8f},
		{-CHIKEN_LENGTH * 1.2f,  CHIKEN_HEIGHT * 0.8f,  CHIKEN_WIDTH * 0.8f},
		 					   										
		{ CHIKEN_LENGTH * 1.2f, -CHIKEN_HEIGHT * 0.8f,  CHIKEN_WIDTH * 0.8f},
		{ CHIKEN_LENGTH * 1.2f, -CHIKEN_HEIGHT * 0.8f, -CHIKEN_WIDTH * 0.8f},
		{-CHIKEN_LENGTH * 1.2f, -CHIKEN_HEIGHT * 0.8f, -CHIKEN_WIDTH * 0.8f},
		{-CHIKEN_LENGTH * 1.2f, -CHIKEN_HEIGHT * 0.8f,  CHIKEN_WIDTH * 0.8f},
	};

	int i;
	dFloat Ixx;
	dFloat Iyy;
	dFloat Izz;
	dFloat mass;
	NewtonCollision* vehicleCollision;

	m_angle = 0.0f;
	m_omega = 4.0f;
	m_amplitud = 2.0f;
	m_turnAmp = 0.0f;
	m_radius = 1.0f / (2.0f * m_amplitud) ;



	// create the vehicle body from the hull of a hull collision
	dMatrix collisioMatrix (GetIdentityMatrix ());
	collisioMatrix.m_posit.m_y = 0.5f;
	vehicleCollision = NewtonCreateConvexHull (nWorld, 16, &TankBody[0][0], 3 * sizeof (dFloat), &collisioMatrix[0][0]);
	GetShapeFromCollision (nWorld, vehicleCollision);

	// calculate the bbox
	dVector minBox (1.0e10f, 1.0e10f, 1.0e10f); 
	dVector maxBox (-1.0e10f, -1.0e10f, -1.0e10f); 
	for (i = 0; i < 16; i ++) {
		 dFloat x = TankBody[i][0];
		 dFloat y = TankBody[i][1];
		 dFloat z = TankBody[i][2];
	
		 minBox.m_x = min (minBox.m_x, x);
		 minBox.m_y = min (minBox.m_y, y);
		 minBox.m_z = min (minBox.m_z, z);
		 maxBox.m_x = max (maxBox.m_x, x);
		 maxBox.m_y = max (maxBox.m_y, y);
		 maxBox.m_z = max (maxBox.m_z, z);
	}


	m_boxSize = maxBox - minBox;
	m_boxOrigin = (maxBox + minBox).Scale (0.5f);

	// get the vehicle size and collision origin
	m_boxSize = maxBox - minBox;
	m_boxOrigin = (maxBox + minBox).Scale (0.5f);


	// crate the physics for this vehicle
    mass = 100.0f;
	dVector size (m_boxSize);
	Ixx = mass * (size.m_y * size.m_y + size.m_z * size.m_z) / 12.0f;
	Iyy = mass * (size.m_x * size.m_x + size.m_z * size.m_z) / 12.0f;
	Izz = mass * (size.m_x * size.m_x + size.m_y * size.m_y) / 12.0f;

	// ////////////////////////////////////////////////////////////////
	//
	//create the rigid body
	m_vehicleBody = NewtonCreateBody (nWorld, vehicleCollision);

	// save the pointer to the graphic object with the body.
	NewtonBodySetUserData (m_vehicleBody, this);

	// set the material group id for vehicle
//	NewtonBodySetMaterialGroupID (m_vehicleBody, vehicleID);

	// keep the body active all the time
	NewtonBodySetAutoFreeze (m_vehicleBody, 0);

	// set a destructor for this rigid body
	NewtonBodySetDestructorCallback (m_vehicleBody, DestroyVehicle);

	// set the transform call back function
	NewtonBodySetTransformCallback (m_vehicleBody, SetTransform);

	// set the force and torque call back function
	m_externaforce = externaforce;
	NewtonBodySetForceAndTorqueCallback (m_vehicleBody, Update);

	// set the mass matrix
	NewtonBodySetMassMatrix (m_vehicleBody, mass, Ixx, Iyy, Izz);

	// set the matrix for both the rigid body and the graphic body
	NewtonBodySetMatrix (m_vehicleBody, &m_matrix[0][0]);


	// release the collision 
	NewtonReleaseCollision (nWorld, vehicleCollision);	

	
	// get initial value for the tank tire placement
	dMatrix localMatrix (GetIdentityMatrix());
	dFloat lenght = 1.7 * CHIKEN_LENGTH;

	float radius = 0.3f;
	float length = radius * 2.5f;


	// procedurally place tires on the right side of the tank
	localMatrix.m_posit.m_z = -CHIKEN_WIDTH * 1.2f;
	localMatrix.m_posit.m_y = CHIKEN_HEIGHT;

	localMatrix.m_posit.m_x = - lenght / 2;
	m_leftLegs[0] = new AstroChikenLeg (parent, nWorld, this, localMatrix, radius, length, 1.0f, externaforce);

	localMatrix.m_posit.m_x = lenght / 2;
	m_leftLegs[1] = new AstroChikenLeg (parent, nWorld, this, localMatrix, radius, length, 1.0f, externaforce);

	localMatrix.m_posit.m_x = 0;
	m_leftLegs[2] = new AstroChikenLeg (parent, nWorld, this, localMatrix, radius, length, 1.0f, externaforce);


	// procedurally place tires on the left side of the tank
	localMatrix.m_posit.m_z = CHIKEN_WIDTH * 1.2f;

	localMatrix.m_posit.m_x = - lenght / 2;
	m_rightLegs[0] = new AstroChikenLeg (parent, nWorld, this, localMatrix, radius, length, -1.0f, externaforce);

	localMatrix.m_posit.m_x = lenght / 2;
	m_rightLegs[1] = new AstroChikenLeg (parent, nWorld, this, localMatrix, radius, length, -1.0f, externaforce);

	localMatrix.m_posit.m_x = 0.0f;
	m_rightLegs[2] = new AstroChikenLeg (parent, nWorld, this, localMatrix, radius, length, -1.0f, externaforce);


}

AstroChiken::~AstroChiken()
{
 	for (int i = 0; i < int (sizeof (m_rightLegs) / sizeof (AstroChikenLeg*)); i ++) {
		delete m_leftLegs[i];
		delete m_rightLegs[i];
	}
}


void AstroChiken::GetShapeFromCollision (const NewtonBody* body, int vertexCount, const dFloat* faceVertex, int id)
{
	int i;
	ConvexHullPrimitive* primitive;

	primitive = (ConvexHullPrimitive*) NewtonBodyGetUserData (body);

	glBegin(GL_POLYGON);

	// calculate the face normal for this polygon
	dVector normal (0.0f, 0.0f, 0.0f);
	dVector p0 (faceVertex[0 * 3 + 0], faceVertex[0 * 3 + 1], faceVertex[0 * 3 + 2]);
	dVector p1 (faceVertex[1 * 3 + 0], faceVertex[1 * 3 + 1], faceVertex[1 * 3 + 2]);
	for (i = 2; i < vertexCount; i ++) {
		dVector p2 (faceVertex[i * 3 + 0], faceVertex[i * 3 + 1], faceVertex[i * 3 + 2]);
		dVector dp0 (p1 - p0);
		dVector dp1 (p2 - p0);
		normal += dp0 * dp1;
	}

	normal = normal.Scale (dSqrt (1.0f / (normal % normal)));

	// submit the polygon to opengl
	glNormal3f(normal.m_x, normal.m_y, normal.m_z);
	for (i = 0; i < vertexCount; i ++) {
		dVector p0 (faceVertex[i * 3 + 0], faceVertex[i * 3 + 1], faceVertex[i * 3 + 2]);
		// calculate pseudo spherical mapping
		dVector tex(p0.Scale (1.0f / dSqrt (p0 % p0)));
		glTexCoord2f(dAsin (tex.m_x) / 3.1416f + 0.5f, dAsin (tex.m_z) / 3.1416f + 0.5f);
		glVertex3f(p0.m_x, p0.m_y, p0.m_z);
	}
	glEnd();
}


void AstroChiken::GetShapeFromCollision (NewtonWorld* nWorld, NewtonCollision* collision)
{
	NewtonBody *body;

	m_list = glGenLists(1);
	glNewList (m_list, GL_COMPILE);

	body = NewtonCreateBody (nWorld, collision);
	NewtonBodySetUserData (body, this);

	NewtonBodyForEachPolygonDo (body, GetShapeFromCollision);

	glEndList ();

	NewtonDestroyBody(nWorld, body);
}

// rigid body destructor
void AstroChiken::DestroyVehicle (const NewtonBody* body)
{
	AstroChiken* vehicle;

	// get the graphic object form the rigid body
	vehicle = (AstroChiken*) NewtonBodyGetUserData (body);

	// destroy the graphic object
	delete vehicle;
}


void AstroChiken::Render() const
{
//	if (!hideVehicle) {
		// set the color of the cube's surface
		GLfloat cubeColor[] = { 1.0f, 1.0f, 1.0f, 1.0 };
		glMaterialfv(GL_FRONT, GL_SPECULAR, cubeColor);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, cubeColor);
		glMaterialf(GL_FRONT, GL_SHININESS, 50.0);

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, m_texture);

		glPushMatrix();	
		glMultMatrix(&m_matrix[0][0]);
		glCallList (m_list);
		glPopMatrix();


		for (int i = 0; i < 3; i ++) {
			glPushMatrix();	
			m_leftLegs[i]->Render();
			glPopMatrix();

			glPushMatrix();	
			m_rightLegs[i]->Render();
			glPopMatrix();
		}
				
//	}
}


// Set the vehicle matrix and all tire matrices
void AstroChiken::SetTransform (const NewtonBody* body, const dFloat* matrixPtr)
{
	AstroChiken* vehicle;

	// get the graphic object form the rigid body
	vehicle = (AstroChiken*) NewtonBodyGetUserData (body);

	// set the transformation matrix for this rigid body
	dMatrix& matrix = *((dMatrix*)matrixPtr);
	vehicle->SetMatrix (matrix);
}

void AstroChiken::Update (const NewtonBody* body)
{
	AstroChiken* vehicle;

	// get the graphic object form the rigid body
	vehicle = (AstroChiken*) NewtonBodyGetUserData (body);

	// update external forces
	vehicle->m_externaforce(body); 

	vehicle->CalculateLegPosition();
}


void AstroChiken::CalculateLegPosition()
{
	dFloat time;
	dFloat rightAngle;
	dFloat leftAngle;

	dFloat rightLiftAngle;
	dFloat leftLiftAngle;

	NewtonWorld* world;

	world = NewtonBodyGetWorld (m_vehicleBody);

	time = NewtonGetTimeStep (world);
	m_angle = dMod (m_angle + m_omega * time, 2.0f * 3.1416);

	dFloat rightAmp = 1.0f;
	dFloat leftAmp = 1.0f;


	 dFloat liftMinAngle = 45.0f * 3.1516f / 180.0f;

	// calculate desired angles for the left side
	leftAngle = leftAmp * dAsin (m_radius * (m_amplitud + m_turnAmp) * dSin (m_angle));
	leftLiftAngle = dAsin (m_radius * (m_amplitud + m_turnAmp) * dCos (m_angle));
	m_leftLegs[0]->m_engine->SetTargetAngle (leftAngle, leftLiftAngle + liftMinAngle);
	m_leftLegs[1]->m_engine->SetTargetAngle (leftAngle, leftLiftAngle + liftMinAngle);

	leftAngle = leftAmp * dAsin (m_radius * (m_amplitud + m_turnAmp) * dSin (m_angle + 3.1416));
	leftLiftAngle = dAsin (m_radius * (m_amplitud + m_turnAmp) * dCos (m_angle + 3.1416));
	m_leftLegs[2]->m_engine->SetTargetAngle (leftAngle, leftLiftAngle + liftMinAngle);



	// calculate desired angles for the right side
	rightAngle = rightAmp * dAsin (m_radius * (m_amplitud - m_turnAmp) * dSin (m_angle + 3.1416));
	rightLiftAngle = dAsin (m_radius * (m_amplitud - m_turnAmp) * dCos (m_angle + 3.1416));
	m_rightLegs[2]->m_engine->SetTargetAngle (rightAngle, rightLiftAngle - liftMinAngle);

	rightAngle = rightAmp * dAsin (m_radius * (m_amplitud - m_turnAmp) * dSin (m_angle));
	rightLiftAngle = dAsin (m_radius * (m_amplitud - m_turnAmp) * dCos (m_angle));

	m_rightLegs[0]->m_engine->SetTargetAngle (rightAngle, rightLiftAngle - liftMinAngle);
	m_rightLegs[1]->m_engine->SetTargetAngle (rightAngle, rightLiftAngle - liftMinAngle);
 }
