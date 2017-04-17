// RayCastCar.cpp: implementation of the RayCastCar class.
//
//////////////////////////////////////////////////////////////////////
#include <stdafx.h>
#include "RayCastCar.h"
#include "RenderPrimitive.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RayCastCar::RayCastCar(NewtonWorld* nWorld, const dMatrix& matrix)
	:RenderPrimitive (matrix)
{



}

RayCastCar::~RayCastCar()
{

}


void RayCastCar::Render() const
{
/*
	// tire primitive are align different do 
	static dMatrix localMatrix (dgYawMatrix (3.1416 * 0.5f));
	if (!hideVehicle) {
		//ChamferCylinderPrimitive::Render();

		GLfloat cubeColor[] = { 1.0f, 1.0f, 1.0f, 1.0 };
		glMaterialfv(GL_FRONT, GL_SPECULAR, cubeColor);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, cubeColor);
		glMaterialf(GL_FRONT, GL_SHININESS, 50.0);

		dMatrix matrix (localMatrix * m_matrix);
		glMultMatrixf(&matrix[0][0]);

		// set up the cube's texture
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glBindTexture(GL_TEXTURE_2D, g_tireTexture);
		glCallList (m_list);
	}
*/
}

