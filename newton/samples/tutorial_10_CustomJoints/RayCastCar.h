// RayCastCar.h: interface for the RayCastCar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAYCASTCAR_H__17F4F804_E3BB_47BE_A070_99D85E263427__INCLUDED_)
#define AFX_RAYCASTCAR_H__17F4F804_E3BB_47BE_A070_99D85E263427__INCLUDED_


#include "..\COMMON\RenderPrimitive.h"


class CustomRayCastCar;

class RayCastCar : public RenderPrimitive  
{
	public:
	RayCastCar(NewtonWorld* nWorld, const dMatrix& matrix);
	virtual ~RayCastCar();


	protected:
	void Render() const;


	NewtonBody* m_carBody;
	CustomRayCastCar *m_carJoint;
};

#endif 
