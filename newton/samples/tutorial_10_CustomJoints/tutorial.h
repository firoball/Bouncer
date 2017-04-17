//********************************************************************
// Newton Game dynamics 
// copyright 2000-2004
// By Julio Jerez
// VC: 6.0
// simple demo list vector class with iterators
//********************************************************************

#ifndef __TUTORIAL_1734uedfksd87i__
#define __TUTORIAL_1734uedfksd87i__

#include "OpenGlUtil.h"
#include "Newton.h"

extern dVector cameraDir;
extern dVector cameraEyepoint;


void InitScene();
void DrawScene ();
int main(int argc, char **argv);

void  PhysicsBodyDestructor (const NewtonBody* body);
void  PhysicsApplyGravityForce (const NewtonBody* body);
void  PhysicsSetTransform (const NewtonBody* body, const dFloat* matrix);


#endif
  


 
 

  
  




