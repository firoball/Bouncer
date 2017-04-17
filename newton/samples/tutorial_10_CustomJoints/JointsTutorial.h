//********************************************************************
// Newton Game dynamics 
// copyright 2000-2004
// By Julio Jerez
// VC: 6.0
// simple demo list vector class with iterators
//********************************************************************

// JointsTutorial.h: interface for the JointsTutorial class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JOINTSTUTORIAL_H__7A1C323F_E267_4C98_BD55_3355B21F049E__INCLUDED_)
#define AFX_JOINTSTUTORIAL_H__7A1C323F_E267_4C98_BD55_3355B21F049E__INCLUDED_

#include "Newton.h"
#include "dMatrix.h"
#include "RenderPrimitive.h"


void AddRope (dSceneNode* parent, NewtonWorld* nWorld);
void AddVise (dSceneNode* parent, NewtonWorld* nWorld);
void AddGears (dSceneNode* parent, NewtonWorld* nWorld);
void AddRollingBeats (dSceneNode* parent, NewtonWorld* nWorld);
void AddDoubleSwingDoors (dSceneNode* parent, NewtonWorld* nWorld);

#endif 
