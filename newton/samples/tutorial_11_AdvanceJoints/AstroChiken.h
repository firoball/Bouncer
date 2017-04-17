// AstroChiken.h: interface for the AstroChiken class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ASTROCHIKEN_H__CFFD2B11_F9C5_493A_8173_CF03649DBEEB__INCLUDED_)
#define AFX_ASTROCHIKEN_H__CFFD2B11_F9C5_493A_8173_CF03649DBEEB__INCLUDED_


#include "RenderPrimitive.h"

class AstroChikenLeg;

class AstroChiken : public RenderPrimitive
{
	public:
	AstroChiken(dSceneNode* parent, NewtonWorld* nWorld, const dMatrix& matrix, NewtonApplyForceAndTorque externaforce);
	virtual ~AstroChiken();

	NewtonBody* GetRigidBody() const;

	protected:
	static void Update (const NewtonBody* body);
	static void  DestroyVehicle (const NewtonBody* body);
	static void  SetTransform (const NewtonBody* body, const dFloat* matrixPtr);
	static void GetShapeFromCollision (const NewtonBody* body, int vertexCount, const dFloat* faceVertex, int id);

	virtual void Render() const;
	void GetShapeFromCollision (NewtonWorld* nWorld, NewtonCollision* collision);


	void CalculateLegPosition();
	
	GLuint m_list;
	dVector m_boxSize;
	dVector m_boxOrigin;
	NewtonBody* m_vehicleBody;


	dFloat m_angle;
	dFloat m_omega;
	dFloat m_amplitud;
	dFloat m_turnAmp;
	dFloat m_radius;
	

	AstroChikenLeg* m_leftLegs[3];
	AstroChikenLeg* m_rightLegs[3];

	NewtonApplyForceAndTorque m_externaforce;

	friend class AstroChikenLeg;
};

#endif // !defined(AFX_ASTROCHIKEN_H__CFFD2B11_F9C5_493A_8173_CF03649DBEEB__INCLUDED_)
